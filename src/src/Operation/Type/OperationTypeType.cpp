#include "OperationTypeType.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ExpressionType.h"

const ska::Symbol* ska::OperationType<ska::Operator::TYPE>::GetSymbol(const SymbolTable& symbolTable) const {
	auto& typeNameNode = node[0];
	if (typeNameNode.size() == 1) {
		const auto* importedScriptSymbol = symbolTable[typeNameNode.name()];
		return importedScriptSymbol == nullptr ? nullptr : (importedScriptSymbol->getType())[typeNameNode[0].name()];
	} 
	
	return symbolTable[typeNameNode.name()];
}

bool ska::OperationType<ska::Operator::TYPE>::IsBuiltIn() const {
	return node[0].size() == 0 && ExpressionTypeMap.find(node[0].name()) != ExpressionTypeMap.end();
}

bool ska::OperationType<ska::Operator::TYPE>::IsObject() const {
	return !node[1].logicalEmpty() && !IsBuiltIn();
}

std::string ska::OperationType<ska::Operator::TYPE>::GetName() const {
	auto& typeNode = node[0];
	return typeNode.size() == 1 ? (typeNode.name() + "::" + typeNode[0].name()) : typeNode.name();
}
