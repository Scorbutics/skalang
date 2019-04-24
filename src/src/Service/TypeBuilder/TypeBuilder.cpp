#include "Config/LoggerConfigLang.h"
#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "NodeValue/AST.h"
#include "Service/ASTFactory.h"

#include "Interpreter/Value/Script.h"
#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(LogLevel::Debug, TypeBuilder)

ska::TypeBuilder::TypeBuilder(StatementParser& parser, const TypeCrosser& typeCrosser) :	
    subobserver_priority_queue<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<FunctionTokenEvent>(std::bind(&TypeBuilder::matchFunction, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<VarTokenEvent>(std::bind(&TypeBuilder::matchVariable, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ReturnTokenEvent>(std::bind(&TypeBuilder::matchReturn, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ArrayTokenEvent>(std::bind(&TypeBuilder::matchArray, this, std::placeholders::_1), parser, 6),
	subobserver_priority_queue<ScriptLinkTokenEvent>(std::bind(&TypeBuilder::matchScriptLink, this, std::placeholders::_1), parser, 6){
	m_typeBuilder = BuildTypeBuildersContainer(typeCrosser);
}
bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
	auto& node = event.rootNode();
	node.buildType(m_typeBuilder, event.script());
	SLOG(LogLevel::Debug) << "Type built for variable \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value() << "\" (Operator " << event.rootNode().op() << ")";
    return true;
}

bool ska::TypeBuilder::matchReturn(ReturnTokenEvent& event) const {
	if(event.type() != ReturnTokenEventType::START) {
		auto& node = event.rootNode();
		node.buildType(m_typeBuilder, event.script());
    }
	return true;
}

bool ska::TypeBuilder::matchArray(ArrayTokenEvent & event) const {
	auto& node = event.rootNode();
	node.buildType(m_typeBuilder, event.script());
	SLOG(LogLevel::Debug) << "Type built for array = \"" << event.rootNode().type().value() << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}

bool ska::TypeBuilder::matchScriptLink(ScriptLinkTokenEvent& event) const {
	auto& node = event.rootNode();
	node.buildType(m_typeBuilder, event.script());
	SLOG(LogLevel::Debug) << "Type built for script link \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value() << "\" (Operator " << event.rootNode().op() << ")";
	return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
	auto& node = event.rootNode();
	node.buildType(m_typeBuilder, event.script());
	SLOG(LogLevel::Debug) << "Type built for expression \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value() << "\" (Operator " << event.rootNode().op() << ")";
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {	
	if (event.type() != FunctionTokenEventType::DECLARATION_NAME) {
		auto& node = event.rootNode();
		node.buildType(m_typeBuilder, event.script());
		SLOG(LogLevel::Debug) << "Type built for function parameter declaration / call \"" << event.rootNode() << "\" = \"" << event.rootNode().type().value() << "\" (Operator " << event.rootNode().op() << ")";
	}
    return true;
}


