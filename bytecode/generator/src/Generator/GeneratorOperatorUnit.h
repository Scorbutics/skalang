#pragma once

#include "Value/BytecodeScriptGenerationOutput.h"

namespace ska {
	namespace bytecode {
		class Generator;
		class GenerationContext;

		class GeneratorOperatorUnit {
		public:
			virtual ScriptGenerationOutput generate(GenerationContext& node) = 0;
		};

		class GeneratorOperatorBase :
			public GeneratorOperatorUnit {
		public:
			GeneratorOperatorBase(Generator& generator) :
				m_generator(generator) {}

			ScriptGenerationOutput generateNext(GenerationContext node);

		private:
			Generator& m_generator;
		};
	}
}
