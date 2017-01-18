#include "gtest/gtest.h"
#include "config_parser.h"





TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

class NginxStringConfigTest : public ::testing::Test {
protected:
  bool ParseString(const std::string config_string) {
    std::stringstream config_stream(config_string);
    return parser_.Parse(&config_stream, &config_);
  }
  NginxConfigParser parser_;
  NginxConfig config_; 
};


TEST(NginxConfigTest, ToString) {
  NginxConfigStatement statement;
  statement.tokens_.push_back("foo");
  statement.tokens_.push_back("bar");

  EXPECT_EQ(statement.ToString(0), "foo bar;\n");
}

TEST(NginxConfigParserTest, AnotherSimpleConfig) {
  std::string config_string = "foo bar;";
  std::stringstream config_stream(config_string);
  NginxConfigParser parser;
  NginxConfig config;
  EXPECT_TRUE(parser.Parse(&config_stream,&config)) << "Parsing failed";
  EXPECT_EQ(config.statements_.size(),1) << "Number of statements is false";

}


TEST(NginxConfigParserTest, FailedConfig) {
  std::string config_string = "foo bar";
  std::stringstream config_stream(config_string);
  NginxConfigParser parser;
  NginxConfig config;
  EXPECT_FALSE(parser.Parse(&config_stream,&config));

}

TEST_F(NginxStringConfigTest, TestingUnbalancedBraces) {
  EXPECT_FALSE(ParseString("host { 80;")) << "Failed curly brace test";

}

TEST_F(NginxStringConfigTest, MultiLineConfig) {
  EXPECT_TRUE(ParseString("proxy_redirect\t\toff;\nproxy_set_header\t\tHost\t\t$host;\nproxy_set_header\t\tX-Real-IP\t\t$remote_addr;\n"));
  EXPECT_EQ(config_.statements_.size(),3);
  EXPECT_EQ(config_.statements_[0]->tokens_.size(),2);
  EXPECT_EQ(config_.statements_[1]->tokens_.size(),3);
  EXPECT_EQ(config_.statements_[2]->tokens_.size(),3);
}
TEST_F(NginxStringConfigTest, NestedConfig) {
  EXPECT_TRUE(ParseString("http {\n\tinclude\tcong/mime.types\n\tserver {\n\t\tlisten 80;\n\t\troot html;\n\t}\n}"));
  EXPECT_EQ(config_.statements_.size(),1);
  ASSERT_NE(config_.statements_[0]->child_block_,nullptr);
}


//fix unbalanced curly braces
