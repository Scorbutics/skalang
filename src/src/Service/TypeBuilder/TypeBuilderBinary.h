#pragma once

#include "TypeBuilderOperator.h"
#include "Operation/Type/OperationTypeBinary.h"

namespace ska {
	class TypeCrosser;

    template<>
    struct TypeBuilderOperator<Operator::BINARY> :
		public TypeBuildUnit {
	private:
		using OperateOn = OperationType<Operator::BINARY>;
		const TypeCrosser& m_typeCrosser;
	public:
		TypeBuilderOperator(const TypeCrosser& typeCrosser);
        Type build(const ScriptAST& script, const ASTNode& node) override final {
			return build(script, OperateOn{node});
		}
		Type build(const ScriptAST& script, OperateOn node);
	};
}
