#include "NodeValue/AST.h"
#include "OperationImport.h"

#include "Service/ScriptNameBuilder.h"

ska::ScriptPtr ska::Operation<ska::Operator::IMPORT>::GetScript() {
	return parent.createImport(ScriptNameDeduce(parent.name(), node[0].name()));
}
