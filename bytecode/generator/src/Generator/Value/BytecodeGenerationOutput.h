#pragma once
#include <vector>
#include <unordered_map>
#include "BytecodeScriptGenerationOutput.h"
#include "BytecodeScript.h"

namespace ska {
	namespace bytecode {
		using ScriptGenerationOutputContainer = std::vector<ScriptGenerationOutput>;
		using ScriptGenerationServiceContainer = std::vector<ScriptGenerationService>;
		using ScriptGenerationNameToIndexMap = std::unordered_map<std::string, std::size_t>;

		class GenerationOutput {
		public:
			GenerationOutput() = default;

			std::size_t push(ScriptGenerationService service);

			void setOut(std::size_t index, ScriptGenerationOutput scriptOutput);

			GenerationOutput(const GenerationOutput&) = delete;
			GenerationOutput& operator=(const GenerationOutput&) = delete;
			GenerationOutput(GenerationOutput&&) = default;
			GenerationOutput& operator=(GenerationOutput&&) = default;
			~GenerationOutput() = default;

			auto size() const { return m_services.size(); }

			auto& backService() { return m_services.back(); }
			const auto& generated() const { return m_output; }
			auto& back() { return m_output.back(); }
			const auto& back() const { return m_output.back(); }

			std::pair<std::size_t, ScriptGenerationService*> script(const std::string& fullScriptName);

		private:
			ScriptGenerationServiceContainer m_services;
			ScriptGenerationOutputContainer m_output;
			ScriptGenerationNameToIndexMap m_mapping;
		};
	}
}
