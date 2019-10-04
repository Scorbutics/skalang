#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace ska {
	template <class ScriptT>
	struct ScriptCacheBase {
		using ScriptTHandlePtr = std::unique_ptr<ScriptT>;
	public:
		auto find(const std::string& scriptName) { return namedMapCache.find(scriptName); }

		auto begin() { return namedMapCache.begin(); }
		auto end() { return namedMapCache.end(); }

		auto& at(const std::string& scriptName) {
			const auto index = namedMapCache.at(scriptName);
			if (index >= cache.size()) {
				throw std::runtime_error("bad script index for script name \"" + scriptName + "\"");
			}
			return cache[index];
		}

		void emplace(std::string scriptName, ScriptTHandlePtr script) {
			const auto emplacedItem = namedMapCache.emplace(std::move(scriptName), cache.size());
			if(emplacedItem.second) {
				cache.push_back(std::move(script));
			} else {
				throw std::runtime_error("unable to emplace script cause it was already registered");
			}
		}

		void clear() {
			namedMapCache.clear();
			cache.clear();
		}

	private:
		std::unordered_map<std::string, std::size_t> namedMapCache;
		std::vector<ScriptTHandlePtr> cache;
	};
}