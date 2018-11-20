#include "Parser.h"
#include "Helper.h"

void Parser::parse() {
    root = program();
    if (Log::hasError()) {
        std::stringstream msg;
        msg << "You have " << Log::getErrorCount() << " errors.";
        throw msg.str();
    }
}

bool Parser::match(Token::Type expected, bool flag) {
    if (token.type == expected) {
        last_token = token;
        token = interface.nextToken();
        return true;
    } else {
        if (flag) {
            if (token.type == Token::Type::NO_MORE_TOKEN) {
                Log::parse_error("expected token " + Helper::getTokenTypeName(expected)
                    + ", but there is no more token", last_token.line, last_token.offset);
            } else {
                Log::parse_error("unexpected token " + token.token + ", "
                    + Helper::getTokenTypeName(expected) + " expected", token.line, token.offset);
            }
        }
        return false;
    }
}

TreeNode* Parser::program() {
    token = interface.nextToken();
    declarations();
    return stmt_sequence();
}

void Parser::declarations() {
    VarType type = VarType::VT_VOID;
    Token::Type token_type = token.type;
    while (match(Token::Type::KEY_INT) || match(Token::Type::KEY_BOOL) || match(Token::Type::KEY_STRING)) {
        switch (token_type) {
            case Token::Type::KEY_INT:
                type = VarType::VT_INT;
                break;
            case Token::Type::KEY_BOOL:
                type = VarType::VT_BOOL;
                break;
            case Token::Type::KEY_STRING:
                type = VarType::VT_STRING;
                break;
            default:
                Log::parse_error("the token can not be parsed to a type: "
                    + Helper::getTokenTypeName(token.type), token.line, token.offset);
                break;
        }
        do {
            std::string name = token.token;
            if (token.type != Token::Type::ID) {
                Log::parse_error("unexpected token " + token.token + ", "
                    + Helper::getTokenTypeName(Token::Type::ID) + " expected", token.line, token.offset);
                break;
            }

            // 期望获取一个标识符
            match(Token::Type::ID);

            // 插入符号表
            const SymbolTable::Symbol& symbol = symbol_table.lookup(name);
            if (&symbol == &SymbolTable::Symbol::NONE)
                symbol_table.insert(name, type, token.line);
            else {
                std::stringstream msg;
                msg << "the variable " << last_token.token << " has already declared in line " << symbol.lines[0];
                Log::parse_error(msg.str(), last_token.line, last_token.offset);
            }
        } while(match(Token::Type::OP_COMMA));
        match(Token::Type::OP_SEMICOLON, true);
    }
}

TreeNode* Parser::stmt_sequence() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::STMT_SEQUENCE);
    node->children[0] = statement();
    if (match(Token::Type::OP_SEMICOLON))
        node->children[1] = stmt_sequence();
    else {
        TreeNode* tmp = node->children[0];
        node->children[0] = nullptr;
        delete node;
        node = tmp;
    }
    return node;
}

TreeNode* Parser::statement() {
    TreeNode* node = nullptr;
    switch (token.type) {
        case Token::Type::KEY_IF:
            node = if_stmt();
            break;
        case Token::Type::KEY_READ:
            node = read_stmt();
            break;
        case Token::Type::KEY_REPEAT:
            node = repeat_stmt();
            break;
        case Token::Type::KEY_WRITE:
            node = write_stmt();
            break;
        case Token::Type::KEY_WHILE:
            node = while_stmt();
            break;
        case Token::Type::ID:
            node = assign_stmt();
            break;
        default:
            break;
    }
    return node;
}

TreeNode* Parser::if_stmt() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::IF_STMT);
    match(Token::Type::KEY_IF, true);
    node->children[0] = or_exp();
    match(Token::Type::KEY_THEN, true);
    node->children[1] = stmt_sequence();
    if (match(Token::Type::KEY_ELSE))
        node->children[2] = stmt_sequence();
    match(Token::Type::KEY_END, true);
    return node;
}

TreeNode* Parser::repeat_stmt() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::REPEAT_STMT);
    match(Token::Type::KEY_REPEAT, true);
    node->children[0] = stmt_sequence();
    match(Token::Type::KEY_UNTIL, true);
    node->children[1] = or_exp();
    return node;
}

TreeNode* Parser::assign_stmt() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::ASSIGN_STMT);
    if (token.type == Token::Type::ID)
        node->children[0] = factor();
    match(Token::Type::OP_ASSIGN, true);
    node->children[1] = or_exp();
    return node;
}

TreeNode* Parser::read_stmt() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::READ_STMT);
    match(Token::Type::KEY_READ, true);
    if (token.type == Token::Type::ID)
        node->children[0] = factor();
    else
        Log::parse_error("unexpected token: " + token.token + ", expected ID", token.line, token.offset);
    return node;
}

TreeNode* Parser::write_stmt() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::WRITE_STMT);
    match(Token::Type::KEY_WRITE, true);
    node->children[0] = or_exp();
    return node;
}

TreeNode* Parser::while_stmt() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::WHILE_STMT);
    match(Token::Type::KEY_WHILE, true);
    node->children[0] = or_exp();
    match(Token::Type::KEY_DO, true);
    node->children[1] = stmt_sequence();
    match(Token::Type::KEY_END, true);
    return node;
}

TreeNode* Parser::or_exp() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::OR_EXP);
    node->children[0] = and_exp();
    if (match(Token::Type::KEY_OR))
        node->children[1] = or_exp();
    else {
        TreeNode* tmp = node->children[0];
        node->children[0] = nullptr;
        delete node;
        node = tmp;
    }
    return node;
}

TreeNode* Parser::and_exp() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::AND_EXP);
    node->children[0] = comparison_exp();
    if (match(Token::Type::KEY_AND))
        node->children[1] = and_exp();
    else {
        TreeNode* tmp = node->children[0];
        node->children[0] = nullptr;
        delete node;
        node = tmp;
    }
    return node;
}

TreeNode* Parser::comparison_exp() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::NONE);
    node->children[0] = add_sub_exp();
    switch (token.type) {
        case Token::Type::OP_LSS:
            match(Token::Type::OP_LSS, true);
            node->type = TreeNode::NodeType::LT_EXP;
            node->children[1] = comparison_exp();
            break;
        case Token::Type::OP_LEQ:
            match(Token::Type::OP_LEQ, true);
            node->type = TreeNode::NodeType::LE_EXP;
            node->children[1] = comparison_exp();
            break;
        case Token::Type::OP_GTR:
            match(Token::Type::OP_GTR, true);
            node->type = TreeNode::NodeType::GT_EXP;
            node->children[1] = comparison_exp();
            break;
        case Token::Type::OP_GEQ:
            match(Token::Type::OP_GEQ, true);
            node->type = TreeNode::NodeType::GE_EXP;
            node->children[1] = comparison_exp();
            break;
        case Token::Type::OP_EQU:
            match(Token::Type::OP_EQU, true);
            node->type = TreeNode::NodeType::EQ_EXP;
            node->children[1] = comparison_exp();
            break;
        default: {
            TreeNode* tmp = node->children[0];
            node->children[0] = nullptr;
            delete node;
            node = tmp;
            break;
        }
    }
    return node;
}

TreeNode* Parser::add_sub_exp() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::NONE);
    node->children[0] = mul_div_exp();
    switch (token.type) {
        case Token::Type::OP_ADD:
            match(Token::Type::OP_ADD, true);
            node->type = TreeNode::NodeType::PLUS_EXP;
            node->children[1] = add_sub_exp();
            break;
        case Token::Type::OP_SUB:
            match(Token::Type::OP_SUB, true);
            node->type = TreeNode::NodeType::SUB_EXP;
            node->children[1] = add_sub_exp();
            break;
        default: {
            TreeNode* tmp = node->children[0];
            node->children[0] = nullptr;
            delete node;
            node = tmp;
            break;
        }
    }
    return node;
}

TreeNode* Parser::mul_div_exp() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::NONE);
    node->children[0] = factor();
    switch (token.type) {
        case Token::Type::OP_MUL:
            match(Token::Type::OP_MUL, true);
            node->type = TreeNode::NodeType::MUL_EXP;
            node->children[1] = mul_div_exp();
            break;
        case Token::Type::OP_DIV:
            match(Token::Type::OP_DIV, true);
            node->type = TreeNode::NodeType::DIV_EXP;
            node->children[1] = mul_div_exp();
            break;
        default: {
            TreeNode* tmp = node->children[0];
            node->children[0] = nullptr;
            delete node;
            node = tmp;
            break;
        }
    }
    return node;
}

TreeNode* Parser::factor() {
    TreeNode* node = TreeNode::create(TreeNode::NodeType::FACTOR);
    switch (token.type) {
        case Token::Type::ID: {
            node->token = new Token;
            *node->token = token;
            const SymbolTable::Symbol& symbol = symbol_table.lookup(token.token);
            if(&symbol == &SymbolTable::Symbol::NONE)
                Log::parse_error("the symbol " + token.token + " is not declared", token.line, token.offset);
            else {
                node->varType = symbol.type;
                symbol_table.insert(token.token, node->varType, token.line);
            }
            match(Token::Type::ID, true);
            break;
        }
        case Token::Type::NUMBER:
            node->token = new Token;
            *node->token = token;
            node->varType = VarType::VT_INT;
            match(Token::Type::NUMBER, true);
            break;
        case Token::Type::STRING:
            node->token = new Token;
            *node->token = token;
            node->varType = VarType::VT_STRING;
            match(Token::Type::STRING, true);
            break;
        case Token::Type::KEY_TRUE:
            node->token = new Token();
            *node->token = token;
            node->varType = VarType::VT_BOOL;
            match(Token::Type::KEY_TRUE, true);
            break;
        case Token::Type::KEY_FALSE:
            node->token = new Token();
            *node->token = token;
            node->varType = VarType::VT_BOOL;
            match(Token::Type::KEY_FALSE, true);
            break;
        case Token::Type::OP_LP:
            node->type = TreeNode::NodeType::OR_EXP;
            match(Token::Type::OP_LP, true);
            node = or_exp();
            match(Token::Type::OP_RP, true);
            break;
        default:
            Log::parse_error("unexpected token: " + token.token, token.line, token.offset);
            break;
    }
    return node;
}
