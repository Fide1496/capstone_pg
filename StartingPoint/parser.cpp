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


// Function declarations
unique_ptr<Statement> parseStatement();
unique_ptr<CompoundStmt> parseCompound();
unique_ptr<WriteStmt> parseWrite();
unique_ptr<Block> parseBlock();

// TODO: implement parsing functions for each grammar in your language
unique_ptr<Statement> parseStatement(){

  if (peek() == TOK_BEGIN){
    return parseCompound();
  }
  else if(peek() == WRITE){
    return parseWrite();
  }
  // else{
  //   cout<<"IDK what to do here but just cuz";
  // }
}

unique_ptr<WriteStmt> parseWrite(){
  // Make a pointer to the node we need to build
  auto write = make_unique<WriteStmt>();
  // Step through the grammar, storing anything necessary as member variables
  expect(WRITE, "start of write block");
  expect(OPENPAREN, "open parenthases");
  // Store string literal variable
  expect(STRINGLIT,"string literal");
  // write->stringlit  = peekLex;
  expect(CLOSEPAREN, "close parenthases");
  
  // Nothing left in the grammar so we return our node pointer
  return write;
}

unique_ptr<CompoundStmt> parseCompound(){
  auto compound = make_unique<CompoundStmt>();

  expect(TOK_BEGIN, "begin token");

  compound->statements.push_back(parseStatement());

  // Handle multiple optional statements
  while(peek() == SEMICOLON){
    expect(SEMICOLON, "semicolon");
    compound->statements.push_back(parseStatement());
  }

  expect(END, "end of compound stmt");

  return compound;
}


unique_ptr<Block> parseBlock(){
  // Start by creating a pointer to the node we need
  auto b = make_unique<Block>();

  b->compound = parseCompound();
  // Step through the grammar, storing anything necessary as member variables
  
  
  // When done with the grammar, return the pointer to our node
  return b;
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
  if (peek() != TOK_EOF) {
    ostringstream oss;
    oss << "Parse error (line " << yylineno << "): extra tokens after <program>, got "
        << tname(peekTok) << " [" << (yytext ? yytext : "") << "]";
    throw runtime_error(oss.str());
  }

  return root;
}