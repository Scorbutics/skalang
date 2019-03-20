#include "NodeValue/AST.h"
#include "Service/Script.h"
#include "OperationImport.h"


ska::Script& ska::Operation<ska::Operator::IMPORT>::GetScript() {
	return node.script();
}
