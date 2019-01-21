#include "Service/SymbolTable.h"
#include "OperationFieldAccess.h"

ska::Type ska::Operation<Operator::FIELD_ACCESS>::GetObjectType(const SymbolTable& symbols) {
	assert(GetObject().type() == ExpressionType::OBJECT && node.size() == 2);
	auto objectType = GetObject().type().value();
	const auto& typeName = GetObjectTypeName();
	if (typeName.empty() || objectType.userDefinedSymbolTable() == nullptr) {
		return Type { ExpressionType::VOID };
	}
	
	auto symbolObject = symbols[typeName];
	if (symbolObject != nullptr) {
		objectType = symbolObject->getType();
	}
	
	return objectType;
}

ska::Symbol* ska::Operation<Operator::FIELD_ACCESS>::GetFieldSymbolFromObjectSymbolTable(const Type& objectType) {
	const auto fieldAccessed = GetFieldName();
	assert(!fieldAccessed.empty());
	
	const auto& userDefinedTable = objectType.userDefinedSymbolTable()->children();
	assert(userDefinedTable.size() > 0);
	const auto& table = userDefinedTable.back();
	return (*table)(fieldAccessed);
}

