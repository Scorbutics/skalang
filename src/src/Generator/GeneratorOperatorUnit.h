#pragma once

#include "Value/BytecodeCell.h"

namespace ska {
	class BytecodeGenerator;
	class ExecutionContext;

	class GeneratorOperatorUnit {
	public:
		virtual BytecodeCell generate(ExecutionContext& node) = 0;
	};

	class GeneratorOperatorBase :
		public GeneratorOperatorUnit {
	public:
		GeneratorOperatorBase(BytecodeGenerator& generator) :
			m_generator(generator) {}
	protected:
		BytecodeGenerator& m_generator;
	};

}
