#pragma once

#include "Value/BytecodeCellGroup.h"

namespace ska {
	class BytecodeGenerator;
	class BytecodeGenerationContext;

	class GeneratorOperatorUnit {
	public:
		virtual BytecodeCellGroup generate(BytecodeGenerationContext& node) = 0;
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
