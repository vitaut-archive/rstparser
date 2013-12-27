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

#ifndef RSTPARSER_H_
#define RSTPARSER_H_

#include <memory>
#include <vector>

namespace rst {

// A document tree node.
class Node {
 private:
  // Do not implement!
  Node(const Node &);
  void operator=(const Node &);

 public:
  Node() {}
};

// A paragraph.
class Paragraph : public Node {
 private:
  std::string text_;
};

// A compound node, i.e. the one that may contain other nodes.
class CompoundNode : public Node {
private:
  std::vector<Node*> children_;
  
public:
  virtual ~CompoundNode() {
    for (std::vector<Node*>::iterator
        i = children_.begin(), e = children_.end(); i != e; ++i) {
      delete *i;
    }
  }

  void AddChild(std::auto_ptr<Node> child) {
    children_.push_back(child.get());
    child.release();
  }
};

// A parser for a subset of reStructuredText.
class Parser {
 private:
  // Parses a paragraph.
  std::auto_ptr<Node> ParseParagraph(const char *s) {
    std::string text;
    for (;;) {
      const char *line_start = s;
      while (*s && *s != '\n')
        ++s;
      if (s == '\n')
        ++s;
      text.append(line_start, s);
      // Skip whitespace.
      while (std::isspace(*s))
        ++s;
      if (*s == '\n') {
        ++s;
        break;  // Empty line ends the paragraph.
      }
      if (!*s)
        break;  // End of input.
    }
    return std::auto_ptr<Node>(new Paragraph(text));
  }

  // Parses a block node.
  std::auto_ptr<Node> ParseBlockNode(const char *s) {
    // Skip empty lines.
    while (*s) {
      // Skip whitespace.
      while (std::isspace(*s))
        ++s;
      if (*s != '\n')
        break;
      ++s;
    }
    char DIRECTIVE_START[] = ".. ";
    const size_t DIRECTIVE_START_LEN = sizeof(DIRECTIVE_START) - 1;
    if (std::strncmp(s, DIRECTIVE_START, DIRECTIVE_START_LEN) == 0) {
      s += DIRECTIVE_START_LEN;
      // TODO: parse directive name, then "::"
    }
    return ParseParagraph(s);
  }

 public:
  // Parses a string containing reStructuredText and returns a document node.
  std::auto_ptr<CompoundNode> Parse(const char *s) {
    std::auto_ptr<CompoundNode> document(new CompoundNode());
    while (*s) {
      std::auto_ptr<Node> node = ParseBlockNode(s);
      if (node)
        document->AddChild(node);
    }
    return document;
  }
};
}

#endif  // RSTPARSER_H_

