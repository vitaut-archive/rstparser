/*
 reStructuredText parser tests.

 Copyright (c) 2012, Victor Zverovich
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <gtest/gtest.h>

#ifdef _WIN32
# include <crtdbg.h>
#endif

#include "rstparser.h"

namespace {

class TestHandler : public rst::ContentHandler {
 private:
  std::string tag_;
  std::string content_;

 public:
  const std::string &content() const { return content_; }

  void StartBlock(rst::BlockType type) {
    switch (type) {
    case rst::PARAGRAPH:
      tag_ = "p";
      break;
    case rst::BLOCKQUOTE:
      tag_ = "blockquote";
      break;
    }
    content_ += "<" + tag_ + ">";
  }

  void EndBlock() {
    content_ += "</" + tag_ + ">";
  }

  void HandleText(const char *text, std::size_t size) {
    content_.append(text, size);
  }
};

std::string Parse(const char *s) {
  TestHandler handler;
  rst::Parser parser(&handler);
  parser.Parse(s);
  return handler.content();
}
}

TEST(ParserTest, Paragraph) {
  EXPECT_EQ("<p>test</p>", Parse("test"));
  EXPECT_EQ("<p>test</p>", Parse("\ntest"));
}

TEST(ParserTest, BlockQuote) {
  EXPECT_EQ("<blockquote>test</blockquote>", Parse(" test"));
}

TEST(ParserTest, PreserveInnerSpace) {
  EXPECT_EQ("<p>a  b</p>", Parse("a  b"));
}

TEST(ParserTest, ReplaceWhitespace) {
  EXPECT_EQ("<p>a       b</p>", Parse("a\tb"));
  EXPECT_EQ("<blockquote>a      b</blockquote>", Parse(" a\tb"));
  EXPECT_EQ("<p>a b</p>", Parse("a\vb"));
}

TEST(ParserTest, StripTrailingSpace) {
  EXPECT_EQ("<p>test</p>", Parse("test \t"));
}

TEST(ParserTest, MultiLineBlock) {
  EXPECT_EQ("<p>line 1\nline 2</p>", Parse("line 1\nline 2"));
}

TEST(ParserTest, UnindentBlock) {
  EXPECT_EQ("<blockquote>abc</blockquote><p>def</p>", Parse(" abc\ndef"));
}

int main(int argc, char **argv) {
#ifdef _WIN32
  // Disable message boxes on assertion failures.
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
