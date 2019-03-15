#include "NodeValue/AST.h"
#include "Service/Script.h"
#include "OperationImport.h"


ska::ASTNode* ska::Operation<ska::Operator::IMPORT>::GetScriptNode() {
	return &node.script().rootNode();
}
