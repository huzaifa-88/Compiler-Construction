#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>

using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_STRING, T_CHAR, T_BOOL,
    T_ID, T_NUM, T_IF, T_ELSE, T_RETURN, 
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, 
    T_SEMICOLON, T_GT, T_LT, T_EOF
};

struct Token{
    TokenType type;
    string value;
    int line;
};

class Lexer{

    private:
        string src;
        size_t pos;
        int line;

    public:
        Lexer(const string &src){
            this->src = src;
            this->pos = 0;
            this->line = 1;
        }

        string consumeNumber(){
            size_t start = pos;
            while (pos < src.size() && isdigit(src[pos])) pos++;
            return src.substr(start, pos - start);
        }

        string consumeWord(){
            size_t start = pos;
            while (pos < src.size() && isalnum(src[pos])) pos++;
            return src.substr(start, pos - start);
        }

        vector<Token> tokenize(){
            vector<Token> tokens;
            while(pos < src.size()){
                char current = src[pos];\
                if(current == '\n') 
                {
                    line++;
                    pos++;
                    continue;
                };

                if(isspace(current)){
                    pos++;
                    continue;
                }
                if(isdigit(current)){
                    tokens.push_back(Token{T_NUM, consumeNumber(), line});
                    continue;
                }
                if(isalpha(current)){
                    string word = consumeWord();
                    if (word == "int") tokens.push_back(Token{T_INT, word, line});
                    else if (word == "float") tokens.push_back(Token{T_FLOAT, word, line});
                    else if (word == "double") tokens.push_back(Token{T_DOUBLE, word, line});
                    else if (word == "string") tokens.push_back(Token{T_STRING, word, line});
                    else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line}); 
                    else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});
                    else if(word == "if") tokens.push_back(Token{T_IF, word, line});
                    else if(word == "else") tokens.push_back(Token{T_ELSE, word, line});
                    else if(word == "return") tokens.push_back(Token{T_RETURN, word, line});
                    else tokens.push_back(Token{T_ID, word, line});
                    continue;
                }
                switch(current){
                    case '=': tokens.push_back(Token{T_ASSIGN, "=", line}); break;
                    case '+': tokens.push_back(Token{T_PLUS, "+", line}); break;
                    case '-': tokens.push_back(Token{T_MINUS, "-", line}); break;
                    case '*': tokens.push_back(Token{T_MUL, "*", line}); break;
                    case '/': tokens.push_back(Token{T_DIV, "/", line}); break;
                    case '(': tokens.push_back(Token{T_LPAREN, "(", line}); break;
                    case ')': tokens.push_back(Token{T_RPAREN, ")", line}); break;
                    case '{': tokens.push_back(Token{T_LBRACE, "{", line}); break;
                    case '}': tokens.push_back(Token{T_RBRACE, "}", line}); break;
                    case ';': tokens.push_back(Token{T_SEMICOLON, ";", line}); break;
                    case '>': tokens.push_back(Token{T_GT, ">", line}); break;
                    case '<': tokens.push_back(Token{T_LT, "<", line}); break;
                    default: 
                        cout << "Unexpected character: " << current <<" at line " << line << endl; 
                        exit(1);
                }
                pos++;
            }
            tokens.push_back(Token{T_EOF, "", line}); // END of FILE token
            return tokens;
        }
};  // End of class Lexer

class Parser{
    
public:
    Parser(const vector<Token> &tokens){
        this->tokens = tokens;
        this->pos = 0;
    }
    void parseProgram(){
        while (tokens[pos].type != T_EOF){
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax error" << endl;
    }
private:
    vector<Token> tokens;
    size_t pos;


    void parseStatement(){
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE || tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR) {
            parseDeclaration();
        }
        else if(tokens[pos].type == T_ID){
            parseAssignment();
        }
        else if(tokens[pos].type == T_IF){
            parseIfStatement();
        }
        else if(tokens[pos].type == T_RETURN){
            parseReturnStatement();
        }
        else if(tokens[pos].type == T_LBRACE){
            parseBlock();
        }
        else{
            cout << "Syntax Error: Unexpected token " << tokens[pos].value << "at line" << tokens[pos].line << endl;
            exit(1);
        }
    }   //parseStatement Ending

    void parseBlock(){
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF){
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration() {
        expect(T_INT);
        expect(T_ID);
        expect(T_SEMICOLON);
    }

    void parseAssignment(){
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement(){
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
        if(tokens[pos].type == T_ELSE){
            expect(T_ELSE);
            parseStatement();
        }
    }

    void parseReturnStatement(){
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression(){
        parseTerm();
        while(tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS){
            pos++;
            parseTerm();
        }
        if(tokens[pos].type == T_GT){
            pos++;
            parseExpression();
        }
    }

    void parseTerm(){
        parseFactor();
        while(tokens[pos].type == T_MUL || tokens[pos].type == T_DIV){
            pos++;
            parseFactor();
        }
    }

    void parseFactor(){
        if(tokens[pos].type == T_NUM || tokens[pos].type == T_ID){
            pos++;
        }
        else if(tokens[pos].type == T_LPAREN){
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        }
        else{
            cout << "Syntax error: unexpected token " << tokens[pos].value << " at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void expect(TokenType type){
        if(tokens[pos].type == type){
            pos++;
        }
        else{
            cout << tokens[pos].value << " "<<tokens[pos].type<<endl;
            cout << "Syntax Error: Expected "<< type << " but found " << tokens[pos].value << " at line " << tokens[pos].line << endl;
            exit(1);
        }
    }
};  // End of class Parser


int main(){
    string input = R"(
        int a;
        a = 5
        int b;
        b = a + 10;
        if(b > 10){
            return b;
        }
        else{
            return 0;
        }
    )";
    // string path = "code.txt";
    // ifstream file(path);
    // if (!file.is_open()) {
    //     std::cerr << "Could not open the file!" << std::endl;
    //     return 1;
    // }
    
    // string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    // for(int pos = 0; pos < tokens.size(); pos++){
    //     cout << tokens[pos].type << ":" << tokens[pos].value << ":" << tokens[pos].line << endl;
    // }

    Parser parser(tokens);
    parser.parseProgram();
    
    return 0;
}