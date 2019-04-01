#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/ScriptBinding.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/ScriptCache.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Interpreter/Interpreter.h"

int main(int argc, char* argv[]) {
	if (argc <= 1) {
        std::cout << "No file name entered. Exiting...";
        return -1;
    }
	
    const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto scriptCache = ska::ScriptCache{};
	
	auto parser = ska::StatementParser {reservedKeywords};
	auto typeBuilder = ska::TypeBuilder {parser};
	auto symbolsTypeUpdater = ska::SymbolTableTypeUpdater {parser};
	auto typeChecker = ska::SemanticTypeChecker {parser};
	auto interpreter = ska::Interpreter {reservedKeywords};

	auto inputFile = std::ifstream{ argv[1] };
	if(inputFile.fail()) {
		std::cout << "File not found : \"" << argv[1] << "\"";
        return -1;
	}
	
	auto input = std::stringstream {};
	input << inputFile.rdbuf();

	auto tokenizer = ska::Tokenizer{reservedKeywords, input.str()};
	auto tokens = tokenizer.tokenize();
	auto reader = ska::Script {scriptCache, "main", std::move(tokens)};
	
	auto scriptBinding = ska::ScriptBridge{ scriptCache, "runner_lib", typeBuilder, symbolsTypeUpdater, reservedKeywords };
	scriptBinding.bindFunction("printInt", std::function<void(int)>([](int value) {
		std::cout << value << std::endl;
	}));
	
	scriptBinding.bindFunction("printString", std::function<void(std::string)>([](std::string value) {
		std::cout << value << std::endl;
	}));
	
	scriptBinding.bindGenericFunction("print", { "Dummy", "void" }, std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> params) -> ska::NodeValue {
		auto& mem = params[0].nodeval<ska::ObjectMemory>();
		auto& data = (*mem)["data"];
		std::cout << data.first->convertString() << std::endl;
		return ska::NodeValue{};
	}));
	scriptBinding.build();

	try {
		reader.parse(parser);
		interpreter.script(reader);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}


