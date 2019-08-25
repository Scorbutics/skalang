#pragma once

namespace ska {
	class ASTNode;
	namespace bytecode {
	class GenerationOutput;

	class SymbolInfoAdder {
	public:
		//void process(GenerationOutput& generated, const ASTNode& root);
	private:
		void processUnit(GenerationOutput& generated, const ASTNode& root);
	};
  }
}
