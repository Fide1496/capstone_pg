// =============================================================================
//   ast.h 
// =============================================================================
// MSU CSE 4714/6714 Capstone Project (Spring 2026)
// Author: Derek Willis
// =============================================================================
#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

// -----------------------------------------------------------------------------
// Pretty printer
// -----------------------------------------------------------------------------
inline void ast_line(ostream& os, string prefix, bool last, string label) {
  os << prefix << (last ? "└── " : "├── ") << label << "\n";
}

// TODO: Define and Implement structures to hold each data node!
// Tip: Build with the root of your tree as the lowest struct in this file
//      Implement each higher node in the tree HIGHER up in this file than its children
//      i.e. The root struct at the bottom of the file
//           The leaves of the tree toward the top of the file

// Struct Definitions
struct Statement;
struct CompoundStmt;
struct WriteStmt;
struct Block;
struct Program;
struct AssignStmt;
struct ReadStmt;


struct Statement {

  vector<unique_ptr<Statement>> statements;

  virtual void print_tree(ostream& os, string prefix, bool last) = 0;
  virtual void interpret(ostream& out) = 0;
};

struct CompoundStmt : public Statement {

  vector<unique_ptr<Statement>> statements;

  // void print_tree(ostream& os, string prefix, bool last) {
  //   ast_line(os, prefix, last, "Compound");
  //   string child_prefix = prefix + (last ? "    " : "│   ");
  //   for (size_t i = 0; i < statements.size(); ++i) {
  //     statements[i]->print_tree(os, child_prefix, i == statements.size() - 1);
  //   }
  // }

  void print_tree(ostream& os, string prefix, bool last) {
    ast_line(os, prefix, last, "Compound");
    string child_prefix = prefix + (last ? "    " : "│   ");
    for (size_t i = 0; i < statements.size(); ++i) {
      statements[i]->print_tree(os, child_prefix, i == statements.size() - 1);
    }
  }

  void interpret(ostream& out) {
    for (const auto& stmt : statements) {
      stmt->interpret(out);
    }
  }
};

struct WriteStmt : public Statement {

  string string_lit;
  Token type;

  void print_tree(ostream& os, string prefix, bool last) {
    ast_line(os, prefix, last, "Write");
    string child_prefix = prefix + (last ? "    " : "│   ");
    ast_line(os, child_prefix, true, "StringLit: " + string_lit);
  }

  void interpret(ostream& out) {
    out << string_lit << endl;
  }

};


// TODO: Finish this struct for Block
struct Block
{
  // TODO: Declare Any Member Variables
  unique_ptr<CompoundStmt> compound = make_unique<CompoundStmt>();

  // Member Function to Print
  void print_tree(ostream& os, string prefix, bool last){
    ast_line(os, prefix, last, "Block");
    string child_prefix = prefix + (last ? "    " : "│   ");

    if (compound) {
      compound->print_tree(os, child_prefix, true);
    } else {
      ast_line(os, child_prefix, true, "(empty)");
    }
  };

  // Member Function to Interpret
  void interpret(ostream& out){
    // TODO: Finish this function
    compound->interpret(out);
  };
};

// You do not need to edit this struct, but can if you choose
struct Program 
{
  // Member Variables
  string name; 
  unique_ptr<Block> block;

  // Member Functions
  void print_tree(ostream& os)
  {
    cout << "Program\n";
    ast_line(os, "", false, "name: " + name);
    if (block) block->print_tree(os, "", true);
    else 
    { 
      ast_line(os, "", true, "Block"); 
      ast_line(os, "    ", true, "(empty)");
    }
  }

  void interpret(ostream& out) 
  { 
    if (block) block->interpret(out); 
  }
};

struct AssignStmt{
  string id;
  Token type;
  string value;

  void print_tree(ostream& os, string prefix, bool last) {
    ast_line(os, prefix, last, "Assign");
    string child_prefix = prefix + (last ? "    " : "│   ");
    ast_line(os, child_prefix, true, "ID: " + id);
    ast_line(os, child_prefix, true, "Type: " + tokName(type));
    ast_line(os, child_prefix, true, "Value: " + value);
  }
  void interpreet(ostream& out) {

  }
  
};

struct ReadStmt{
  string target;

  void print_tree(ostream& os, string prefix, bool last) {
    ast_line(os, prefix, last, "Read");
    string child_prefix = prefix + (last ? "    " : "│   ");
    ast_line(os, child_prefix, true, "Target: " + target);
  }
  void interpret(ostream& out) {

  }

};

// program → PROGRAM IDENT SEMICOLON block
// block → compound
// statement → compound | write
// compound → TOK_BEGIN statement { SEMICOLON statement } END
// write → WRITE OPENPAREN STRINGLIT CLOSEPAREN
