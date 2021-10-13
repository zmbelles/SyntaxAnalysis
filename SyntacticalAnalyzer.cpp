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
        cout << pos << " " << tok << " " << lex << endl;
        tokens.push_back(tok);
        lexemes.push_back(lex);
        getline(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

bool SyntaxAnalyzer::parse(){
    if (vdec()){
        if (*tokitr == "t_begin"){
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

bool SyntaxAnalyzer::vdec(){

    if (*tokitr != "t_var")
        return true;
    else{
        tokitr++; lexitr++;

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
        if (*tokitr == "t_id"){
            
            //what is this doing?
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
    cout << "result " << result << endl;
    return result;
}

bool SyntaxAnalyzer::stmtlist(){
    int result = stmt();
    while (result == 1){
        result = stmt();
    }
    if (result == 0)
        return false;
    else
        return true;
}
int SyntaxAnalyzer::stmt(){
    if (*tokitr == "t_if"){
        tokitr++; lexitr++;
        if (ifstmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_while"){
        tokitr++; lexitr++;
        if (whilestmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_id"){  // assignment starts with identifier
        tokitr++; lexitr++;
        cout << "t_id" << endl;
        if (assignstmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_input"){
        tokitr++; lexitr++;
        if (inputstmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_output"){
        tokitr++; lexitr++;
        cout << "t_output" << endl;
        if (outputstmt()) return 1;
        else return 0;
    }
    return 2;  //stmtlist can be null
}

bool SyntaxAnalyzer::ifstmt(){
    
    if(*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if(expr()){
            if(*tokitr == "s_rparen"){
                tokitr ++; lexitr++;
                if(*tokitr == "t_then"){
                    tokitr++; lexitr++;
                    if(stmtlist()){
                        if(elsepart()){
                            if(*tokitr == "t_end"){
                                tokitr++; lexitr++;
                                if(*tokitr == "t_if"){
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

bool SyntaxAnalyzer::elsepart(){
    if (*tokitr == "t_else"){
        tokitr++; lexitr++;
        if (stmtlist())
            return true;
        else
            return false;
    }
    return true;   // elsepart can be null
}

bool SyntaxAnalyzer::whilestmt(){
    if(*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if(expr()){
            if(*tokitr == "s_rparen"){
                tokitr++; lexitr++;
                if(*tokitr == "t_loop"){
                    tokitr++; lexitr++;
                    if(stmtlist()){
                        if(*tokitr == "t_end"){
                            tokitr++; lexitr++;
                            if(*tokitr == "t_loop"){
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

bool SyntaxAnalyzer::assignstmt(){
    if(*tokitr == "s_assign"){
        tokitr++; lexitr++;
        if(expr()){
            return true;
        }
    }
    return false;
    // write this function
}
bool SyntaxAnalyzer::inputstmt(){
    if (*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if (*tokitr == "t_id"){
            tokitr++; lexitr++;
            if (*tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}

bool SyntaxAnalyzer::outputstmt(){
    
    if(*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if(expr()){
            if(*tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
                
            }
        }
        else if(*tokitr == "t_string"){
            tokitr++; lexitr++;
            if(*tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
    // write this function
}

bool SyntaxAnalyzer::expr(){
    if (simpleexpr()){
        if (logicop()){
            if (simpleexpr())
                return true;
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

bool SyntaxAnalyzer::simpleexpr(){
    if(term()){
        if(arithop()){
            return true;
        }
        else if(relop()){
            return true;
        }
    }
    return false;
    // write this function
}

bool SyntaxAnalyzer::term(){
    if ((*tokitr == "t_int")
    || (*tokitr == "t_str")
    || (*tokitr == "t_id")){
        tokitr++; lexitr++;
        return true;
    }
    else
        if (*tokitr == "s_lparen"){
            tokitr++; lexitr++;
            if (expr())
                if (*tokitr == "s_rparen"){
                    tokitr++; lexitr++;
                    return true;
                }
        }
    return false;
}

bool SyntaxAnalyzer::logicop(){
    if ((*tokitr == "s_and") || (*tokitr == "s_or")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

bool SyntaxAnalyzer::arithop(){
    if ((*tokitr == "s_mult") || (*tokitr == "s_plus") || (*tokitr == "s_minus")
        || (*tokitr == "s_div")    || (*tokitr == "s_mod")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

bool SyntaxAnalyzer::relop(){
    if ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge")
        || (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}


int main(){
    ifstream infile("lexemes.txt");
    SyntaxAnalyzer sa(infile);
    sa.parse();
    return 1;
}

