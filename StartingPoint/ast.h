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

// program → PROGRAM IDENT SEMICOLON block
// block → compound
// statement → compound | write
// compound → TOK_BEGIN statement { SEMICOLON statement } END
// write → WRITE OPENPAREN STRINGLIT CLOSEPAREN

struct Statement{
  
  vector<unique_ptr<Statement>> statements;

  // Virtual functions for abstract base class
  virtual void print_tree(ostream&,string,bool){
  }
  virtual void interpret(ostream&){
  }
};

struct WriteStmt : public Statement{

  string stringlit;

  void print_tree(ostream& out, string prefix){

    cout<<"Inside write statement print tree func\n";
    ast_line(out, prefix, true, "Compound");
  }

  void interpret(){

    cout<<"Inside write statement interpret func\n";
  }

};

struct CompoundStmt : public Statement{
  vector<unique_ptr<Statement>> statements;

  void print_tree(ostream& out, string prefix){
    cout<<"Inside compound statement print tree func\n";
    ast_line(out, prefix, false, "CompoundStmt");
  }

  void interpret(ostream& out){
    cout<<"Inside compound statement interpret func\n";
  }

};



// TODO: Finish this struct for Block
struct Block
{
  // TODO: Declare Any Member Variables
  unique_ptr<CompoundStmt> compound;
  

  // Member Function to Print
  void print_tree(ostream& os,string prefix,bool last){
    // TODO: Finish this function

    cout << "Block\n";

    compound->print_tree(os, "");
    
  }

  // Member Function to Interpret
  void interpret(ostream& out){
    // TODO: Finish this function
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


