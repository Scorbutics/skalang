#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "Service/IsSmartPtr.h"

namespace ska {

	template <class ScriptT>
	struct ScriptCacheBase {
	public:
		auto find(const std::string& scriptName) { return namedMapCache.find(scriptName); }

		auto* operator[](std::size_t index) {
			return index < cache.size() ? cache[index].get() : nullptr;
		}

		const auto* operator[](std::size_t index) const {
			return index < cache.size() ? cache[index].get() : nullptr;
		}

		auto begin() { return namedMapCache.begin(); }
		auto end() { return namedMapCache.end(); }

		auto size() const { return cache.size(); }
		auto& back() { if(cache.back() == nullptr) { throw std::runtime_error("null back script"); } return *cache.back(); }
		const auto& back() const { if(cache.back() == nullptr) { throw std::runtime_error("null back script"); } return *cache.back(); }

		auto& at(const std::string& scriptName) {
			const auto index = namedMapCache.at(scriptName);
			return at(index);
		}

		auto& at(std::size_t index) {
			if (index >= cache.size() || cache[index] == nullptr) {
				throw std::runtime_error("bad script index \"" + std::to_string(index) + "\"");
			}
			return *cache[index];
		}

		bool emplace(std::string scriptName, ScriptT script) {
			const auto wantedScriptId = namedMapCache.size();
			const auto emplacedItem = namedMapCache.emplace(std::move(scriptName), wantedScriptId);
			if(emplacedItem.second) {
				pushCache(wantedScriptId, std::move(script));
				return false;
			} else {
				pushCache(emplacedItem.first->second, std::move(script));
				return true;
			}
		}

		std::size_t id(const std::string& scriptName) {
			if(namedMapCache.find(scriptName) == namedMapCache.end()) {
				const auto wantedScriptId = namedMapCache.size();
				namedMapCache.emplace(scriptName, wantedScriptId);
				pushCache(wantedScriptId, {});
				return wantedScriptId;
			}
			return namedMapCache.at(scriptName);
		}

		void resizeIfTooSmall(std::size_t length) {
			if (cache.size() < length) {
				cache.resize(length);
			}
		}

		void clear() {
			namedMapCache.clear();
			cache.clear();
		}

	private:
		using ScriptTPtr = std::conditional_t<is_smart_ptr<ScriptT>::value, ScriptT, std::unique_ptr<ScriptT>>;
		using ScriptContainer = std::vector<ScriptTPtr>;

		void pushCache(std::size_t index, ScriptTPtr script) {
			if(index >= cache.size()) {
				cache.resize(index + 1);
			}
			if constexpr (is_smart_ptr<ScriptT>::value) {
				cache[index] = std::move(script);
			} else {
				cache[index] = std::make_unique<ScriptT>(std::move(script));
			}
		}
		std::unordered_map<std::string, std::size_t> namedMapCache;
		ScriptContainer cache;
	};
}