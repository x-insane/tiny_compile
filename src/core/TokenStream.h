#ifndef TINY_COMPILE_TOKENSTREAM_H
#define TINY_COMPILE_TOKENSTREAM_H

#include <vector>
#include "Parser.h"

class TokenStream : public Parser::TokenInterface {
    const std::vector<Token>& list;
    int index = -1;

public:
    inline explicit TokenStream(std::vector<Token>& tokens) : list(tokens) { }

    inline Token nextToken() override {
        if (++index < list.size())
            return list[index];
        Token token;
        token.type = Token::Type::NO_MORE_TOKEN;
        return token;
    }
};

#endif //TINY_COMPILE_TOKENSTREAM_H
