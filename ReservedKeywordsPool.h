#pragma once

#include <unordered_map>

namespace ska {

	enum ReservedKeywords {
		FOR,
		IF,
		ELSE,
		VAR,
		FUNCTION
	};

	struct ReservedKeywordsPool {
		ReservedKeywordsPool() : 
			poolList(BuildList()),
			pool(BuildPool(poolList)) {
		}

		const std::vector<std::string> poolList;
		const std::unordered_map<std::string, std::size_t> pool;
		
	private:
		static std::vector<std::string> BuildList() {
			return std::vector<std::string>{ "for", "if", "else", "var", "function" };
		}

		static std::unordered_map<std::string, std::size_t> BuildPool(const std::vector<std::string>& reservedList) {
			auto pool = std::unordered_map<std::string, std::size_t>{};

			auto index = 0u;
			for (const auto& r : reservedList) {
				pool.emplace(r, index++);
			}
			return pool;
		}
	};
}