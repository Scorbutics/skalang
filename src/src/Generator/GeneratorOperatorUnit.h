#pragma once

#include "Value/BytecodeGenerationOutput.h"

namespace ska {
	namespace bytecode {
		class Generator;
		class GenerationContext;

		class GeneratorOperatorUnit {
		public:
			virtual GenerationOutput generate(GenerationContext& node) = 0;
		};

		class GeneratorOperatorBase :
			public GeneratorOperatorUnit {
		public:
			GeneratorOperatorBase(Generator& generator) :
				m_generator(generator) {}
		protected:
			Generator& m_generator;
		};
	}
}
