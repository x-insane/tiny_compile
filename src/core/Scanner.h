#ifndef TINY_COMPILE_TOKEN_SCAN_H
#define TINY_COMPILE_TOKEN_SCAN_H

#include <fstream>
#include <vector>
#include "Token.h"

class Scanner {
    std::ifstream& in;
    std::vector<Token> list;
private:
    // temporary variables
    std::string word;
    Token::Type type = Token::Type::NONE;
    int line_number = 1;
    int char_number = -1;
    bool isSign = false;
private:
    inline explicit Scanner(std::ifstream& fin) : in(fin) {
        analyse();
    }
public:
    inline std::vector<Token> getTokens() {
        return list;
    }
public:
    static std::vector<Token> scan(std::ifstream& fin);
private:
    void new_line();
    void close_word();
    void analyse();
};

#endif //TINY_COMPILE_TOKEN_SCAN_H
