#pragma once
#include <limits>
#include "Generator/BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
  namespace bytecode {
    template <class NodeIterable, size_t Modulo = 3>
		void ApplyNOperations(Script& script, NodeIterable&& node, Command command, GenerationOutput& output, std::size_t maxSize = std::numeric_limits<std::size_t>::max()) {
			auto temporaryContainer = std::vector<Value> {};

			std::size_t index = 0u;
			for (const auto& paramNode : std::forward<NodeIterable>(node)) {
				if (paramNode != nullptr && index < maxSize) {
					temporaryContainer.push_back(script.queryVariableOrValue(*paramNode));
					if (temporaryContainer.size() == Modulo) {
						output.push(Instruction { command, std::move(temporaryContainer)});
						temporaryContainer = {};
					}
				}
				index++;
			}
			assert(temporaryContainer.size() <= Modulo);
			if (!temporaryContainer.empty()) {
				output.push(Instruction { command, std::move(temporaryContainer) });
			}
		}
  }
}