#include "jaot/ir_lowering.h"

#include "jaot/parser.h"

#include <unordered_map>

namespace JAOT {
    namespace {
        using LocalIds = std::unordered_map<std::string, IR::LocalId>;
        using MethodReturnTypes = std::unordered_map<std::string, Type>;

        [[noreturn]] void loweringError(const std::string &message) {
            throw std::runtime_error("IR lowering error: " + message);
        }

        class MethodLowerer {
        public:
            explicit MethodLowerer(const MethodReturnTypes &methodReturnTypes) : methodReturnTypes_(methodReturnTypes) {}

            IR::Method lower(const Method &method) {
                IR::Method result;
                result.name = method.name;
                result.returnType = method.returnType;

                for (const Parameter &parameter : method.parameters) {
                    const IR::LocalId id = addLocal(result, parameter.name, parameter.type, true);
                    result.parameters.push_back(id);
                }

                for (const Stmt &statement : method.body) {
                    if (statement.kind == StmtKind::VarDecl) {
                        addLocal(result, statement.name, Type::Int, false);
                    }
                }

                IR::BasicBlock entry;
                entry.id = 0;

                bool hasReturn = false;

                for (const Stmt &statement : method.body) {
                    if (statement.kind == StmtKind::VarDecl && !statement.expression) {
                        continue;
                    }

                    if (statement.kind == StmtKind::Return) {
                        entry.terminator.kind = IR::TerminatorKind::Return;
                        entry.terminator.location = statement.location;

                        if (statement.expression) {
                            entry.terminator.expression = lowerExpression(*statement.expression);
                        }

                        hasReturn = true;
                        break;
                    }

                    entry.statements.push_back(lowerStatement(statement));
                }

                if (!hasReturn) {
                    if (method.returnType != Type::Void) {
                        loweringError("int method is missing return: " + method.name);
                    }

                    entry.terminator.kind = IR::TerminatorKind::Return;
                }

                result.blocks.push_back(std::move(entry));
                return result;
            }

        private:
            IR::LocalId addLocal(IR::Method &method, const std::string &name, Type type, bool isParameter) {
                if (localIds_.contains(name)) {
                    loweringError("duplicate local: " + name);
                }

                if (nextLocalId_ > std::numeric_limits<IR::LocalId>::max()) {
                    loweringError("too many locals in method " + method.name);
                }

                const auto id = static_cast<IR::LocalId>(nextLocalId_++);
                localIds_.emplace(name, id);
                method.locals.push_back({id, name, type, isParameter});
                return id;
            }

            IR::LocalId findLocal(const std::string &name) const {
                const auto found = localIds_.find(name);
                if (found == localIds_.end()) {
                    loweringError("unresolved local: " + name);
                }

                return found->second;
            }

            std::unique_ptr<IR::Expr> lowerExpression(const Expr &expression) const {
                auto result = std::make_unique<IR::Expr>();
                result->type = Type::Int;
                result->location = expression.location;

                switch (expression.kind) {
                    case ExprKind::Integer:
                        result->kind = IR::ExprKind::Integer;
                        result->integer = expression.integer;
                        return result;

                    case ExprKind::Variable:
                        result->kind = IR::ExprKind::Local;
                        result->local = findLocal(expression.name);
                        return result;

                    case ExprKind::Binary:
                        result->kind = IR::ExprKind::Binary;
                        result->binaryOperator = expression.op;
                        result->left = lowerExpression(*expression.left);
                        result->right = lowerExpression(*expression.right);
                        return result;

                    case ExprKind::Call: {
                        result->kind = IR::ExprKind::Call;
                        result->callee = expression.callee;

                        if (expression.callee == "printInt") {
                            result->type = Type::Void;
                        } else {
                            const auto method =
                                    methodReturnTypes_.find(expression.callee);
                            if (method == methodReturnTypes_.end()) {
                                loweringError(
                                    "unresolved function: " + expression.callee);
                            }
                            result->type = method->second;
                        }

                        for (const auto &argument : expression.arguments) {
                            result->arguments.push_back(
                                lowerExpression(*argument));
                        }

                        return result;
                    }
                }

                loweringError("unknown expression kind");
            }


            IR::Statement lowerStatement(const Stmt &statement) const {
                IR::Statement result;
                result.location = statement.location;

                switch (statement.kind) {
                    case StmtKind::VarDecl: {
                        result.kind = IR::StatementKind::AssignLocal;
                        result.target = findLocal(statement.name);
                        result.expression = lowerExpression(*statement.expression);

                        return result;
                    }

                    case StmtKind::Assignment:
                        result.kind = IR::StatementKind::AssignLocal;
                        result.target = findLocal(statement.name);
                        result.expression =
                                lowerExpression(*statement.expression);
                        return result;

                    case StmtKind::Expression:
                        result.kind = IR::StatementKind::Evaluate;
                        result.expression =
                                lowerExpression(*statement.expression);
                        return result;

                    case StmtKind::Return:
                        loweringError(
                            "return must be lowered as a block terminator"
                        );
                }

                loweringError("unknown statement kind");
            }

            const MethodReturnTypes &methodReturnTypes_;
            LocalIds localIds_;
            std::size_t nextLocalId_ = 0;
        };
    }

    IR::Program IrLowerer::lower(const Program &program) const {
        MethodReturnTypes methodReturnTypes;
        for (const Method &method : program.methods) {
            if (!methodReturnTypes.emplace(method.name, method.returnType).second) {
                loweringError("duplicate method: " + method.name);
            }
        }

        IR::Program result;
        result.methods.reserve(program.methods.size());
        for (const Method &method : program.methods) {
            MethodLowerer lowerer(methodReturnTypes);
            result.methods.push_back(lowerer.lower(method));
        }

        return result;
    }
}
