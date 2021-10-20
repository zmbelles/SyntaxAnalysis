#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

class SyntaxAnalyzer{
    private:
        vector<string> lexemes;
        vector<string> tokens;
        vector<string>::iterator lexitr;
        vector<string>::iterator tokitr;

        // map of variables to datatype (i.e. sum t_integer)
        // sets this up for stage 3
        map<string, string> symboltable;

        // other private methods
        bool vdec();
        int vars();
        bool stmtlist();
        int stmt();
        bool ifstmt();
        bool elsepart();
        bool whilestmt();
        bool assignstmt();
        bool inputstmt();
        bool outputstmt();
        bool expr();
        bool simpleexpr();
        bool term();
        bool logicop();
        bool arithop();
        bool relop();

    public:
        SyntaxAnalyzer(istream& infile);
        // pre: 1st parameter consists of an open file containing a source code's
        //    valid scanner output.  This data must be in the form
        //            token : lexeme
        // post: the vectors have been populated

        bool parse();
        // pre: none
        // post: The lexemes/tokens have been parsed and the symboltable created.
        // If an error occurs, a message prints indicating the token/lexeme pair
        // that caused the error.  If no error occurs, the symboltable contains all
        // variables and datatypes.
};
SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string line, tok, lex;
    int pos;
    getline(infile, line);
    bool valid = true;
    while(!infile.eof() && (valid)){
        pos = line.find(":");
        tok = line.substr(0, pos-1);
        lex = line.substr(pos+2, line.length());
        tokens.push_back(tok);
        lexemes.push_back(lex);
        getline(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

// post:
//    users source code is entirely checked for syntactical errors
bool SyntaxAnalyzer::parse(){
    
    if (vdec()){
        if (tokitr!=tokens.end() && *tokitr == "t_begin"){
            tokitr++; lexitr++;
                if (tokitr!=tokens.end() && stmtlist()){
                    if (tokitr!=tokens.end()){
                        if (*tokitr == "t_end"){
                            tokitr++; lexitr++;
                            if (tokitr==tokens.end()){
                                cout << "Valid source code file" << endl;
                                return true;
                            }
                            else{
                                cout << "end came too early" << endl;
                            }
                        }
                        else{
                            cout << "invalid statement ending code" << endl;
                        }
                    }
                    else{
                        cout << "no end" << endl;
                    }
                }
                else{
                    cout << "bad/no stmtlist or no end" << endl;
                }
            }
        else{
            cout << "no begin" << endl;
        }
    }
    else{
        cout << "bad var list" << endl;
    }
    return false;
}
//post:
//    source code is checked for variables
bool SyntaxAnalyzer::vdec(){

    if (tokitr!=tokens.end() && *tokitr != "t_var"
        && *tokitr != "t_integer" && *tokitr != "t_string")
        return true;
    else{

        int result = 0;   // 0 none found, 1 found, -1 error
        result = vars();  // need at least one
        if (result != 1)
            return false;
        while (result == 1){
            if (tokitr!=tokens.end())
                result = vars(); // parse vars
            else
                result = -1;
        }

        if (result == -1)
            return false;
        else
            return true;
    }
}
/*
 post:
 source code is checked for valid variable declarations
 */
int SyntaxAnalyzer::vars(){
    
    string temp;
        if (*tokitr == "t_integer"){
            temp = "t_integer";
            tokitr++; lexitr++;
        }
        else if (*tokitr == "t_string"){
            temp = "t_string";
            tokitr++; lexitr++;
        }
        else
            return 0;

        int result = 0;  // 0 none found, 1 found, -1 error
        while (tokitr != tokens.end() && result == 0){
            if (*tokitr == "t_id" || *tokitr == "t_str"){
                symboltable[*lexitr] = temp;
                tokitr++; lexitr++;
                if (tokitr != tokens.end() && *tokitr == "s_comma"){
                    tokitr++; lexitr++;
                }
                else if (tokitr != tokens.end() && *tokitr == "s_semi"){
                    result = 1;
                    tokitr++; lexitr++;
                }
                else
                    result = -1;
            }
            else{
                result = -1;
            }
        }
    return result;
    }

/*
 post:
    source code is checked for valid statements
 */
bool SyntaxAnalyzer::stmtlist(){
    int result = stmt();
    while (result == 1){
        result = stmt();
    }
    if (result == 0)
        return false;
    else
        return true; //can be the empty set
}
int SyntaxAnalyzer::stmt(){
    if (tokitr!=tokens.end() && *tokitr == "t_if"){
        tokitr++; lexitr++;
        if (ifstmt()) return 1;
        else return 0;
    }
    else if (tokitr!=tokens.end() && *tokitr == "t_while"){
        tokitr++; lexitr++;
        if (whilestmt()) return 1;
        else return 0;
    }
    else if (tokitr!=tokens.end() && *tokitr == "t_id"){  // assignment starts with identifier
        tokitr++; lexitr++;
        if (assignstmt()) return 1;
        else return 0;
    }
    else if (tokitr!=tokens.end() && *tokitr == "t_input"){
        tokitr++; lexitr++;
        if (inputstmt()) return 1;
        else return 0;
    }
    else if (tokitr!=tokens.end() && *tokitr == "t_output"){
        tokitr++; lexitr++;
        if (outputstmt()) return 1;
        else return 0;
    }
    return 2;  //stmtlist can be null
}

/*
 post:
    if statement of source code is checked for validity
 */
bool SyntaxAnalyzer::ifstmt(){
    
    if(tokitr!=tokens.end() && *tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if(expr()){
            if(tokitr!=tokens.end() && *tokitr == "s_rparen"){
                tokitr ++; lexitr++;
                if(tokitr!=tokens.end() && *tokitr == "t_then"){
                    tokitr++; lexitr++;
                    if(stmtlist()){
                        if(elsepart()){
                            if(tokitr!=tokens.end() && *tokitr == "t_end"){
                                tokitr++; lexitr++;
                                if(tokitr!=tokens.end() && *tokitr == "t_if"){
                                    tokitr++; lexitr++;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
        
    return false;
}
/*
 post:
    else statement of source code is checked for validity
 */
bool SyntaxAnalyzer::elsepart(){
    if (tokitr!=tokens.end() && *tokitr == "t_else"){
        tokitr++; lexitr++;
        if (stmtlist())
            return true;
        else
            return false;
    }
    return true;   // elsepart can be null
}
/*
 post:
    while statement of source code is checked for validity
 */
bool SyntaxAnalyzer::whilestmt(){
    if(tokitr!=tokens.end() && *tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if(expr()){
            if(tokitr!=tokens.end() && *tokitr == "s_rparen"){
                tokitr++; lexitr++;
                if(tokitr!=tokens.end() && *tokitr == "t_loop"){
                    tokitr++; lexitr++;
                    if(stmtlist()){
                        if(tokitr!=tokens.end() && *tokitr == "t_end"){
                            tokitr++; lexitr++;
                            if(tokitr!=tokens.end() && *tokitr == "t_loop"){
                                tokitr++; lexitr++;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
    // write this function
}
/*
 post:
    statement is checked for assignment operator
 */
bool SyntaxAnalyzer::assignstmt(){
    if(tokitr!=tokens.end() && *tokitr == "s_assign"){
        tokitr++; lexitr++;
        if(expr()){
            return true;
        }
        else if(tokitr!=tokens.end() && *tokitr == "s_semi"){
            tokitr ++; lexitr++;
            return true;
        }
    }
    return false;
    // write this function
}
/*
 post:
    input statement of source code is checked for validity
 */
bool SyntaxAnalyzer::inputstmt(){
    if (tokitr!=tokens.end() && *tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if (tokitr!=tokens.end() && *tokitr == "t_id"){
            tokitr++; lexitr++;
            if (tokitr!=tokens.end() && *tokitr == "s_rparen"){\
                tokitr++; lexitr++;\
                return true;
            }
        }
    }
    return false;
    
}

/*
 post:
    output statement of source code is checked for validity
 */
bool SyntaxAnalyzer::outputstmt(){
    
    if(tokitr!=tokens.end() && *tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if(expr()){
            if(tokitr!=tokens.end() && *tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
        else if(tokitr!=tokens.end() && *tokitr == "t_string"){
            tokitr++; lexitr++;
        }
        else if(tokitr!=tokens.end() && *tokitr == "t_id"){
            tokitr++; lexitr++;
        }
        if(tokitr!=tokens.end() && *tokitr == "s_rparen"){
            tokitr++; lexitr++;
            return true;
        }
    }
    return false;
    // write this function
}

/*
 post:
    source code is checked for valid expression
 */
bool SyntaxAnalyzer::expr(){
    if (simpleexpr()){
        if (logicop()){
            if (simpleexpr()){
                return true;
            }
            else
                return false;
        }
        else
            return true;
    }
    else{
        return false;
    }
}
/*
 post:
    the validity of a simple expression is returned
 */
bool SyntaxAnalyzer::simpleexpr(){
    if(term()){
        if(arithop()){
            if(term())
            return true;
        }
        else if(relop()){
            if(term())
                return true;
        }
    }
    return false;
}

/*
 the validity of a term is returned
 */
bool SyntaxAnalyzer::term(){
    if ((tokitr!=tokens.end() && *tokitr == "t_int")
    ||  (tokitr!=tokens.end() && *tokitr == "t_str")
    ||  (tokitr!=tokens.end() && *tokitr == "t_id")){
        tokitr++; lexitr++;
        return true;
    }
    else
        if (tokitr!=tokens.end() && *tokitr == "s_lparen"){
            tokitr++; lexitr++;
            if (expr())
                if (tokitr!=tokens.end() && *tokitr == "s_rparen"){
                    tokitr++; lexitr++;
                    return true;
                }
        }
    return false;
}

/*
 returned if the dereferenced token iterator is pointing to a logical operator
 */
bool SyntaxAnalyzer::logicop(){
    if    ((tokitr!=tokens.end() && *tokitr == "s_and")
        || (tokitr!=tokens.end() && *tokitr == "s_or")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}
/*
 returned if the dereferenced token iterator is pointing to a arithmatic operator
 */
bool SyntaxAnalyzer::arithop(){
    if    ((tokitr!=tokens.end() && *tokitr == "s_mult")
        || (tokitr!=tokens.end() && *tokitr == "s_plus")
        || (tokitr!=tokens.end() && *tokitr == "s_minus")
        || (tokitr!=tokens.end() && *tokitr == "s_div")
        || (tokitr!=tokens.end() && *tokitr == "s_mod")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}
/*
 returns true if the dereferenced token iterator is pointing to a relational operator
 */
bool SyntaxAnalyzer::relop(){
    if    ((tokitr!=tokens.end() && *tokitr == "s_lt")
        || (tokitr!=tokens.end() && *tokitr == "s_gt")
        || (tokitr!=tokens.end() && *tokitr == "s_ge")
        || (tokitr!=tokens.end() && *tokitr == "s_eq")
        || (tokitr!=tokens.end() && *tokitr == "s_ne")
        || (tokitr!=tokens.end() && *tokitr == "s_le")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}


int main(){
    ifstream infile("OutputFile.txt");
    SyntaxAnalyzer sa(infile);
    sa.parse();
    return 1;
}
