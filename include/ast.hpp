#ifndef AST
#define AST

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

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
    FOR_LOOP,
    BREAK,
    CONTINUE,
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
    std::vector <std::unique_ptr<Stmt>> body;
    NodeType getKind() const override{
        return kind;
    }
    Program();
};


class Expr : public Stmt{
    public:
    // Clone for assignment targets (Identifier, MemberExpr). Returns nullptr
    // for non-assignable expressions.
    virtual std::unique_ptr<Expr> clone() const{
        return nullptr;
    }
};

class VarDeclaration : public Stmt{
    NodeType kind = NodeType::VAR_D;
    public:

    bool constant = false;

    std::unordered_map <std::string, std::unique_ptr<Expr>> vars;

    NodeType getKind() const override{
        return kind;
    }
};

class FunDeclaration : public Stmt{
    NodeType kind = NodeType::FUN_D;
    public:

    std::vector <std::string> parameters;
    std::string name;
    std::vector <std::unique_ptr<Stmt>> body;

    NodeType getKind() const override{
        return kind;
    }
};

class AssignExpr : public Expr{
    private:
    NodeType kind = NodeType::ASSIGNEXPR;
    
    public:
    std::unique_ptr<Expr> assignexpr;
    std::unique_ptr<Expr> value;

    NodeType getKind() const override{
        return kind;
    }

};

class CondExpr : public Stmt{
    private:
    NodeType kind = NodeType::CONDEXPR;
    
    public:

    std::unique_ptr<Expr> condition;
    std::vector <std::unique_ptr<Stmt>> ThenBranch;
    std::vector <std::unique_ptr<Stmt>> ElseBranch;

    NodeType getKind() const override{
        return kind;
    }

};

class WhileStmt : public Stmt{
    private:
    NodeType kind = NodeType::WHILE_LOOP;
    
    public:

    std::unique_ptr<Expr> condition;
    std::vector <std::unique_ptr<Stmt>> ThenBranch;

    NodeType getKind() const override{
        return kind;
    }

};

class ForStmt : public Stmt{
    private:
    NodeType kind = NodeType::FOR_LOOP;
    
    public:

    std::unique_ptr<Stmt> iterator_dec;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> operation;
    std::vector <std::unique_ptr<Stmt>> ThenBranch;

    NodeType getKind() const override{
        return kind;
    }

};

class BreakStmt : public Stmt{
    private:
    NodeType kind = NodeType::BREAK;
    
    public:

    NodeType getKind() const override{
        return kind;
    }

};

class ContinueStmt : public Stmt{
    private:
    NodeType kind = NodeType::CONTINUE;
    
    public:

    NodeType getKind() const override{
        return kind;
    }

};

class BinaryExpr : public Expr{
    NodeType kind = NodeType::BINARYEXPR;
    public:
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    std::string op = "";

    NodeType getKind() const override{
        return kind;
    }
};

class NotExpr : public Expr{
    NodeType kind = NodeType::NOTEXPR;
    public:
    std::unique_ptr<Expr> val;

    NodeType getKind() const override{
        return kind;
    }
};

class Identifier : public Expr{
    NodeType kind = NodeType::IDENTIFIER;
    public:
    std::string symbol = "";
    Identifier(std::string val){
        this->symbol = val;
    }
    NodeType getKind() const override{
        return kind;
    }
    std::unique_ptr<Expr> clone() const override{
        return std::make_unique<Identifier>(symbol);
    }
};

class UnaryExpr : public Expr{
    NodeType kind = NodeType::UNARYEXPR;
    public:

    bool left;
    bool plus;
    std::unique_ptr<Expr> identifier;

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
    std::unique_ptr<Expr> val;

    NodeType getKind() const override{
        return kind;
    }
};


class ObjectLiteral : public Expr{
    NodeType kind = NodeType::OBJECT_L;
    public:

    std::vector <std::unique_ptr<PropertyLiteral>> properties;

    NodeType getKind() const override{
        return kind;
    }
};

class ElementLiteral : public Expr{
    NodeType kind = NodeType::ELEMENT_L;
    public:

    unsigned long long key;
    std::unique_ptr<Expr> val;

    NodeType getKind() const override{
        return kind;
    }
};

class ListLiteral : public Expr{
    NodeType kind = NodeType::LIST_L;
    public:

    std::vector <std::unique_ptr<ElementLiteral>> properties;

    NodeType getKind() const override{
        return kind;
    }
};

class MemberExpr : public Expr{
    NodeType kind = NodeType::MEMBEREXPR;
    public:

    std::unique_ptr<Expr> object;
    std::unique_ptr<Expr> property;
    bool computed;

    NodeType getKind() const override{
        return kind;
    }
    std::unique_ptr<Expr> clone() const override{
        auto temp = std::make_unique<MemberExpr>();
        temp->computed = computed;
        temp->object = object ? object->clone() : nullptr;
        temp->property = property ? property->clone() : nullptr;
        return temp;
    }
};

class CallExpr : public Expr{
    NodeType kind = NodeType::CALLEXPR;
    public:

    std::vector <std::unique_ptr<Expr>> args;
    std::unique_ptr<Expr> callexpr;

    NodeType getKind() const override{
        return kind;
    }
};

#endif