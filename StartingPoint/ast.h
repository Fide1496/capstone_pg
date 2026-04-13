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
#include <map>
#include <variant>
#include <type_traits>
using namespace std;

// -----------------------------------------------------------------------------
// External symbol table
// -----------------------------------------------------------------------------
inline map<string, variant<int,double>> symbolTable;

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


struct Statement{
  
  vector<unique_ptr<Statement>> statements;

  // Virtual functions for abstract base class
  virtual void print_tree(ostream&,string, bool) {}
  virtual void interpret(ostream&) {}
};


struct Spawn{


  void print_tree(ostream& out, string prefix, bool last){
    ast_line(out, prefix, last, "Spawn Statement");
    string child_prefix = prefix + (last ? "    ": "|   ");
  }

  void interpret(ostream& out) {
    (void)out;
  }
};

struct Primary{

  vector<unique_ptr<Primary>> primaries;

  virtual void print_tree(ostream&,string, bool) {}
  virtual void interpret(ostream&) {}

};

struct Value : public Primary{
  string value;
  Token type;

  void print_tree(ostream& out, string prefix, bool last){
    ast_line(out, prefix, last, "Value: " + value);
  }

  void interpret(ostream& out) {
    (void)out;
  }
};

struct Term : public Primary{

};

struct Factor : public Primary{
  string value;
  Token type;

  void print_tree(ostream& out, string prefix, bool last){
    ast_line(out, prefix, last, "Factor: " + value);
  }

  void interpret(ostream& out) {
    (void)out;
  }
};

struct AssignStmt : public Statement{

  string id;
  Token type;
  unique_ptr<Value> value;

  void print_tree(ostream& out, string prefix, bool last){
    ast_line(out, prefix, last, "Assign Statement");
    string child_prefix = prefix + (last ? "    ": "|   ");
  }

  void interpret(ostream& out) override {
    (void)out;
    auto& lhs = symbolTable[id]; 
    visit([&](auto& slot) {
      using T = decay_t<decltype(slot)>; 
      if (type == INTLIT) {
        slot = static_cast<T>(stoi(value));
      } else if (type == FLOATLIT) {
        slot = static_cast<T>(stod(value));
      } else {
        auto& rhs = symbolTable[value]; 
        visit([&](auto r) { slot = static_cast<T>(r); }, rhs);
      }
    }, lhs);
  }

};

struct ReadStmt : public Statement{

  string target;

  void print_tree(ostream& out, string prefix, bool last){
    ast_line(out, prefix, last, "Read Statement");
    string child_prefix = prefix + (last ? "    ": "|   ");
  }

  void interpret(ostream& out) override {
    (void)out;
    auto it = symbolTable.find(target);
    if (it != symbolTable.end()) {
      visit([&](auto& value) { cin >> value; }, it->second);
    }
  }
};

struct WriteStmt : public Statement{

  string content;
  Token type;

  void print_tree(ostream& out, string prefix, bool last){

    // cout<<"Inside write statement print tree func\n";
    ast_line(out, prefix, last, "Write Statement");
    string child_prefix = prefix + (last ? "    ": "|   ");
    ast_line(out, child_prefix, true, "String Literal: " + content);
  }

  void interpret(ostream& out) override {
    if (type == IDENT) {
      auto it = symbolTable.find(content);
      if (it != symbolTable.end()) {
        visit([&out](auto&& value) { out << value << endl; }, it->second);
      } else {
        out << "undefined variable" << endl;
      }
    } else {
      out << content << endl;
    }
  }

};

struct CompoundStmt : public Statement{
  vector<unique_ptr<Statement>> statements;

  void print_tree(ostream& out, string prefix, bool last){
    // cout<<"Inside compound statement print tree func\n";

    ast_line(out, prefix, last, "Compound Statement");
    string child_prefix = prefix + (last ? "    ": "|   ");
    for (const auto& stmt: statements){
      stmt->print_tree(out, child_prefix, last);
    }
  }

  void interpret(ostream& out){
    // cout<<"Inside compound statement interpret func\n";
    for (const auto& stmt: statements){
      stmt->interpret(out);
    }
  }

};


// TODO: Finish this struct for Block
struct Block
{
  // TODO: Declare Any Member Variables
  unique_ptr<CompoundStmt> compound;
  

  // Member Function to Print
  void print_tree(ostream& out,string prefix,bool last){
    // TODO: Finish this function

    cout << "Block\n";

    compound->print_tree(out, prefix, last);
    
  }

  // Member Function to Interpret
  void interpret(ostream& out){
    if (compound) compound->interpret(out); 
    // cout<<"inside block interpret func" << out;
  
  }
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

=============================================================================
  ast.h 
=============================================================================
MSU CSE 4714/6714 Capstone Project (Spring 2026)
Author: Derek Willis
=============================================================================
