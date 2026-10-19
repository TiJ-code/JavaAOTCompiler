#include "jaot/ir_lowering.h"

#include <algorithm>

#include "jaot/parser.h"

#include <limits>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace JAOT {
    namespace {
        using MethodReturnTypes = std::unordered_map<std::string, MethodSignature>;
        using ClassFields = std::unordered_map<std::string, std::vector<IR::Field>>;

        [[noreturn]] void loweringError(const std::string &message) {
            throw std::runtime_error("IR lowering error: " + message);
        }

        class MethodLowerer {
        public:
            MethodLowerer(
                const MethodReturnTypes &returnTypes,
                const std::unordered_set<std::string> &classNames,
                const ClassFields &classFields,
                const std::string &currentClass)
                : returnTypes_(returnTypes),
                  classNames_(classNames),
                  classFields_(classFields),
                  currentClass_(currentClass),
                  result_() {
            }

            IR::Method lower(const Method &method) {
                result_.signature.name = method.name;
                result_.signature.ownerClass = currentClass_;
                result_.signature.returnType = method.returnType;
                for (const Parameter &parameter : method.parameters) {
                    result_.signature.parameterTypes.push_back(parameter.type);
                }
                scopes_.emplace_back();

                for (const Parameter &parameter : method.parameters) {
                    const IR::LocalId id =
                        addLocal(
                            parameter.name,
                            parameter.type,
                            true);
                    result_.parameters.push_back(id);
                }

                IR::BasicBlock entry;
                entry.id = 0;
                result_.blocks.push_back(std::move(entry));
                currentBlock_ = 0;

                lowerStatements(method.body);

                if (currentBlock_) {
                    IR::Terminator &terminator = block(*currentBlock_).terminator;
                    if (method.returnType.kind != Type::Void) {
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
                    loweringError(
                        "too many basic blocks in method " +
                        result_.signature.name);
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
                TypeRef type,
                bool isParameter
            ) {
                if (scopes_.back().contains(name)) {
                    loweringError("duplicate local: " + name);
                }

                if (result_.locals.size() >
                    std::numeric_limits<IR::LocalId>::max()) {
                    loweringError(
                        "too many locals in method " +
                        result_.signature.name);
                }

                const auto id =
                    static_cast<IR::LocalId>(result_.locals.size());

                scopes_.back().emplace(name, id);
                result_.locals.push_back({
                    .id = id,
                    .name = name,
                    .type = std::move(type),
                    .isParameter = isParameter
                });

                return id;
            }

            [[nodiscard]] IR::LocalId findLocal(const std::string &name) const {
                for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
                    const auto found = scope->find(name);
                    if (found != scope->end()) {
                        return found->second;
                    }
                }
                loweringError("unresolved local: " + name);
            }

            [[nodiscard]] TypeRef localType(IR::LocalId id) const {
                if (id >= result_.locals.size()) {
                    loweringError("invalid local id");
                }
                return result_.locals[id].type;
            }

            [[nodiscard]] std::optional<IR::LocalId> findLocalIfPresent(
                const std::string &name
            ) const {
                for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
                    const auto found = scope->find(name);
                    if (found != scope->end()) {
                        return found->second;
                    }
                }
                return std::nullopt;
            }

            static void requireAssignable(
                const TypeRef &source,
                const TypeRef &target,
                const std::string &context
            ) {
                if (!isAssignable(source, target)) {
                    loweringError(
                        "cannot assign " + typeName(source) + " to " +
                        typeName(target) + " in " + context);
                }
            }

            std::unique_ptr<IR::Expr> lowerExpression(const Expr &expression) {
                auto lowered = std::make_unique<IR::Expr>();
                lowered->location = expression.location;

                switch (expression.kind) {
                    case ExprKind::Integer:
                        lowered->kind = IR::ExprKind::Integer;
                        lowered->type = TypeRef{Type::Int, {}};
                        lowered->integer = expression.integer;
                        return lowered;

                    case ExprKind::Boolean:
                        lowered->kind = IR::ExprKind::Boolean;
                        lowered->type = TypeRef{Type::Boolean, {}};
                        lowered->boolean = expression.boolean;
                        return lowered;

                    case ExprKind::Null:
                        lowered->kind = IR::ExprKind::Null;
                        lowered->type = TypeRef{Type::Reference, {}};
                        return lowered;

                    case ExprKind::NewObject:
                        if (!classNames_.contains(expression.className)) {
                            loweringError(
                                "unknown class: " + expression.className);
                        }
                        lowered->kind = IR::ExprKind::NewObject;
                        lowered->type =
                            TypeRef{Type::Reference, expression.className};
                        lowered->className = expression.className;
                        return lowered;

                    case ExprKind::Variable:
                        lowered->kind = IR::ExprKind::Local;
                        lowered->local = findLocal(expression.name);
                        lowered->type = localType(lowered->local);
                        if (lowered->type.kind == Type::Reference) {
                            lowered->className = lowered->type.className;
                        }
                        return lowered;

                    case ExprKind::FieldAccess: {
                        if (!expression.left) {
                            loweringError("incomplete field access");
                        }

                        lowered->kind = IR::ExprKind::FieldAccess;
                        lowered->left = lowerExpression(*expression.left);

                        if (lowered->left->type.kind != Type::Reference) {
                            loweringError("field receiver is not a reference");
                        }

                        const auto classFields =
                            classFields_.find(lowered->left->type.className);

                        if (classFields == classFields_.end()) {
                            loweringError(
                                "unknown class: " +
                                lowered->left->type.className);
                        }

                        const auto field = std::ranges::find_if(
                            classFields->second,
                            [&expression](const IR::Field &candidate) {
                                 return candidate.name == expression.name;
                            }
                        );

                        if (field == classFields->second.end()) {
                            loweringError(
                                "unknown field '" + expression.name +
                                "' in class " +
                                lowered->left->type.className);
                        }

                        lowered->fieldOffset = field->offset;
                        lowered->type = field->type;
                        if (lowered->type.kind == Type::Reference) {
                            lowered->className = lowered->type.className;
                        }

                        return lowered;
                    }

                    case ExprKind::Binary:
                        lowered->kind = IR::ExprKind::Binary;
                        lowered->binaryOperator = expression.op;
                        lowered->left = lowerExpression(*expression.left);
                        lowered->right = lowerExpression(*expression.right);

                        if (expression.op == "+" || expression.op == "-" ||
                            expression.op == "*" || expression.op == "/") {
                            if (lowered->left->type.kind != Type::Int ||
                                lowered->right->type.kind != Type::Int) {
                                loweringError(
                                    "arithmetic operators require integer operands");
                            }
                        } else if (lowered->left->type.kind !=
                                       lowered->right->type.kind) {
                            loweringError(
                                "comparison operands must have compatible types");
                        }

                        lowered->type =
                            expression.op == "+" || expression.op == "-" ||
                            expression.op == "*" || expression.op == "/"
                                ? TypeRef{Type::Int, {}}
                                : TypeRef{Type::Boolean, {}};
                        return lowered;

                    case ExprKind::Unary:
                        lowered->kind = IR::ExprKind::Unary;
                        lowered->unaryOperator = expression.op;
                        lowered->operand = lowerExpression(*expression.operand);
                        if (expression.op == "!" &&
                            lowered->operand->type.kind != Type::Boolean) {
                            loweringError(
                                "logical negation requires a boolean operand");
                        }
                        lowered->type = TypeRef{Type::Boolean, {}};
                        return lowered;

                    case ExprKind::Call: {
                        lowered->kind = IR::ExprKind::Call;

                        if (expression.callee == "printInt" ||
                            expression.callee == "printBool") {
                            lowered->callee = MethodSignature{
                                .name = expression.callee,
                                .ownerClass = "<runtime>",
                                .returnType = TypeRef{Type::Void, {}},
                                .parameterTypes = {
                                    TypeRef{
                                        expression.callee == "printInt"
                                            ? Type::Int
                                            : Type::Boolean,
                                        {}
                                    }
                                }
                            };
                        } else {
                            std::string owner = currentClass_;

                            if (!expression.calleeClass.empty()) {
                                const auto receiver =
                                    findLocalIfPresent(expression.calleeClass);

                                if (receiver) {
                                    const TypeRef receiverType =
                                        localType(*receiver);
                                    if (receiverType.kind != Type::Reference) {
                                        loweringError(
                                            "call receiver is not a reference");
                                    }

                                    owner = receiverType.className;
                                    lowered->receiver =
                                        std::make_unique<IR::Expr>();
                                    lowered->receiver->kind =
                                        IR::ExprKind::Local;
                                    lowered->receiver->local = *receiver;
                                    lowered->receiver->type = receiverType;
                                    lowered->receiver->className =
                                        receiverType.className;
                                } else if (!classNames_.contains(
                                               expression.calleeClass)) {
                                    loweringError(
                                        "unknown call receiver: " +
                                        expression.calleeClass);
                                } else {
                                    owner = expression.calleeClass;
                                }
                            }

                            const auto found =
                                returnTypes_.find(owner + "." + expression.callee);
                            if (found == returnTypes_.end()) {
                                loweringError(
                                    "unresolved function: " +
                                    owner + "." + expression.callee);
                            }

                            lowered->callee = found->second;
                        }

                        for (const auto &argument : expression.arguments) {
                            lowered->arguments.push_back(
                                lowerExpression(*argument));
                        }

                        lowered->type = lowered->callee.returnType;

                        if (lowered->callee.ownerClass != "<runtime>" &&
                            lowered->arguments.size() !=
                                lowered->callee.parameterTypes.size()) {
                            loweringError(
                                "wrong argument count for " +
                                lowered->callee.ownerClass + "." +
                                lowered->callee.name);
                        }

                        if (lowered->callee.ownerClass != "<runtime>") {
                            for (std::size_t i = 0;
                                 i < lowered->arguments.size();
                                 ++i) {
                                requireAssignable(
                                    lowered->arguments[i]->type,
                                    lowered->callee.parameterTypes[i],
                                    lowered->callee.ownerClass + "." +
                                        lowered->callee.name);
                            }
                        } else {
                            requireAssignable(
                                lowered->arguments.front()->type,
                                lowered->callee.parameterTypes.front(),
                                lowered->callee.name);
                        }

                        if (lowered->callee.ownerClass == "<runtime>" &&
                            lowered->arguments.size() != 1) {
                            loweringError(
                                expression.callee + " expects one argument");
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
                        if (statement.declaredType.kind == Type::Reference &&
                            !classNames_.contains(statement.declaredType.className)) {
                            loweringError(
                                "unknown class: " + statement.declaredType.className);
                        }

                        std::unique_ptr<IR::Expr> initializer;
                        if (statement.expression) {
                            initializer = lowerExpression(*statement.expression);
                            requireAssignable(
                                initializer->type,
                                statement.declaredType,
                                "initializer for " + statement.name);
                        }

                        const IR::LocalId id = addLocal(
                            statement.name,
                            statement.declaredType,
                            false
                        );

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
                        if (statement.assignmentTarget) {
                            IR::Statement assignment;
                            assignment.kind = IR::StatementKind::AssignField;
                            assignment.location = statement.location;
                            assignment.targetExpression = lowerExpression(*statement.assignmentTarget);
                            assignment.expression = lowerExpression(*statement.expression);
                            requireAssignable(
                                assignment.expression->type,
                                assignment.targetExpression->type,
                                "field assignment");
                            appendStatement(std::move(assignment));
                            return;
                        }

                        IR::Statement assignment;
                        assignment.kind = IR::StatementKind::AssignLocal;
                        assignment.location = statement.location;
                        assignment.target = findLocal(statement.name);
                        assignment.expression = lowerExpression(*statement.expression);
                        requireAssignable(
                            assignment.expression->type,
                            localType(assignment.target),
                            "assignment to " + statement.name);
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
                            requireAssignable(
                                terminator.expression->type,
                                result_.signature.returnType,
                                "return from " + result_.signature.name);
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
            const std::unordered_set<std::string> &classNames_;
            const ClassFields &classFields_;
            const std::string &currentClass_;
            IR::Method result_;
            std::vector<std::unordered_map<std::string, IR::LocalId>> scopes_;
            std::optional<IR::BlockId> currentBlock_;
        };
    }

    IR::Program IrLowerer::lower(const Program &program) const {
        MethodReturnTypes returnTypes;
        std::unordered_set<std::string> classNames;
        ClassFields classFields;
        std::size_t methodCount = 0;

        for (const ClassDeclaration &declaration : program.classes) {
            classNames.insert(declaration.name);
            methodCount += declaration.methods.size();

            for (const Method &method : declaration.methods) {
                MethodSignature signature{
                    .name = method.name,
                    .ownerClass = declaration.name,
                    .returnType = method.returnType,
                    .parameterTypes = {}};
                for (const Parameter &parameter : method.parameters) {
                    signature.parameterTypes.push_back(parameter.type);
                }
                if (!returnTypes.emplace(
                        declaration.name + "." + method.name,
                        std::move(signature)).second) {
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

            std::size_t offset = 0;

            for (const Field &field : declaration.fields) {
                irClass.fields.push_back({
                    .name = field.name,
                    .type = field.type,
                    .offset = offset,
                    .size = field.type.kind == Type::Reference
                        ? std::size_t{8}
                        : std::size_t{4}
                });
                offset += irClass.fields.back().size;
            }

            irClass.size = offset;
            classFields.emplace(declaration.name, irClass.fields);
            result.classes.push_back(std::move(irClass));
        }

        result.methods.reserve(methodCount);

        for (const ClassDeclaration &declaration : program.classes) {
            for (const Method &method : declaration.methods) {
                MethodLowerer lowerer(
                    returnTypes, classNames, classFields, declaration.name);
                result.methods.push_back(lowerer.lower(method));
            }
        }

        return result;
    }
}
