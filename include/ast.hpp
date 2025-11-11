#ifndef AST
#define AST

#include <vector>
#include <string>


#include <iostream> // temp

enum class NodeType{
    NONE,
    PROGRAM,
    VAR_D,
    FUN_D,
    ASSIGNEXPR,
    MEMBEREXPR,
    CALLEXPR,
    NOTEXPR,
    CONDEXPR,
    WHILE_LOOP,
    NUMERIC_L,
    STRING_L,
    PROPERTY_L,
    ELEMENT_L,
    OBJECT_L,
    LIST_L,
    IDENTIFIER,
    BINARYEXPR,
    UNARYEXPR
};

class Stmt{
    NodeType kind = NodeType::NONE;
    public:
    virtual NodeType getKind() const{
        return kind;
    }
    virtual ~Stmt() = default;
};

class Program : public Stmt{
    NodeType kind = NodeType::PROGRAM;
    public:
    std::vector <Stmt*> body;
    NodeType getKind() const override{
        return kind;
    }
    Program();
};


class Expr : public Stmt{
    public:
};

class VarDeclaration : public Stmt{
    NodeType kind = NodeType::VAR_D;
    public:

    bool constant = false;
    std::string identifier;
    Expr* val;

    NodeType getKind() const override{
        return kind;
    }
};

class FunDeclaration : public Stmt{
    NodeType kind = NodeType::FUN_D;
    public:

    std::vector <std::string> parameters;
    std::string name;
    std::vector <Stmt*> body;

    NodeType getKind() const override{
        return kind;
    }
};

class AssignExpr : public Expr{
    private:
    NodeType kind = NodeType::ASSIGNEXPR;
    
    public:
    Expr* assignexpr;
    Expr* value;

    NodeType getKind() const override{
        return kind;
    }

};

class CondExpr : public Stmt{
    private:
    NodeType kind = NodeType::CONDEXPR;
    
    public:

    Expr* condition;
    std::vector <Stmt*> ThenBranch;
    std::vector <Stmt*> ElseBranch;

    NodeType getKind() const override{
        return kind;
    }

};

class WhileStmt : public Stmt{
    private:
    NodeType kind = NodeType::WHILE_LOOP;
    
    public:

    Expr* condition;
    std::vector <Stmt*> ThenBranch;

    NodeType getKind() const override{
        return kind;
    }

};


class BinaryExpr : public Expr{
    NodeType kind = NodeType::BINARYEXPR;
    public:
    Expr* left;
    Expr* right;
    std::string op = "";

    NodeType getKind() const override{
        return kind;
    }
};

class NotExpr : public Expr{
    NodeType kind = NodeType::NOTEXPR;
    public:
    Expr* val;

    NodeType getKind() const override{
        return kind;
    }
};

class Identifier : public Expr{
    NodeType kind = NodeType::IDENTIFIER;
    public:
    std::string symbol = "";

    Identifier(std::string val){
        NodeType kind = NodeType::IDENTIFIER;
        this->symbol = val;
    }
    NodeType getKind() const override{
        return kind;
    }
};

class UnaryExpr : public Expr{
    NodeType kind = NodeType::UNARYEXPR;
    public:

    bool left;
    bool plus;
    Expr* identifier;

    NodeType getKind() const override{
        return kind;
    }
};

class NumericLiteral : public Expr{
    NodeType kind = NodeType::NUMERIC_L;
    public:
    long double val;
    NumericLiteral(std::string val);

    NodeType getKind() const override{
        return kind;
    }
};

class StringLiteral : public Expr{
    NodeType kind = NodeType::STRING_L;
    public:

    std::string val;

    StringLiteral(std::string val);

    NodeType getKind() const override{
        return kind;
    }
};

class PropertyLiteral : public Expr{
    NodeType kind = NodeType::PROPERTY_L;
    public:

    std::string key;
    Expr* val = nullptr;

    NodeType getKind() const override{
        return kind;
    }
};


class ObjectLiteral : public Expr{
    NodeType kind = NodeType::OBJECT_L;
    public:

    std::vector <PropertyLiteral*> properties;

    NodeType getKind() const override{
        return kind;
    }
};

class ElementLiteral : public Expr{
    NodeType kind = NodeType::ELEMENT_L;
    public:

    unsigned long long key;
    Expr* val = nullptr;

    NodeType getKind() const override{
        return kind;
    }
};

class ListLiteral : public Expr{
    NodeType kind = NodeType::LIST_L;
    public:

    std::vector <ElementLiteral*> properties;

    NodeType getKind() const override{
        return kind;
    }
};

class MemberExpr : public Expr{
    NodeType kind = NodeType::MEMBEREXPR;
    public:

    Expr* object = nullptr;
    Expr* property = nullptr;
    bool computed;

    NodeType getKind() const override{
        return kind;
    }
};

class CallExpr : public Expr{
    NodeType kind = NodeType::CALLEXPR;
    public:

    std::vector <Expr*> args;
    Expr* callexpr = nullptr;

    NodeType getKind() const override{
        return kind;
    }
};

#endif