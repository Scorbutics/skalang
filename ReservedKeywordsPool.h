#pragma once

#include <unordered_set>

namespace ska {
	struct ReservedKeywordsPool {
		ReservedKeywordsPool() : pool(BuildPool()) {}

		const std::unordered_set<std::string> pool;
	
	private:
		static std::unordered_set<std::string> BuildPool() {
			std::unordered_set<std::string> pool;
			pool.insert("for");
			pool.insert("if");
			pool.insert("else");
			pool.insert("var");
			pool.insert("function");
			return pool;
		}
	};
}