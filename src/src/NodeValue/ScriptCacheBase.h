#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "Service/IsSmartPtr.h"

namespace ska {
	struct ScriptCacheBaseLog;
}

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::ScriptCacheBaseLog);

namespace ska {

	template <class ScriptT>
	struct ScriptCacheBase {
	private:
		using ScriptTPtr = std::conditional_t<is_smart_ptr<ScriptT>::value, ScriptT, std::unique_ptr<ScriptT>>;
		using ScriptTRaw = typename remove_smart_ptr<ScriptT>::type;
		using ScriptContainer = std::vector<ScriptTPtr>;
	public:
		auto find(const std::string& scriptName) { return namedMapCache.find(scriptName); }

		auto& operator[](std::size_t index) {
			assert(index < cache.size());
			return *cache[index];
		}

		const auto& operator[](std::size_t index) const {
			assert(index < cache.size());
			return *cache[index];
		}

		auto begin() { return namedMapCache.begin(); }
		auto end() { return namedMapCache.end(); }

		auto size() const { return cache.size(); }
		auto& back() { if(cache.back() == nullptr) { throw std::runtime_error("null back script"); } return *cache.back(); }
		const auto& back() const { if(cache.back() == nullptr) { throw std::runtime_error("null back script"); } return *cache.back(); }

		bool exist(std::size_t index) const {
			return index < cache.size() && cache[index] != nullptr;
		}

		auto* atOrNull(const std::string& scriptName) {
			const auto it = namedMapCache.find(scriptName);
			return it != namedMapCache.end() ? &at(it->second) : nullptr;
		}

		const auto* atOrNull(const std::string& scriptName) const {
			const auto it = namedMapCache.find(scriptName);
			return it != namedMapCache.end() ? &at(it->second) : nullptr;
		}

		ScriptTRaw& at(const std::string& scriptName) {
			const auto index = namedMapCache.at(scriptName);
			auto* result = atOrNull(index);
			if (result == nullptr) {
				throw std::runtime_error("bad script name \"" + scriptName + "\" with index \"" + std::to_string(index) + "\"");
			}
			return *result;
		}

		ScriptTRaw& at(std::size_t index) {
			if (!exist(index)) {
				throw std::runtime_error("bad script index \"" + std::to_string(index) + "\"");
			}
			return *cache[index];
		}

		const ScriptTRaw& at(std::size_t index) const {
			if (!exist(index)) {
				throw std::runtime_error("bad script index \"" + std::to_string(index) + "\"");
			}
			return *cache[index];
		}

		bool emplace(std::string scriptName, ScriptT script, bool force = false) {
			const auto wantedScriptId = findNextAvailableScriptId();
			const auto emplacedItem = namedMapCache.emplace(std::move(scriptName), wantedScriptId);
			if(emplacedItem.second) {
				SLOG_STATIC(LogLevel::Info, ScriptCacheBaseLog) << "Adding script " << emplacedItem.first->first << " at index " << emplacedItem.first->second;
				pushCache(wantedScriptId, std::move(script));
				return false;
			} else if (force || !exist(emplacedItem.first->second)) {
				SLOG_STATIC(LogLevel::Info, ScriptCacheBaseLog) << "Adding script " << emplacedItem.first->first << " at index " << emplacedItem.first->second;
				pushCache(emplacedItem.first->second, std::move(script));
			}
			return true;
		}

		void force(std::size_t scriptId, std::string scriptName, ScriptT script) {
			auto previous = namedMapCache.find(scriptName);
			if (previous != namedMapCache.end()) {
				cache[previous->second] = nullptr;
				namedMapCache.erase(previous);
			}
			namedMapCache[scriptName] = scriptId;
			SLOG_STATIC(LogLevel::Info, ScriptCacheBaseLog) << "Force script " << scriptName << " at index " << scriptId;
			pushCache(scriptId, std::move(script));
		}

		template <class ScriptTLocal>
		ScriptTLocal& emplaceNamed(ScriptTLocal script, bool force = false) {
			auto name = script.name();
			emplace(name, std::move(script), force);
			return at(name);
		}

		template <class ScriptTLocal>
		void set(std::size_t index, ScriptTLocal script) {
			if (index >= cache.size()) {
				throw std::runtime_error("unable to add a new script by using the index setter : use \"emplace\" first.");
			}
			static_assert(std::is_same_v<ScriptTLocal, ScriptT> || std::is_same_v<ScriptTLocal, ScriptTPtr>);
			if constexpr (is_smart_ptr<ScriptTLocal>::value) {
				cache[index] = std::move(script);
			} else {
				cache[index] = std::make_unique<ScriptTLocal>(std::move(script));
			}
		}

		std::size_t id(const std::string& scriptName) {
			if(namedMapCache.find(scriptName) == namedMapCache.end()) {
				const auto wantedScriptId = findNextAvailableScriptId();
				namedMapCache.emplace(scriptName, wantedScriptId);
				pushCachePtr(wantedScriptId, nullptr);
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
		std::size_t findNextAvailableScriptId() const {
			std::size_t i;
			for(i = 0; i < cache.size(); i++) {
				if(cache[i] == nullptr) {
					return i;
				}
			}
			return i;
		}

		ScriptTRaw* atOrNull(std::size_t index) {
			if (!exist(index)) {
				return nullptr;
			}
			return cache[index].get();
		}

		void pushCachePtr(std::size_t index, ScriptTPtr script) {
			resizeIfTooSmall(index + 1);
			cache[index] = std::move(script);
		}

		void pushCache(std::size_t index, ScriptT script) {
			resizeIfTooSmall(index + 1);
			if constexpr(is_smart_ptr<ScriptT>::value) {
				cache[index] = std::move(script);
			} else {
				cache[index] = std::make_unique<ScriptT>(std::move(script));
			}
		}

		std::unordered_map<std::string, std::size_t> namedMapCache;
		ScriptContainer cache;
	};
}