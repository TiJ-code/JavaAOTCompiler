#include "jaot/ir_lowering.h"

#include "jaot/parser.h"

#include <limits>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace JAOT {
    namespace {
        using MethodReturnTypes = std::unordered_map<std::string, Type>;

        [[noreturn]] void loweringError(const std::string &message) {
            throw std::runtime_error("IR lowering error: " + message);
        }

        class MethodLowerer {
        public:
            explicit MethodLowerer(const MethodReturnTypes &returnTypes)
                : returnTypes_(returnTypes) {}

            IR::Method lower(const Method &method) {
                result_.name = method.name;
                result_.returnType = method.returnType;
                scopes_.emplace_back();

                for (const Parameter &parameter : method.parameters) {
                    const IR::LocalId id =
                        addLocal(parameter.name, parameter.type, true);
                    result_.parameters.push_back(id);
                }

                IR::BasicBlock entry;
                entry.id = 0;
                result_.blocks.push_back(std::move(entry));
                currentBlock_ = 0;

                lowerStatements(method.body);

                if (currentBlock_) {
                    IR::Terminator &terminator = block(*currentBlock_).terminator;
                    if (method.returnType != Type::Void) {
                        loweringError(
                            "non-void method is missing return: " + method.name);
                    }
                    terminator.kind = IR::TerminatorKind::Return;
                    currentBlock_.reset();
                }

                return std::move(result_);
            }

        private:
            IR::BlockId newBlock() {
                if (result_.blocks.size() >
                    std::numeric_limits<IR::BlockId>::max()) {
                    loweringError("too many basic blocks in method " + result_.name);
                }
                const auto id =
                    static_cast<IR::BlockId>(result_.blocks.size());
                IR::BasicBlock basicBlock;
                basicBlock.id = id;
                result_.blocks.push_back(std::move(basicBlock));
                return id;
            }

            IR::BasicBlock &block(IR::BlockId id) {
                if (id >= result_.blocks.size() || result_.blocks[id].id != id) {
                    loweringError("invalid basic block id");
                }
                return result_.blocks[id];
            }

            IR::LocalId addLocal(
                const std::string &name,
                Type type,
                bool isParameter) {
                if (scopes_.back().contains(name)) {
                    loweringError("duplicate local: " + name);
                }
                if (result_.locals.size() >
                    std::numeric_limits<IR::LocalId>::max()) {
                    loweringError("too many locals in method " + result_.name);
                }

                const auto id =
                    static_cast<IR::LocalId>(result_.locals.size());
                scopes_.back().emplace(name, id);
                result_.locals.push_back({id, name, type, isParameter});
                return id;
            }

            IR::LocalId findLocal(const std::string &name) const {
                for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
                    const auto found = scope->find(name);
                    if (found != scope->end()) {
                        return found->second;
                    }
                }
                loweringError("unresolved local: " + name);
            }

            Type localType(IR::LocalId id) const {
                if (id >= result_.locals.size()) {
                    loweringError("invalid local id");
                }
                return result_.locals[id].type;
            }

            std::unique_ptr<IR::Expr> lowerExpression(const Expr &expression) {
                auto lowered = std::make_unique<IR::Expr>();
                lowered->location = expression.location;

                switch (expression.kind) {
                    case ExprKind::Integer:
                        lowered->kind = IR::ExprKind::Integer;
                        lowered->type = Type::Int;
                        lowered->integer = expression.integer;
                        return lowered;

                    case ExprKind::Boolean:
                        lowered->kind = IR::ExprKind::Boolean;
                        lowered->type = Type::Boolean;
                        lowered->boolean = expression.boolean;
                        return lowered;

                    case ExprKind::Variable:
                        lowered->kind = IR::ExprKind::Local;
                        lowered->local = findLocal(expression.name);
                        lowered->type = localType(lowered->local);
                        return lowered;

                    case ExprKind::Binary:
                        lowered->kind = IR::ExprKind::Binary;
                        lowered->binaryOperator = expression.op;
                        lowered->left = lowerExpression(*expression.left);
                        lowered->right = lowerExpression(*expression.right);
                        lowered->type =
                            expression.op == "+" || expression.op == "-" ||
                            expression.op == "*" || expression.op == "/"
                                ? Type::Int
                                : Type::Boolean;
                        return lowered;

                    case ExprKind::Unary:
                        lowered->kind = IR::ExprKind::Unary;
                        lowered->unaryOperator = expression.op;
                        lowered->operand = lowerExpression(*expression.operand);
                        lowered->type = Type::Boolean;
                        return lowered;

                    case ExprKind::Call: {
                        lowered->kind = IR::ExprKind::Call;
                        lowered->callee = expression.callee;
                        if (expression.callee == "printInt" ||
                            expression.callee == "printBool") {
                            lowered->type = Type::Void;
                        } else {
                            const auto found = returnTypes_.find(expression.callee);
                            if (found == returnTypes_.end()) {
                                loweringError(
                                    "unresolved function: " + expression.callee);
                            }
                            lowered->type = found->second;
                        }
                        for (const auto &argument : expression.arguments) {
                            lowered->arguments.push_back(
                                lowerExpression(*argument));
                        }
                        return lowered;
                    }
                }

                loweringError("unknown expression kind");
            }

            void appendStatement(IR::Statement statement) {
                if (!currentBlock_) {
                    loweringError("statement lowered after terminated block");
                }
                block(*currentBlock_).statements.push_back(std::move(statement));
            }

            void lowerStatements(const std::vector<Stmt> &statements) {
                for (const Stmt &statement : statements) {
                    if (!currentBlock_) {
                        return;
                    }
                    lowerStatement(statement);
                }
            }

            void lowerScopedStatements(const std::vector<Stmt> &statements) {
                scopes_.emplace_back();
                lowerStatements(statements);
                scopes_.pop_back();
            }

            void lowerStatement(const Stmt &statement) {
                switch (statement.kind) {
                    case StmtKind::VarDecl: {
                        std::unique_ptr<IR::Expr> initializer;
                        if (statement.expression) {
                            initializer = lowerExpression(*statement.expression);
                        }

                        const IR::LocalId id = addLocal(
                            statement.name, statement.declaredType, false);
                        if (initializer) {
                            IR::Statement assignment;
                            assignment.kind = IR::StatementKind::AssignLocal;
                            assignment.location = statement.location;
                            assignment.target = id;
                            assignment.expression = std::move(initializer);
                            appendStatement(std::move(assignment));
                        }
                        return;
                    }

                    case StmtKind::Assignment: {
                        IR::Statement assignment;
                        assignment.kind = IR::StatementKind::AssignLocal;
                        assignment.location = statement.location;
                        assignment.target = findLocal(statement.name);
                        assignment.expression = lowerExpression(*statement.expression);
                        appendStatement(std::move(assignment));
                        return;
                    }

                    case StmtKind::Expression: {
                        IR::Statement evaluate;
                        evaluate.kind = IR::StatementKind::Evaluate;
                        evaluate.location = statement.location;
                        evaluate.expression = lowerExpression(*statement.expression);
                        appendStatement(std::move(evaluate));
                        return;
                    }

                    case StmtKind::Return: {
                        IR::Terminator &terminator =
                            block(*currentBlock_).terminator;
                        terminator.kind = IR::TerminatorKind::Return;
                        terminator.location = statement.location;
                        if (statement.expression) {
                            terminator.expression =
                                lowerExpression(*statement.expression);
                        }
                        currentBlock_.reset();
                        return;
                    }

                    case StmtKind::Block:
                        lowerScopedStatements(statement.body);
                        return;

                    case StmtKind::If: {
                        const IR::BlockId thenId = newBlock();
                        const IR::BlockId elseId =
                            statement.hasElse ? newBlock() : 0;
                        const IR::BlockId mergeId = newBlock();
                        const IR::BlockId falseId =
                            statement.hasElse ? elseId : mergeId;

                        IR::Terminator &branch =
                            block(*currentBlock_).terminator;
                        branch.kind = IR::TerminatorKind::Branch;
                        branch.location = statement.location;
                        branch.condition = lowerExpression(*statement.condition);
                        branch.trueTarget = thenId;
                        branch.falseTarget = falseId;

                        currentBlock_ = thenId;
                        lowerScopedStatements(statement.body);
                        const bool thenFallsThrough = currentBlock_.has_value();
                        if (thenFallsThrough) {
                            IR::Terminator &jump =
                                block(*currentBlock_).terminator;
                            jump.kind = IR::TerminatorKind::Jump;
                            jump.target = mergeId;
                        }

                        bool elseFallsThrough = true;
                        if (statement.hasElse) {
                            currentBlock_ = elseId;
                            lowerScopedStatements(statement.elseBody);
                            elseFallsThrough = currentBlock_.has_value();
                            if (elseFallsThrough) {
                                IR::Terminator &jump =
                                    block(*currentBlock_).terminator;
                                jump.kind = IR::TerminatorKind::Jump;
                                jump.target = mergeId;
                            }
                        }

                        if (thenFallsThrough || elseFallsThrough) {
                            currentBlock_ = mergeId;
                        } else {
                            currentBlock_.reset();
                        }
                        return;
                    }

                    case StmtKind::While: {
                        const IR::BlockId conditionId = newBlock();
                        const IR::BlockId bodyId = newBlock();
                        const IR::BlockId exitId = newBlock();

                        IR::Terminator &entryJump = block(*currentBlock_).terminator;
                        entryJump.kind = IR::TerminatorKind::Jump;
                        entryJump.location = statement.location;
                        entryJump.target = conditionId;

                        currentBlock_ = conditionId;
                        IR::Terminator &branch = block(*currentBlock_).terminator;
                        branch.kind = IR::TerminatorKind::Branch;
                        branch.location = statement.location;
                        branch.condition = lowerExpression(*statement.condition);
                        branch.trueTarget = bodyId;
                        branch.falseTarget = exitId;

                        currentBlock_ = bodyId;
                        lowerScopedStatements(statement.body);
                        if (currentBlock_) {
                            IR::Terminator &backJump = block(*currentBlock_).terminator;
                            backJump.kind = IR::TerminatorKind::Jump;
                            backJump.location = statement.location;
                            backJump.target = conditionId;
                        }

                        currentBlock_ = exitId;
                        return;
                    }
                }

                loweringError("unknown statement kind");
            }

            const MethodReturnTypes &returnTypes_;
            IR::Method result_;
            std::vector<std::unordered_map<std::string, IR::LocalId>> scopes_;
            std::optional<IR::BlockId> currentBlock_;
        };
    }

    IR::Program IrLowerer::lower(const Program &program) const {
        MethodReturnTypes returnTypes;
        std::size_t methodCount = 0;

        for (const ClassDeclaration &declaration : program.classes) {
            methodCount += declaration.methods.size();

            for (const Method &method : declaration.methods) {
                if (!returnTypes.emplace(method.name, method.returnType).second) {
                    loweringError("duplicate method: " + method.name);
                }
            }
        }

        IR::Program result;
        result.classes.reserve(program.classes.size());

        for (const ClassDeclaration &declaration : program.classes) {
            IR::Class irClass;
            irClass.name = declaration.name;
            irClass.fields.reserve(declaration.fields.size());

            for (const Field &field : declaration.fields) {
                irClass.fields.push_back({.name = field.name, .typeName = field.typeName});
            }

            result.classes.push_back(std::move(irClass));
        }

        for (const ClassDeclaration &declaration : program.classes) {
            for (const Method &method : declaration.methods) {
                MethodLowerer lowerer(returnTypes);
                result.methods.push_back(lowerer.lower(method));
            }
        }

        return result;
    }
}
