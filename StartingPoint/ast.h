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
#include <cmath>
#include <climits>
#include "lexer.h"
using namespace std;

inline map<string, variant<int,double>> symbolTable;

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


inline variant<int, double> handleMath(variant<int, double> lhs, Token op, variant<int, double> rhs) {
  

  if (holds_alternative<double>(lhs) || holds_alternative<double>(rhs)) {
    double lhs_variable = visit([](auto&& arg) -> double { return static_cast<double>(arg); }, lhs);
    double rhs_variable = visit([](auto&& arg) -> double { return static_cast<double>(arg); }, rhs);
    if (op == PLUS) {
      return lhs_variable + rhs_variable;
    } else if (op == MINUS) {
      return lhs_variable - rhs_variable;
    } else if (op == MULTIPLY) {
      return lhs_variable * rhs_variable;
    } else if (op == DIVIDE) {
      return lhs_variable / rhs_variable;
    } else if (op == MOD) {
      return fmod(lhs_variable, rhs_variable);
    }
    else {
      throw runtime_error("unknown operator");
    }

  }
  else {
    int left_variable = visit([](auto&& arg) -> int { return static_cast<int>(arg); }, lhs);
    int right_variable = visit([](auto&& arg) -> int { return static_cast<int>(arg); }, rhs);
    
    long long result;
    
    if (op == PLUS) {
      result = (long long)left_variable + right_variable;
    } else if (op == MINUS) {
      result = (long long)left_variable - right_variable;
    } else if (op == MULTIPLY) {
      result = (long long)left_variable * right_variable;
    } else if (op == DIVIDE) {
      result = (long long)left_variable / right_variable;
    } else if (op == MOD) {
      result = left_variable % right_variable;
    }
    else {
      throw runtime_error("unknown operator");
    }
    if (result > INT_MAX || result < INT_MIN) {
      cerr << "Warning: Integer overflow occurred. Result may be inaccurate." << endl;
    }
    return static_cast<int>(result);
  }  
}

struct Program;
struct Block;
struct CompoundStmt;
struct WriteStmt;
struct AssignStmt;
struct ReadStmt;
struct Value;
struct Term;
struct Factor;
struct Primary;
struct Literal;

struct Statement{
  
  vector<unique_ptr<Statement>> statements;

  virtual void print_tree(ostream&,string, bool) {}
  virtual void interpret(ostream&) {}
};


struct Primary{

  virtual void print_tree(ostream&,string, bool) {}
  virtual variant<int, double> interpret(ostream&) {
    return 0;
  }

};


// TODO: custom statement for spawn
// Calculated program score based on number and types of variables in symbol table, 
// then prints a motivational message based on the score
struct Spawn : public Statement{


  void print_tree(ostream& out, string prefix, bool last){
    ast_line(out, prefix, last, "Spawn Statement");
    string child_prefix = prefix + (last ? "    ": "|   ");
    out << child_prefix <<  "Program variables:\n";
    for (const auto& [name, value] : symbolTable) {
      out << child_prefix << "  " << name;
      out << "\n";
    } 
  }


  void interpret(ostream& out) {
    (void)out;
    
    double ident_weight = 10;
    double double_weight = 5;
    double int_weight = 1;
    double score = 0;

    for (const auto& [name, value] : symbolTable) {
      visit([&score, &ident_weight, &double_weight, &int_weight](auto&& arg) { 
        using T = decay_t<decltype(arg)>;
        if constexpr (is_same_v<T, int>) {
          score += arg * int_weight;
        } else if constexpr (is_same_v<T, double>) {
          score += arg * double_weight;
        } else {
          score += ident_weight;
        }
      }, value);
    }
    out << "Grading your program based on your variables...\n";
    out << "Program Score: " << score << "\n";

    if (score < 5){
      out << "Poor...Do better next time\n";
    }
    else if (score < 10){
      out << "Average...but you're getting there\n";
    }
    else {
      out << "You'd get a whole O on your O.W.L exams. \n";
    }
  }
  };

struct Literal : public Primary{
  string id;
  string value;
  Token value_type;

  void print_tree(ostream& out, string prefix, bool last){
    if (value_type == INTLIT) {
      ast_line(out, prefix, last, "Integer Literal: " + id);
    } else if (value_type == FLOATLIT) {
      ast_line(out, prefix, last, "Float Literal: " + id);
    } else {
      ast_line(out, prefix, last, "Identifier: " + id);
    }
  }

  variant<int, double> interpret(ostream& out) override {
    (void)out;

    if (value_type == INTLIT) {
      return stoi(value);
    } 
    else if (value_type == FLOATLIT) {
      return stod(value);
    } 
    else {
      auto it = symbolTable.find(value);
      if (it == symbolTable.end()) {
        throw runtime_error("undefined variable: " + value);
      }
      return it->second;
    }
  }
};

struct Factor : public Primary{

  bool negative = false;
  unique_ptr<Primary> primary;

  void print_tree(ostream& out, string prefix, bool last)override{
    ast_line(out, prefix, last, "Factor: " + string(negative ? "Negative" : "Positive"));
    if (primary){
      string child_prefix = prefix + (last ? "    ": "|   ");
      primary->print_tree(out, child_prefix, true);
    }
  }

  variant<int, double> interpret(ostream& out) {
    (void)out;
    auto val = primary->interpret(out);
    if (negative) {
      return visit([](auto&& arg) -> variant<int, double> { return -arg; }, val);
    }
    return val;
  }
};

struct Term : public Primary{
  vector<Token> ops;
  vector<unique_ptr<Factor>> factors;

  void print_tree(ostream& out, string prefix, bool last){
    ast_line(out, prefix, last, "Term: ");
    string child_prefix = prefix + (last ? "    ": "|   ");
    for (size_t i = 0; i < factors.size(); ++i) {
      if (i > 0) {
        string opStr = (ops[i-1] == MULTIPLY) ? "*" :
                       (ops[i-1] == DIVIDE)   ? "/" : "%";
        ast_line(out, child_prefix, false, "Op: " + opStr);
      }
      factors[i]->print_tree(out, child_prefix, last);
    }
  }

  variant<int, double> interpret(ostream& out) {
    (void)out;
    auto result = factors[0]->interpret(out);
    for (size_t i = 1; i < factors.size(); ++i) {
      result = handleMath(result, ops[i-1], factors[i]->interpret(out));
    }
    return result;

  }

};


struct Value : public Primary{
  vector<Token> operator_signs;
  vector<unique_ptr<Term>> terms;

  void print_tree(ostream& out, string prefix, bool last){
    ast_line(out, prefix, last, "Value: ");
    string child_prefix = prefix + (last ? "    ": "|   ");
    for (size_t i = 0; i < terms.size(); ++i) {
      if (i > 0) {
        string opStr = (operator_signs[i-1] == PLUS) ? "+" : "-";
        ast_line(out, child_prefix, false, "Op: " + opStr);
      }
      terms[i]->print_tree(out, child_prefix, last);
    }
  }

  variant<int, double> interpret(ostream& out) {
    auto result = terms[0]->interpret(out);
    for (size_t i = 1; i < terms.size(); ++i) {
      result = handleMath(result, operator_signs[i-1], terms[i]->interpret(out));
    }
    return result;
  }
};


// assign → IDENT ( ASSIGN | CUSTOM_OPERATORS ) value
struct AssignStmt : public Statement{

  string id;
  Token type;
  unique_ptr<Value> rhs;

  void print_tree(ostream& out, string prefix, bool last) override{
    string assignment_type;

    if (type == ASSIGN) 
      assignment_type = ":=";
    else if(type==PLUS_ASSIGN)
      assignment_type = "+=";
    else if(type==MINUS_ASSIGN)
      assignment_type = "-=";
    else if(type==MULTIPLY_ASSIGN)
      assignment_type = "*=";
    else if(type== DIVIDE_ASSIGN)
      assignment_type = "/=";
    else if(type==CUSTOM)
      assignment_type = "spawn";

    ast_line(out, prefix, last, "Assign Statement");
    string child_prefix = prefix + (last ? "    ": "|   ");
    if (rhs) {
      rhs->print_tree(out, child_prefix, true);
    }
  }

  // TODO
  void interpret(ostream& out) override {
    (void)out;
    auto it = symbolTable.find(id);
    if (it == symbolTable.end()) {
      throw runtime_error("undefined variable: " + id);
    }
    auto rhs_value = rhs->interpret(out);
    if (holds_alternative<int>(it->second)) {
      if (holds_alternative<int>(rhs_value))
        it->second = get<int>(rhs_value);
      else
        it->second = static_cast<int>(get<double>(rhs_value));
    }
    else if (holds_alternative<double>(it->second)) {
      if (holds_alternative<int>(rhs_value))
        it->second = static_cast<double>(get<int>(rhs_value));
      else
        it->second = get<double>(rhs_value);
    }

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
    if (it == symbolTable.end()) {throw runtime_error("undefined variable: " + target);}

    string input;
    cin >> input;
    
    visit([&](auto& slot) {
      using T = decay_t<decltype(slot)>;

      try {
        if constexpr (is_same_v<T, int>) {
          double temp = stod(input);  

          if (temp > INT_MAX || temp < INT_MIN) {
            cerr << "[Warning] value out of int range, truncating\n";
          }

          if (floor(temp) != temp) {
            cerr << "[Warning] narrowing from REAL to INTEGER\n";
          }

          slot = static_cast<int>(temp);
        }
        else if constexpr (is_same_v<T, double>) {
          slot = stod(input);
        }
      }
      catch (...) {
        throw runtime_error("invalid input for variable: " + target);
      }

    }, it->second);

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

    ast_line(out, prefix, last, "Compound Statement");
    string child_prefix = prefix + (last ? "    ": "|   ");
    for (size_t i = 0; i < statements.size(); i++) {
      statements[i]->print_tree(out, child_prefix, i == statements.size() - 1);
    }
  }

  void interpret(ostream& out){
    for (const auto& stmt: statements){
      stmt->interpret(out);
    }
  }

};


struct Block
{
  unique_ptr<CompoundStmt> compound;
  
  void print_tree(ostream& out,string prefix,bool last){
    cout << "Block\n";
    if (compound) compound->print_tree(out, prefix, last);
  }

  void interpret(ostream& out){
    if (compound) compound->interpret(out); 
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
