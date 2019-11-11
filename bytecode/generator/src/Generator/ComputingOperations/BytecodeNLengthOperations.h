#pragma once
#include <limits>
#include "Generator/BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationService.h"
#include "Generator/BytecodeGenerationContext.h"

namespace ska {
  namespace bytecode {
    template <Command command, class NodeIterable, size_t Modulo = 3>
		std::size_t ApplyNOperations(ScriptGenerationOutput& output, GenerationContext& script, NodeIterable&& node,  std::size_t maxSize = std::numeric_limits<std::size_t>::max()) {
			auto temporaryContainer = std::vector<Value> {};

			std::size_t index = 0u;
			for (const auto& paramNode : std::forward<NodeIterable>(node)) {
				if (paramNode != nullptr && index < maxSize) {
					temporaryContainer.push_back(script.querySymbolOrValue(*paramNode));
					if (temporaryContainer.size() == Modulo) {
						output.push(Instruction { command, std::move(temporaryContainer)});
						temporaryContainer = {};
					}
				}
				index++;
			}
			assert(temporaryContainer.size() <= Modulo);
			if (!temporaryContainer.empty()) {
				output.push(Instruction{ command, std::move(temporaryContainer)});
			}
			return index;
		}
  }
}