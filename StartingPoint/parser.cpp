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
#include <map>
#include <variant>
#include "lexer.h"
#include "ast.h"
#include "debug.h"
using namespace std;

// -----------------------------------------------------------------------------
// One-token lookahead
// -----------------------------------------------------------------------------
bool havePeek = false;
Token peekTok = 0;
string peekLex;

extern map<string, variant<int, double>> symbolTable;
inline const char *tname(Token t) { return tokName(t); }

Token peek()
{
  if (!havePeek)
  {
    peekTok = yylex();
    if (peekTok == 0)
    {
      peekTok = TOK_EOF;
      peekLex.clear();
    }
    else
    {
      peekLex = yytext ? string(yytext) : string();
    }
    dbg::line(string("peek: ") + tname(peekTok) + (peekLex.empty() ? "" : " [" + peekLex + "]") + " @ line " + to_string(yylineno));
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
Token expect(Token want, const char *msg)
{
  Token got = nextTok();
  if (got != want)
  {
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
unique_ptr<Statement> parseStatement();\
unique_ptr<Primary> parsePrimary();
unique_ptr<CompoundStmt> parseCompound();
unique_ptr<WriteStmt> parseWrite();
unique_ptr<Block> parseBlock();
unique_ptr<AssignStmt> parseAssign();
unique_ptr<ReadStmt> parseRead();
unique_ptr<Spawn> parseSpawn();
unique_ptr<Value> parseValue();
unique_ptr<Term> parseTerm();
unique_ptr<Factor> parseFactor();

unique_ptr<Spawn> parseSpawn()
{
  auto spawn = make_unique<Spawn>();
  expect(CUSTOM, "spawn statement");
  spawn->print_tree(cout, "", true);
  return spawn;
}

// TODO: statement -> assign | compound | write | read | spawn (CUSTOM)
unique_ptr<Statement> parseStatement()
{

  if (peek() == TOK_BEGIN)
  {
    return parseCompound();
  }
  else if (peek() == WRITE)
  {
    return parseWrite();
  }
  else if (peek() == IDENT)
  {
    return parseAssign();
  }
  else if (peek() == READ)
  {
    return parseRead();
  }
  else if (peek() == CUSTOM)
  {
    return parseSpawn();
  }
  else
  {
    throw runtime_error("expected start of statement");
  }

}



// TODO: primary -> FLOATLIT | INTLIT | IDENT | OPENPAREN value CLOSEPAREN
unique_ptr<Primary> parsePrimary()
{
  if (peek() == INTLIT || peek() == FLOATLIT || peek() == IDENT)
  {
    auto literal = make_unique<Literal>();
    literal->value_type = peek();
    literal->value = peekLex; 
    nextTok();
    return literal;
  }
  else if (peek() == OPENPAREN)
  {
    nextTok();
    auto expr = parseValue();
    expect(CLOSEPAREN, "closing parenthesis");
    return expr;
  }
  else
  {
    throw runtime_error("expected primary expression");
  }
}

// factor -> [ MINUS ] primary
unique_ptr<Factor> parseFactor()
{
  auto factor = make_unique<Factor>();
  factor->negative = false;

  if (peek() == MINUS)
  {
    factor->negative = true;
    nextTok();
  }

  factor->primary = parsePrimary();
  return factor;

}

// TODO factor { ( MULTIPLY | DIVIDE | MOD) factor }
unique_ptr<Term> parseTerm()
{
  auto term = make_unique<Term>();
  term->factors.push_back(parseFactor());

  while (peek() == MULTIPLY || peek() == DIVIDE || peek() == MOD)
  {
    term->ops.push_back(peek());
    nextTok();
    term->factors.push_back(parseFactor());
  }
  return term;
}

// TODO value->term { ( PLUS | MINUX) term }
unique_ptr<Value> parseValue()
{

  auto value = make_unique<Value>();
  value->terms.push_back(parseTerm());

  while (peek() == PLUS || peek() == MINUS)
  {
    value->operator_signs.push_back(peek());
    nextTok();
    value->terms.push_back(parseTerm());
  }

  return value;
}

// assign -> IDENT ( ASSIGN | CUSTOM ) value
unique_ptr<AssignStmt> parseAssign()
{

  auto assign = make_unique<AssignStmt>();

  expect(IDENT, "variable to assign");
  assign->id = peekLex;

  if (peek() == ASSIGN || peek() == PLUS_ASSIGN || peek() == MINUS_ASSIGN || peek() == MULTIPLY_ASSIGN || peek() == DIVIDE_ASSIGN || peek() == CUSTOM)
  {
    assign->type = peek();
    nextTok();
  }
  else
  {
    throw runtime_error("expected assignment operator");
  }
  assign->rhs = parseValue();
  return assign;
}

unique_ptr<ReadStmt> parseRead()
{

  auto read = make_unique<ReadStmt>();
  expect(READ, "read variable");
  expect(OPENPAREN, "open parenthases");
  expect(IDENT, "blah");
  read->target = peekLex;
  auto it = symbolTable.find(read->target);
  if (it == symbolTable.end())
    throw runtime_error("undeclared variable");
  expect(CLOSEPAREN, "close parenthases");

  return read;
}


unique_ptr<WriteStmt> parseWrite()
{

  auto write = make_unique<WriteStmt>();
  expect(WRITE, "start of write block");
  expect(OPENPAREN, "open parenthases");
  if (peek() == STRINGLIT|| peek() == IDENT)
  {
    write->type = peek();
    nextTok();
    write->content = peekLex;
  }

  expect(CLOSEPAREN, "close parenthases");

  return write;
}

unique_ptr<CompoundStmt> parseCompound()
{
  auto compound = make_unique<CompoundStmt>();

  expect(TOK_BEGIN, "begin token");

  compound->statements.push_back(parseStatement());

  
  while (peek() == SEMICOLON)
  {
    expect(SEMICOLON, "semicolon");
    compound->statements.push_back(parseStatement());
  }

  expect(END, "end of compound stmt");

  return compound;
}

unique_ptr<Block> parseBlock()
{

  auto b = make_unique<Block>();

  if (peek() == VAR)
  {
    expect(VAR, "variable declarations");
    while (peek() == IDENT)
    {
      expect(IDENT, "variable name");
      string name = peekLex;
      expect(COLON, "colon");
      Token typeTok = nextTok();
      if (typeTok != INTEGER && typeTok != REAL)
      {
        throw runtime_error("expected variable after colon");
      }
      if (symbolTable.find(name) != symbolTable.end())
      {
        throw runtime_error("variable '" + name + "' already declared");
      }
      symbolTable[name] = (typeTok == INTEGER) ? variant<int, double>(0) : variant<int, double>(0.0);
      expect(SEMICOLON, "semicolon after declaration");
    }
  }

  b->compound = parseCompound();

  return b;
}

// -----------------------------------------------------------------------------
// Program → PROGRAM IDENT ';' Block EOF
// -----------------------------------------------------------------------------
unique_ptr<Program> parseProgram()
{
  // Make a pointer to the node we need to build
  auto p = make_unique<Program>();
  // Step through the grammar, storing anything necessary as member variables
  expect(PROGRAM, "start of program");
  expect(IDENT, "program name");
  // Store the program name
  p->name = peekLex;
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
  if (peek() != TOK_EOF)
  {
    ostringstream oss;
    oss << "Parse error (line " << yylineno << "): extra tokens after <program>, got "
        << tname(peekTok) << " [" << (yytext ? yytext : "") << "]";
    throw runtime_error(oss.str());
  }

  return root;
}