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

void rst::Parser::ParseParagraph() {
  handler_->StartParagraph();
  std::string text;
  for (;;) {
    const char *line_start = ptr_;
    while (*ptr_ && *ptr_ != '\n')
      ++ptr_;
    if (*ptr_ == '\n')
      ++ptr_;
    text.append(line_start, ptr_);
    // Skip whitespace.
    while (std::isspace(*ptr_))
      ++ptr_;
    if (*ptr_ == '\n') {
      ++ptr_;
      break;  // Empty line ends the paragraph.
    }
    if (!*ptr_)
      break;  // End of input.
  }
  handler_->HandleText(text.c_str(), text.size());
  handler_->EndParagraph();
}

void rst::Parser::ParseBlockNode() {
  // Skip empty lines.
  while (*ptr_) {
    // Skip whitespace.
    while (std::isspace(*ptr_))
      ++ptr_;
    if (*ptr_ != '\n')
      break;
    ++ptr_;
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
  ParseParagraph();
}

void rst::Parser::Parse(const char *s) {
  ptr_ = s;
  while (*ptr_)
    ParseBlockNode();
}
