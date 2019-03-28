#include "NodeValue/AST.h"
#include "OperationImport.h"

ska::ScriptPtr ska::Operation<ska::Operator::IMPORT>::GetScript() {
	return parent.subScript(node[0].name() + ".miniska");
}
