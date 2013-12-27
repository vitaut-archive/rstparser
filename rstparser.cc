/*
 A reStructuredText parser written in C++.

 Copyright (c) 2013, Victor Zverovich
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

#include "rstparser.h"

#include <cstring>

rst::ContentHandler::~ContentHandler() {}

void rst::Parser::SkipSpace() {
  while (std::isspace(*ptr_) && *ptr_ != '\n')
    ++ptr_;
}

void rst::Parser::ParseBlock(rst::BlockType type, int indent) {
  handler_->StartBlock(type);
  std::string text;
  for (bool first = true; ;first = false) {
    const char *line_start = ptr_;
    if (!first) {
      // Check indentation.
      SkipSpace();
      if (ptr_ - line_start != indent)
        break;
    }

    // Find the end of the line.
    while (*ptr_ && *ptr_ != '\n')
      ++ptr_;

    // Strip whitespace at the end of the line.
    const char *end = ptr_;
    while (end != line_start && std::isspace(end[-1]))
      --end;

    // Copy text converting all whitespace characters to spaces.
    text.reserve(end - line_start + 1);
    if (!first)
      text.push_back('\n');
    enum {TAB_WIDTH = 8};
    for (const char *s = line_start; s != end; ++s) {
      char c = *s;
      if (c == '\t') {
        text.append("        ",
            TAB_WIDTH - ((indent + s - line_start) % TAB_WIDTH));
      } else if (std::isspace(c)) {
        text.push_back(' ');
      } else {
        text.push_back(*s);
      }
    }
    if (*ptr_ == '\n')
      ++ptr_;
    SkipSpace();
    if (*ptr_ == '\n') {
      ++ptr_;
      break;  // Empty line ends the paragraph.
    }
    if (!*ptr_)
      break;  // End of input.
  }
  if (*text.rbegin() == '\n')
    text.resize(text.size() - 1);
  handler_->HandleText(text.c_str(), text.size());
  handler_->EndBlock();
}

void rst::Parser::Parse(const char *s) {
  ptr_ = s;
  while (*ptr_) {
    // Skip whitespace and empty lines.
    const char *line_start = ptr_;
    SkipSpace();
    if (*ptr_ == '\n') {
      ++ptr_;
      continue;
    }
    switch (*ptr_) {
    case '.':
      if (ptr_[1] == '.' && ptr_[2] == ' ') {
        // TODO: parse directive name, then "::"
      }
      break;
    case '*':
      // TODO: parse list
      break;
    }
    ParseBlock(std::isspace(line_start[0]) ? BLOCKQUOTE : PARAGRAPH,
        ptr_ - line_start);
  }
}
