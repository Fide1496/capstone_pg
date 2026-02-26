// ============================================================================
//  parser.cpp — Recursive-descent parser 
// ----------------------------------------------------------------------------
// MSU CSE 4714/6714 Capstone Project (Spring 2026)
// Author: Derek Willis
// ============================================================================

#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <set>
#include "lexer.h"
#include "ast.h"
#include "debug.h"
using namespace std;

// -----------------------------------------------------------------------------
// One-token lookahead
// -----------------------------------------------------------------------------
bool   havePeek = false;
Token  peekTok  = 0;
string peekLex;

inline const char* tname(Token t) { return tokName(t); }

Token peek() 
{
  if (!havePeek) {
    peekTok = yylex();
    if (peekTok == 0) { peekTok = TOK_EOF; peekLex.clear(); }
    else              { peekLex = yytext ? string(yytext) : string(); }
    dbg::line(string("peek: ") + tname(peekTok) + (peekLex.empty() ? "" : " ["+peekLex+"]")
              + " @ line " + to_string(yylineno));
    havePeek = true;
  }
  return peekTok;
}
Token nextTok() 
{
  Token t = peek();
  dbg::line(string("consume: ") + tname(t));
  havePeek = false;
  return t;
}
Token expect(Token want, const char* msg) 
{
  Token got = nextTok();
  if (got != want) {
    dbg::line(string("expect FAIL: wanted ") + tname(want) + ", got " + tname(got));
    ostringstream oss;
    oss << "Parse error (line " << yylineno << "): expected "
        << tname(want) << " — " << msg << ", got " << tname(got)
        << " [" << (yytext ? yytext : "") << "]";
    throw runtime_error(oss.str());
  }
  return got;
}


// TODO: implement parsing functions for each grammar in your language

unique_ptr<Block> parseBlock(){

    auto block = make_unique<Block>();

    // if (peek() == Compound) {
    //   expect(Compound, "expected Compound in block");
    //   block->compound = parseCompound();
    // }
    
    // expect(block, "expected block");
    // block->compound = peekLex;
   

    return block;

  // Start by creating a pointer to the node we need


  // Step through the grammar, storing anything necessary as member variables

  
  // When done with the grammar, return the pointer to our node
  return 0;
}

// -----------------------------------------------------------------------------
// Program → PROGRAM IDENT ';' Block EOF
// -----------------------------------------------------------------------------
unique_ptr<Program> parseProgram() {
  // Make a pointer to the node we need to build
  auto p = make_unique<Program>();
  // Step through the grammar, storing anything necessary as member variables
  expect(PROGRAM, "start of program");
  expect(IDENT, "program name");
  // Store the program name 
  p->name  = peekLex;
  expect(SEMICOLON, "after program name");
  // Store a pointer to the appropriate block
  p->block = parseBlock();
  expect(TOK_EOF, "at end of file (no trailing tokens after program)");
  // Nothing left in the grammar so we return our node pointer
  return p;
}

// -----------------------------------------------------------------------------
// Parser entry point (called by driver)
// -----------------------------------------------------------------------------
// *****************************************************
// To test piece-wise change the pointer type below
unique_ptr<Program> parse()
// *****************************************************
{
  // Reset lookahead state for a fresh parse
  havePeek = false;
  peekTok = 0;
  peekLex.clear();
  
  // *****************************************************
  // To test piece-wise change the parser function you set as your root
  auto root = parseProgram();
  // *****************************************************

  // Ensure no extra tokens remain
  if (peek() != EOF) {
    ostringstream oss;
    oss << "Parse error (line " << yylineno << "): extra tokens after <program>, got "
        << tname(peekTok) << " [" << (yytext ? yytext : "") << "]";
    throw runtime_error(oss.str());
  }

  return root;
}