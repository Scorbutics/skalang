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

			GenerationOutput generateNext(GenerationContext node);

		private:
			Generator& m_generator;
		};
	}
}
