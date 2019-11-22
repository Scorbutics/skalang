#pragma once

#include "Value/BytecodeInstructionOutput.h"

namespace ska {
	namespace bytecode {
		class Generator;
		class GenerationContext;

		class GeneratorOperatorUnit {
		public:
			virtual InstructionOutput generate(GenerationContext& node) = 0;
			virtual ~GeneratorOperatorUnit() = default;
		};

		class GeneratorOperatorBase :
			public GeneratorOperatorUnit {
		public:
			GeneratorOperatorBase(Generator& generator) :
				m_generator(generator) {}


			InstructionOutput generateNext(GenerationContext node);

		private:
			Generator& m_generator;
		};
	}
}
