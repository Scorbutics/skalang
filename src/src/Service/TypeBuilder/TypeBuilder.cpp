#include "Config/LoggerConfigLang.h"
#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "NodeValue/AST.h"
#include "Service/ASTFactory.h"

#include "NodeValue/ScriptAST.h"
#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(LogLevel::Disabled, TypeBuilder)

ska::TypeBuilder::TypeBuilder(StatementParser& parser, const TypeCrosser& typeCrosser) :
  subobserver_priority_queue<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<FunctionTokenEvent>(std::bind(&TypeBuilder::matchFunction, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<VarTokenEvent>(std::bind(&TypeBuilder::matchVariable, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ReturnTokenEvent>(std::bind(&TypeBuilder::matchReturn, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ArrayTokenEvent>(std::bind(&TypeBuilder::matchArray, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ScriptLinkTokenEvent>(std::bind(&TypeBuilder::matchScriptLink, this, std::placeholders::_1), parser, 6){
	m_typeBuilder = BuildTypeBuildersContainer(typeCrosser);
}


bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type for variable \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for variable \"" << event.rootNode() << "\" = \"" << event.rootNode().type() << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}
void ska::TypeBuilder::buildType(ASTNode& node, ScriptAST& script) {
	if (node.type().has_value()) {
		return;
	}
	for (auto& child : node) {
		buildType(*child, script);
	}
	auto& typeBuilder = m_typeBuilder[static_cast<std::size_t>(node.op())];
	assert(typeBuilder != nullptr && "Cannot calculate the node type (it might be an empty node)");
	auto typeHierarchy = typeBuilder->build(script, node);
	node.updateType(typeHierarchy.type);
}

bool ska::TypeBuilder::matchReturn(ReturnTokenEvent& event) {
	if(event.type() != ReturnTokenEventType::START) {
		auto& node = event.rootNode();
		buildType(node, event.script());
  }
	return true;
}

bool ska::TypeBuilder::matchArray(ArrayTokenEvent & event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type array (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for array = \"" << event.rootNode().type() << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}

bool ska::TypeBuilder::matchScriptLink(ScriptLinkTokenEvent& event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type for script link \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for script link \"" << event.rootNode() << "\" = \"" << event.rootNode().type() << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) {
	auto& node = event.rootNode();
	SLOG(LogLevel::Debug) << "Building type for expression \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
	buildType(node, event.script());
	SLOG(LogLevel::Info) << "Type built for expression \"" << event.rootNode() << "\" = \"" << event.rootNode().type() << "\" (Operator " << event.rootNode().op() << ")";
  return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) {	
	if (event.type() != FunctionTokenEventType::DECLARATION_NAME) {
		auto& node = event.rootNode();
		SLOG(LogLevel::Debug) << "Building type for function parameter declaration / call \"" << event.rootNode() << "\" (Operator " << event.rootNode().op() << ")";
		buildType(node, event.script());
		SLOG(LogLevel::Info) << "Type built for function parameter declaration / call \"" << event.rootNode() << "\" = \"" << event.rootNode().type() << "\" (Operator " << event.rootNode().op() << ")";
	}
  return true;
}


