#include "OperationTypeType.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ExpressionType.h"

std::optional<ska::Type> ska::OperationType<ska::Operator::TYPE>::GetSymbolType(const SymbolTable& symbolTable) const {
	assert(node.size() > 0);
	auto& typeNameNode = node[0];
	if (typeNameNode.size() == 1) {
		const auto* importedScriptSymbol = symbolTable[typeNameNode.name()];
		return importedScriptSymbol == nullptr ? std::optional<Type>{} : importedScriptSymbol->type().lookup(typeNameNode[0].name());
	}

	auto * symbol = symbolTable[typeNameNode.name()];
	return symbol != nullptr ? symbol->type() : std::optional<Type>{};
}

bool ska::OperationType<ska::Operator::TYPE>::IsBuiltIn() const {
	return node.size() > 0 && node[0].size() == 0 && ExpressionTypeMap.find(node[0].name()) != ExpressionTypeMap.end();
}

bool ska::OperationType<ska::Operator::TYPE>::IsObject() const {
	return node.size() > 1 && !node[1].logicalEmpty() && !IsBuiltIn();
}

std::string ska::OperationType<ska::Operator::TYPE>::GetName() const {
	assert(node.size() > 0);
	auto& typeNode = node[0];
	return typeNode.size() == 1 ? (typeNode.name() + "::" + typeNode[0].name()) : typeNode.name();
}

std::string ska::OperationType<ska::Operator::TYPE>::GetTypeName() const {
	assert(node.size() > 0);
	auto& typeNode = node[0];
	auto ss = std::stringstream{};
	ss << typeNode.type().value();
	return ss.str();
}
