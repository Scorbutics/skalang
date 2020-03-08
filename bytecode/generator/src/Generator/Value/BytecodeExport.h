#pragma once
#include "BytecodeOperand.h"

namespace ska {
	class Symbol;

	namespace bytecode {
		struct ExportSymbol {
			Operand value;
			const Symbol* symbol = nullptr;

			friend bool operator==(const ExportSymbol& lhs, const ExportSymbol& rhs);
			friend bool operator!=(const ExportSymbol& lhs, const ExportSymbol& rhs);
		};

		static bool operator==(const ExportSymbol& lhs, const ExportSymbol& rhs) {
			return lhs.value == rhs.value;
		}

		static bool operator!=(const ExportSymbol& lhs, const ExportSymbol& rhs) {
			return lhs.value != rhs.value;
		}
	}
}
