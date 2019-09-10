#pragma once
#include <vector>
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScript.h"

namespace ska {
	namespace bytecode {
		using ScriptGenerationOutputContainer = std::vector<ScriptGenerationOutput>;
		using ScriptGenerationServiceContainer = std::vector<ScriptGenerationService>;
		class GenerationOutput {
		public:
			GenerationOutput(ScriptGenerationService service) {
				m_services.emplace_back(std::move(service));
			}

			void push_back(ScriptGenerationOutput scriptOutput) {
				m_output.emplace_back(std::move(scriptOutput));
			}
			GenerationOutput(const GenerationOutput&) = delete;
			GenerationOutput& operator=(const GenerationOutput&) = delete;
			GenerationOutput(GenerationOutput&&) = default;
			GenerationOutput& operator=(GenerationOutput&&) = default;

			GenerationOutput() = default;
			~GenerationOutput() = default;

			auto& backService() { return m_services.back(); }
			const auto& generated() const { return m_output; }
			auto& back() { return m_output.back(); }
			const auto& back() const { return m_output.back(); }
		private:
			ScriptGenerationServiceContainer m_services;
			ScriptGenerationOutputContainer m_output;
		};
	}
}
