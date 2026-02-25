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

// TODO: Finish this struct for Block
struct Block
{
  // TODO: Declare Any Member Variables

  // Member Function to Print
  void print_tree(ostream&,string,bool){
    // TODO: Finish this function
  };

  // Member Function to Interpret
  void interpret(ostream&){
    // TODO: Finish this function
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


