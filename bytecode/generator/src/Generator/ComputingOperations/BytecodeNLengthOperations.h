#pragma once
#include <limits>
#include "Generator/BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/BytecodeGenerationContext.h"
#include "Generator/BytecodeGenerator.h"

namespace ska {
  namespace bytecode {
	template <Command command, class NodeIterable, size_t Modulo = 3>
		std::size_t ApplyNOperations(Generator& generator, InstructionOutput& output, GenerationContext& script, NodeIterable&& node, std::size_t maxSize = std::numeric_limits<std::size_t>::max()) {
			auto temporaryContainer = std::vector<Operand> {};

			std::size_t index = 0u;
			for (const auto& paramNode : std::forward<NodeIterable>(node)) {
				if (paramNode != nullptr && index < maxSize) {
					auto finalGroup = generator.generatePart({ script, *paramNode });
					temporaryContainer.push_back(finalGroup.operand());
					output.push(std::move(finalGroup));
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