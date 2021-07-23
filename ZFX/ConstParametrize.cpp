#include "IRVisitor.h"
#include "Stmts.h"
#include <map>

namespace zfx {

struct ParamMaxCounter : Visitor<ParamMaxCounter> {
    using visit_stmt_types = std::tuple
        < AsmParamLoadStmt
        >;

    int nuniforms = 0;

    void visit(AsmParamLoadStmt *stmt) {
        nuniforms = std::max(nuniforms, stmt->mem + 1);
    }
};

struct ConstParametrize : Visitor<ConstParametrize> {
    using visit_stmt_types = std::tuple
        < AsmLoadConstStmt
        , Statement
        >;

    std::unique_ptr<IR> ir = std::make_unique<IR>();

    int nuniforms = 0;
    std::map<std::string, int> constants;

    int lookup(std::string const &expr) {
        if (auto it = constants.find(expr); it != constants.end()) {
            return it->second;
        }
        int constid = nuniforms + constants.size();
        constants[expr] = constid;
        return constid;
    }

    void visit(Statement *stmt) {
        ir->push_clone_back(stmt);
    }

    void visit(AsmLoadConstStmt *stmt) {
        int constid = lookup(stmt->value);
        ir->emplace_back<AsmParamLoadStmt>(constid, stmt->dst);
    }

    auto getConstants() const {
        std::map<int, std::string> res;
        for (auto const &[name, idx]: constants) {
            res[idx] = name;
        }
        return res;
    }
};

std::map<int, std::string> apply_const_parametrize(IR *ir) {
    ParamMaxCounter counter;
    counter.apply(ir);
    ConstParametrize visitor;
    visitor.nuniforms = counter.nuniforms;
    visitor.apply(ir);
    *ir = *visitor.ir;
    return visitor.getConstants();
}

}
