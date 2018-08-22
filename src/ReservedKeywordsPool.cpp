#include <vector>
#include "ReservedKeywordsPool.h"

std::unordered_map<std::string, ska::ReservedKeywordsPool::TokenInfo> ska::ReservedKeywordsPool::BuildPool() {
	auto pool = std::unordered_map<std::string, TokenInfo>{};
	
	const auto emplacer = [&](std::size_t index, TokenType type) {
		if(type == TokenType::RESERVED) {
			pool.emplace(TokenGrammarSTR[index], TokenInfo { TokenGrammarSTR[index], Token{ index, TokenType::RESERVED } });
		} else {
			pool.emplace(TokenGrammarSTR[index], TokenInfo { TokenGrammarSTR[index], Token{ TokenGrammarSTR[index], std::move(type) } });
		}
	};

	for(auto index = 0u; index < static_cast<std::size_t>(TokenGrammar::METHOD_CALL_OPERATOR); index++) {
		emplacer(index, TokenType::RESERVED);
	}

	emplacer(static_cast<std::size_t>(TokenGrammar::METHOD_CALL_OPERATOR), TokenType::DOT_SYMBOL);
	emplacer(static_cast<std::size_t>(TokenGrammar::AFFECTATION), TokenType::SYMBOL);
	emplacer(static_cast<std::size_t>(TokenGrammar::BLOCK_BEGIN), TokenType::RANGE);
	emplacer(static_cast<std::size_t>(TokenGrammar::BLOCK_END), TokenType::RANGE);
	emplacer(static_cast<std::size_t>(TokenGrammar::PARENTHESIS_BEGIN), TokenType::RANGE);
	emplacer(static_cast<std::size_t>(TokenGrammar::PARENTHESIS_END), TokenType::RANGE);
	emplacer(static_cast<std::size_t>(TokenGrammar::STRING_DELIMITER), TokenType::SYMBOL);
	emplacer(static_cast<std::size_t>(TokenGrammar::STATEMENT_END), TokenType::SYMBOL);
	emplacer(static_cast<std::size_t>(TokenGrammar::ARGUMENT_DELIMITER), TokenType::SYMBOL);
	emplacer(static_cast<std::size_t>(TokenGrammar::TYPE_DELIMITER), TokenType::SYMBOL);
	
	return pool;
	
}

std::vector<ska::ReservedKeywordsPool::TokenInfo> ska::ReservedKeywordsPool::BuildPatterns(const std::unordered_map<std::string, TokenInfo>& pool) {
	auto patterns = std::vector<TokenInfo>();
	patterns.resize(pool.size());

	auto index = 0u;
	for (const auto& p : pool) {
		if(p.second.token.type() == TokenType::RESERVED) {
			patterns[std::get<std::size_t>(p.second.token.content())] = p.second;
		}
	}

	const auto enumLength = static_cast<std::size_t>(TokenGrammar::UNUSED_Last_Length);
	for(auto index = static_cast<std::size_t>(TokenGrammar::METHOD_CALL_OPERATOR); index < enumLength; index++) {
		patterns[index] = pool.at(TokenGrammarSTR[index]);
	}

	return patterns;
}
