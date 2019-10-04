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
		auto* find(std::size_t index) const { return index < cache.size() ? cache[index].get() : nullptr; }

		auto begin() { return namedMapCache.begin(); }
		auto end() { return namedMapCache.end(); }

		auto& at(const std::string& scriptName) {
			const auto index = namedMapCache.at(scriptName);
			return at(index);
		}

		auto& at(std::size_t index) {
			if (index >= cache.size()) {
				throw std::runtime_error("bad script index \"" + std::to_string(index) + "\"");
			}
			return cache[index];
		}

		void emplace(std::string scriptName, ScriptTHandlePtr script) {
			const auto wantedScriptId = cache.size();
			const auto emplacedItem = namedMapCache.emplace(std::move(scriptName), wantedScriptId);
			if(emplacedItem.second) {
				cache.push_back(std::move(script));
			} else {
				cache[emplacedItem.first->second] = std::move(script);
			}
		}

		std::size_t id(const std::string& scriptName) {
			if(namedMapCache.find(scriptName) == namedMapCache.end()) {
				const auto wantedScriptId = cache.size();
				namedMapCache.emplace(scriptName, wantedScriptId);
				cache.push_back(nullptr);
				return wantedScriptId;
				
			}
			return namedMapCache.at(scriptName);
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