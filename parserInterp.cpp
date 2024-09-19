/* Implementation of Recursive-Descent Parser
 * for the SFort95 Language
 * parser(SP24).cpp
 * Programming Assignment 2
 * Spring 2024
 */

//
//
//
// --------- THIS IS JUST THE ORIGINAL FILE FOR COMPARISON TO NEW PARSER -------------- //
//
//
//
//
#include "parserInterp.h"

map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; // Container of temporary locations of Value objects for results of expressions, variables values and constants
queue<Value> *ValQue;            // declare a pointer variable to a queue of Value objects

namespace Parser
{
    bool pushed_back = false;
    LexItem pushed_token;

    static LexItem GetNextToken(istream &in, int &line)
    {
        if (pushed_back)
        {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem &t)
    {
        if (pushed_back)
        {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool IdentList(istream &in, int &line);

// Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream &in, int &line)
{
    bool dl = false, sl = false;
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok.GetToken() == PROGRAM)
    {
        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == IDENT)
        {
            dl = Decl(in, line);
            if (!dl)
            {
                ParseError(line, "Incorrect Declaration in Program");
                return false;
            }
            sl = Stmt(in, line);
            if (!sl)
            {
                ParseError(line, "Incorrect Statement in program");
                return false;
            }
            tok = Parser::GetNextToken(in, line);

            if (tok.GetToken() == END)
            {
                tok = Parser::GetNextToken(in, line);

                if (tok.GetToken() == PROGRAM)
                {
                    tok = Parser::GetNextToken(in, line);

                    if (tok.GetToken() == IDENT)
                    {
                        cout << "(DONE)" << endl;
                        return true;
                    }
                    else
                    {
                        ParseError(line, "Missing Program Name");
                        return false;
                    }
                }
                else
                {
                    ParseError(line, "Missing PROGRAM at the End");
                    return false;
                }
            }
            else
            {
                ParseError(line, "Missing END of Program");
                return false;
            }
        }
        else
        {
            ParseError(line, "Missing Program name");
            return false;
        }
    }
    else if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }

    else
    {
        ParseError(line, "Missing Program keyword");
        return false;
    }
}

// Decl ::= Type :: VarList
// Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
bool Decl(istream &in, int &line)
{
    bool status = false;
    LexItem tok;
    string strLen;

    LexItem t = Parser::GetNextToken(in, line);

    if (t == INTEGER || t == REAL || t == CHARACTER)
    {
        tok = t;

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == DCOLON)
        {
            status = VarList(in, line, tok, 1);

            if (status)
            {
                status = Decl(in, line);
                if (!status)
                {
                    ParseError(line, "Declaration Syntactic Error.");
                    return false;
                }
                return status;
            }
            else
            {
                ParseError(line, "Missing Variables List.");
                return false;
            }
        }
        else if (t == CHARACTER && tok.GetToken() == LPAREN)
        {
            tok = Parser::GetNextToken(in, line);

            if (tok.GetToken() == LEN)
            {
                tok = Parser::GetNextToken(in, line);

                if (tok.GetToken() == ASSOP)
                {
                    tok = Parser::GetNextToken(in, line);

                    if (tok.GetToken() == ICONST)
                    {
                        strLen = tok.GetLexeme();

                        tok = Parser::GetNextToken(in, line);
                        if (tok.GetToken() == RPAREN)
                        {
                            tok = Parser::GetNextToken(in, line);
                            if (tok.GetToken() == DCOLON)
                            {
                                status = VarList(in, line, tok, 1);

                                if (status)
                                {
                                    cout << "Definition of Strings with length of " << strLen << " in declaration statement." << endl;
                                    status = Decl(in, line);
                                    if (!status)
                                    {
                                        ParseError(line, "Declaration Syntactic Error.");
                                        return false;
                                    }
                                    return status;
                                }
                                else
                                {
                                    ParseError(line, "Missing Variables List.");
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            ParseError(line, "Missing Right Parenthesis for String Length definition.");
                            return false;
                        }
                    }
                    else
                    {
                        ParseError(line, "Incorrect Initialization of a String Length");
                        return false;
                    }
                }
            }
        }
        else
        {
            ParseError(line, "Missing Double Colons");
            return false;
        }
    }

    Parser::PushBackToken(t);
    return true;
} // End of Decl

// Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream &in, int &line)
{
    bool status;

    LexItem t = Parser::GetNextToken(in, line);

    switch (t.GetToken())
    {

    case PRINT:
        status = PrintStmt(in, line);

        if (status)
            status = Stmt(in, line);
        break;

    case IF:
        status = BlockIfStmt(in, line);
        if (status)
            status = Stmt(in, line);
        break;

    case IDENT:
        Parser::PushBackToken(t);
        status = AssignStmt(in, line);
        if (status)
            status = Stmt(in, line);
        break;

    default:
        Parser::PushBackToken(t);
        return true;
    }

    return status;
} // End of Stmt

bool SimpleStmt(istream &in, int &line)
{
    bool status;

    LexItem t = Parser::GetNextToken(in, line);

    switch (t.GetToken())
    {

    case PRINT:
        status = PrintStmt(in, line);

        if (!status)
        {
            ParseError(line, "Incorrect Print Statement");
            return false;
        }
        cout << "Print statement in a Simple If statement." << endl;
        break;

    case IDENT:
        Parser::PushBackToken(t);
        status = AssignStmt(in, line);
        if (!status)
        {
            ParseError(line, "Incorrect Assignent Statement");
            return false;
        }
        cout << "Assignment statement in a Simple If statement." << endl;

        break;

    default:
        Parser::PushBackToken(t);
        return true;
    }

    return status;
} // End of SimpleStmt

// Assuming LexItem has a method to get Token type or you can directly access a token member
// You need to replace getTokenType or the appropriate method/property based on your implementation
bool VarList(istream &in, int &line, LexItem &idtok, int strlen)
{
    bool status = false, exprstatus = false;
    string identstr;
    Value val;

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == IDENT)
    {
        identstr = tok.GetLexeme();
        if (defVar.find(identstr) == defVar.end() || !defVar[identstr])
        {
            defVar[identstr] = true;
            SymTable[identstr] = tok.GetToken();
        }
        else
        {
            ParseError(line, "Variable Redefinition");
            return false;
        }
    }
    else
    {
        ParseError(line, "Missing Variable Name");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok == ASSOP)
    {
        exprstatus = Expr(in, line, val);
        if (!exprstatus)
        {
            ParseError(line, "Incorrect initialization for a variable.");
            return false;
        }

        Token varType = SymTable[identstr];
        if ((varType == SCONST && !val.IsString()) ||
            (varType == RCONST && !val.IsReal()))
        {
            ParseError(line, "Illegal mixed-mode assignment operation");
            return false;
        }

        if (SymTable[identstr] == RCONST && val.IsInt())
        {
            double realValue = static_cast<double>(val.GetInt());
            val.SetReal(realValue);
            val.SetType(VREAL);
        }

        if (val.IsString())
        {
            if (strlen == 1 || val.GetstrLen() == 1)
            {
                string valAsString = val.GetString();
                val.SetString(valAsString.substr(0, 1));
            }
        }

        TempsResults[identstr] = val;

        tok = Parser::GetNextToken(in, line);
        if (tok == COMMA)
        {
            status = VarList(in, line, tok, strlen);
        }
        else
        {
            Parser::PushBackToken(tok);
            return true;
        }
    }
    else if (tok == COMMA)
    {
        status = VarList(in, line, tok, strlen);
    }
    else if (tok == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        return false;
    }
    else
    {
        Parser::PushBackToken(tok);
        return true;
    }

    return status;
}

// PrintStmt:= PRINT *, ExpreList
bool PrintStmt(istream &in, int &line)
{
    LexItem t;
    ValQue = new queue<Value>;

    t = Parser::GetNextToken(in, line);

    if (t != DEF)
    {

        ParseError(line, "Print statement syntax error.");
        return false;
    }
    t = Parser::GetNextToken(in, line);

    if (t != COMMA)
    {

        ParseError(line, "Missing Comma.");
        return false;
    }
    bool ex = ExprList(in, line);

    if (!ex)
    {
        ParseError(line, "Missing expression after Print Statement");
        return false;
    }

    while (!(*ValQue).empty())
    {
        Value nextVal = (*ValQue).front();
        cout << nextVal;
        ValQue->pop();
    }
    cout << endl;
    return ex;

} // End of PrintStmt

// BlockIfStmt:= if (Expr) then {Stmt} [Else Stmt]
// SimpleIfStatement := if (Expr) Stmt
bool BlockIfStmt(istream &in, int &line)
{
    bool ex = false, status;
    static int nestlevel = 0;
    int level;
    LexItem t;
    Value val;

    t = Parser::GetNextToken(in, line);
    if (t != LPAREN)
    {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    ex = RelExpr(in, line, val);
    if (!ex)
    {
        ParseError(line, "Missing if statement condition");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN)
    {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != THEN)
    {
        Parser::PushBackToken(t);

        status = SimpleStmt(in, line);
        if (status)
        {
            return true;
        }
        else
        {
            ParseError(line, "If-Stmt Syntax Error");
            return false;
        }
    }
    nestlevel++;
    level = nestlevel;
    status = Stmt(in, line);
    if (!status)
    {
        ParseError(line, "Missing Statement for If-Stmt Then-Part");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t == ELSE)
    {
        status = Stmt(in, line);
        if (!status)
        {
            ParseError(line, "Missing Statement for If-Stmt Else-Part");
            return false;
        }
        else
            t = Parser::GetNextToken(in, line);
    }

    if (t != END)
    {

        ParseError(line, "Missing END of IF");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t == IF)
    {
        cout << "End of Block If statement at nesting level " << level << endl;
        return true;
    }

    Parser::PushBackToken(t);
    ParseError(line, "Missing IF at End of IF statement");
    return false;
} // End of IfStmt function

// Var:= ident
bool Var(istream &in, int &line, LexItem &idtok)
{
    string identstr;

    idtok = Parser::GetNextToken(in, line);

    if (idtok == IDENT)
    {
        identstr = idtok.GetLexeme();

        if (!(defVar.find(identstr)->second))
        {
            ParseError(line, "Undeclared Variable");
            return false;
        }
        return true;
    }
    else if (idtok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << idtok.GetLexeme() << ")" << endl;
        return false;
    }
    return false;
} // End of Var

// AssignStmt:= Var = Expr
bool AssignStmt(istream &in, int &line)
{

    bool varstatus = false, status = false;
    LexItem t;

    varstatus = Var(in, line, t);
    string currentVar = t.GetLexeme();

    if (varstatus)
    {
        t = Parser::GetNextToken(in, line);

        if (t == ASSOP)
        {
            Value val;
            status = Expr(in, line, val);
            if (!status)
            {
                ParseError(line, "Missing Expression in Assignment Statment");
                return status;
            }
            if ((SymTable[currentVar] == INTEGER) && (val.GetType() == VREAL))
            {
                double currentReal = val.GetReal();
                val.SetType(VINT);
                val.SetInt(int(currentReal));
                TempsResults[currentVar] = val;
            }
            else
            {
                TempsResults[currentVar] = val;
            }
        }
        else if (t.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        }
        else
        {
            ParseError(line, "Missing Assignment Operator =");
            return false;
        }
    }
    else
    {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    return status;
}

// ExprList:= Expr {,Expr}
bool ExprList(istream &in, int &line)
{
    bool status = false;
    Value retVal;

    status = Expr(in, line, retVal);
    if (!status)
    {
        ParseError(line, "Missing Expression");
        return false;
    }
    ValQue->push(retVal);
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA)
    {

        status = ExprList(in, line);
    }
    else if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else
    {
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
} // End of ExprList

// RelExpr ::= Expr  [ ( == | < | > ) Expr ]
bool RelExpr(istream &in, int &line, Value &retVal)
{
    Value val1, val2;
    bool t1 = Expr(in, line, val1);
    LexItem tok;

    if (!t1)
    {
        return false;
    }
    retVal = val1;

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    if (tok == EQ || tok == LTHAN || tok == GTHAN)
    {
        t1 = Expr(in, line, val2);
        if (!t1)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        if (tok == EQ)
        {
            retVal.operator==(val2);
        }
        else if (tok == LTHAN)
        {
            retVal.operator<(val2);
        }
        else if (tok == GTHAN)
        {
            retVal.operator>(val2);
        }
    }

    return true;
} // End of RelExpr

// Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream &in, int &line, Value &retVal)
{
    Value val1, val2;
    bool t1 = MultExpr(in, line, val1);
    LexItem tok;

    if (!t1)
    {
        return false;
    }

    retVal = val1;

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == PLUS || tok == MINUS || tok == CAT)
    {
        t1 = MultExpr(in, line, val2);
        if (!t1)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }
        if (retVal.GetType() == VSTRING || val2.GetType() == VSTRING)
        {
            ParseError(line - 2, "Invalid types for addition");
            return false;
        }
        else
        {
            if (tok == PLUS)
            {
                retVal = retVal + val2;
            }
            else if (tok == MINUS)
            {
                retVal = retVal - val2;
            }
            else if (tok == CAT)
            {
                retVal.Catenate(val2);
            }
        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    return true;
}

// MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream &in, int &line, Value &retVal)
{
    Value val1, val2;

    bool t1 = TermExpr(in, line, val1);
    LexItem tok;

    if (!t1)
    {
        return false;
    }

    retVal = val1;

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == MULT || tok == DIV)
    {
        t1 = TermExpr(in, line, val2);

        if (!t1)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }
        if (retVal.GetType() == VSTRING || val2.GetType() == VSTRING)
        {
            ParseError(line, "Run time error, incompatible type");
            return false;
        }
        else
        {
            if (tok == MULT)
            {
                retVal = retVal * val2;
            }
            else if (tok == DIV)
            {
                if (val2.IsReal())
                {
                    if (val2.GetReal() == 0)
                    {
                        ParseError(line - 1, "Division by 0");
                        return false;
                    }
                }
                else if (val2.IsInt())
                {
                    if (val2.GetInt() == 0)
                    {
                        ParseError(line - 1, "Division by 0");
                        return false;
                    }
                }
                else
                {
                    retVal = retVal / val2;
                }
            }
        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    return true;
} // End of MultExpr

// TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream &in, int &line, Value &retVal)
{
    Value val1, val2;

    bool t1 = SFactor(in, line, val1);
    LexItem tok;

    if (!t1)
    {
        return false;
    }
    retVal = val1;

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == POW)
    {
        t1 = SFactor(in, line, val2);

        if (!t1)
        {
            ParseError(line, "Missing exponent operand");
            return false;
        }
        if (retVal.GetType() == VSTRING || val2.GetType() == VSTRING)
        {
            ParseError(line, "Run time error");
            return false;
        }
        else
        {
            if (val2.IsReal())
            {
                if (val2.GetReal() == 0)
                {
                    if (retVal.IsReal())
                    {
                        retVal.SetReal(1);
                    }
                    else if (retVal.IsInt())
                    {
                        retVal.SetInt(1);
                    }
                }
                else
                {
                    retVal.Power(val2);
                }
            }
        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    return true;
} // End of TermExpr

// SFactor = Sign Factor | Factor
bool SFactor(istream &in, int &line, Value &retVal)
{
    LexItem t = Parser::GetNextToken(in, line);

    bool status;
    int sign = 1;
    if (t == MINUS)
    {
        sign = -1;
    }
    else if (t == PLUS)
    {
        sign = 1;
    }
    else
        Parser::PushBackToken(t);

    status = Factor(in, line, sign, retVal);
    return status;
} // End of SFactor

// Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream &in, int &line, int sign, Value &retVal)
{

    LexItem tok = Parser::GetNextToken(in, line);

    // cout << tok.GetLexeme() << endl;
    if (tok == IDENT)
    {
        string lexeme = tok.GetLexeme();
        if (!(defVar.find(lexeme)->second))
        {
            ParseError(line, "Using Undefined Variable");
            return false;
        }

        if (TempsResults.find(lexeme) == TempsResults.end())
        {
            ParseError(line, "Variable does not exist");
            return false;
        }
        retVal = TempsResults[lexeme];
        if (sign == -1)
        {
            if (retVal.IsReal())
            {
                retVal.SetReal(-retVal.GetReal());
                SymTable[lexeme] = RCONST;
            }
            else if (retVal.IsInt())
            {
                retVal.SetInt(-retVal.GetInt());
                SymTable[lexeme] = ICONST;
            }
        }
        return true;
    }
    else if (tok == ICONST)
    {
        retVal.SetType(VINT);
        if (sign == -1)
        {
            retVal.SetInt(-(stoi(tok.GetLexeme())));
        }
        else if (sign == 1)
        {
            retVal.SetInt((stoi(tok.GetLexeme())));
        }
        return true;
    }
    else if (tok == SCONST)
    {
        retVal.SetType(VSTRING);
        retVal.SetString(tok.GetLexeme());
        return true;
    }
    else if (tok == RCONST)
    {
        retVal.SetType(VREAL);
        if (sign == -1)
        {
            retVal.SetReal(-(stod(tok.GetLexeme())));
        }
        else if (sign == 1)
        {
            retVal.SetReal(stod(tok.GetLexeme()));
        }
        return true;
    }
    else if (tok == LPAREN)
    {
        bool ex = Expr(in, line, retVal);
        if (!ex)
        {
            ParseError(line, "Missing expression after (");
            return false;
        }
        if (Parser::GetNextToken(in, line) == RPAREN)
            return ex;
        else
        {
            Parser::PushBackToken(tok);
            ParseError(line, "Missing ) after expression");
            return false;
        }
    }
    else if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }

    return false;
}; // THIS GETS CHANGED