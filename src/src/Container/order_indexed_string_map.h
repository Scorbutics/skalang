#pragma once
#include <Config/LoggerConfigLang.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "Service/IsSmartPtr.h"

namespace ska {
	struct order_indexed_string_map_log;
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::order_indexed_string_map_log);

namespace ska {

	template <class T>
	struct order_indexed_string_map {
	private:
		using TPtr = std::conditional_t<is_smart_ptr<T>::value, T, std::unique_ptr<T>>;
		using TRaw = typename remove_smart_ptr<T>::type;
		using LinearContainer = std::vector<TPtr>;
	public:
		auto find(const std::string& name) const {
			auto itName = namedMapCache.find(name);
			if (itName == namedMapCache.end()) { return cache.end(); }
			auto it = cache.begin();
			std::advance(it, itName->second);
			return it;
		}
		auto find(const std::string& name) {
			auto itName = namedMapCache.find(name); 
			if (itName == namedMapCache.end()) { return cache.end(); } 
			auto it = cache.begin();
			std::advance(it, itName->second);
			return it;
		}

		auto& operator[](std::size_t index) {
			assert(index < cache.size());
			return *cache[index];
		}

		const auto& operator[](std::size_t index) const {
			assert(index < cache.size());
			return *cache[index];
		}

		auto begin() { return cache.begin(); }
		auto end() { return cache.end(); }

		auto begin() const { return cache.begin(); }
		auto end() const { return cache.end(); }

		auto size() const { return cache.size(); }
		auto empty() const { return cache.empty(); }
		auto& back() { if(cache.back() == nullptr) { throw std::runtime_error("null back element"); } return *cache.back(); }
		const auto& back() const { if(cache.back() == nullptr) { throw std::runtime_error("null back element"); } return *cache.back(); }

		bool exist(std::size_t index) const {
			return index < cache.size() && cache[index] != nullptr;
		}

		auto* atOrNull(const std::string& name) {
			const auto it = namedMapCache.find(name);
			return it != namedMapCache.end() ? &at(it->second) : nullptr;
		}

		const auto* atOrNull(const std::string& name) const {
			const auto it = namedMapCache.find(name);
			return it != namedMapCache.end() ? &at(it->second) : nullptr;
		}

		TRaw& at(const std::string& name) {
			const auto index = namedMapCache.at(name);
			auto* result = atOrNull(index);
			if (result == nullptr) {
				throw std::runtime_error("bad element name \"" + name + "\" with index \"" + std::to_string(index) + "\"");
			}
			return *result;
		}

		TRaw& at(std::size_t index) {
			if (!exist(index)) {
				throw std::runtime_error("bad element index \"" + std::to_string(index) + "\"");
			}
			return *cache[index];
		}

		const TRaw& at(std::size_t index) const {
			if (!exist(index)) {
				throw std::runtime_error("bad element index \"" + std::to_string(index) + "\"");
			}
			return *cache[index];
		}

		bool emplace(std::string name, T element, bool force = false) {
			const auto wantedElementId = findNextAvailableElementId();
			const auto emplacedItem = namedMapCache.emplace(std::move(name), wantedElementId);
			if(emplacedItem.second) {
				SLOG_STATIC(LogLevel::Info, order_indexed_string_map_log) << "Adding element \"" << emplacedItem.first->first << "\" at index " << wantedElementId;
				pushCache(wantedElementId, std::move(element));
				return false;
			} else if (force || !exist(emplacedItem.first->second)) {
				SLOG_STATIC(LogLevel::Info, order_indexed_string_map_log) << "Adding element \"" << emplacedItem.first->first << "\" at index " << emplacedItem.first->second;
				pushCache(emplacedItem.first->second, std::move(element));
			}
			return true;
		}

		template <class TLocal>
		TLocal& emplaceNamed(TLocal elem, bool force = false) {
			auto name = elem.name();
			emplace(name, std::move(elem), force);
			return at(name);
		}

		template <class TLocal>
		void set(std::size_t index, TLocal element) {
			if (index >= cache.size()) {
				throw std::runtime_error("unable to add a new element by using the index setter : use \"emplace\" first.");
			}
			static_assert(std::is_same_v<TLocal, T> || std::is_same_v<TLocal, TPtr>);
			if constexpr (is_smart_ptr<TLocal>::value) {
				cache[index] = std::move(element);
			} else {
				cache[index] = std::make_unique<TLocal>(std::move(element));
			}
		}

		std::size_t id(const std::string& name) {
			if(namedMapCache.find(name) == namedMapCache.end()) {
				const auto wantedId = findNextAvailableElementId();
				namedMapCache.emplace(name, wantedId);
				pushCachePtr(wantedId, nullptr);
				return wantedId;
			}
			return namedMapCache.at(name);
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

		std::string findName(std::size_t index) const {
			for (const auto& [name, index_] : namedMapCache) {
				if (index_ == index) {
					return name;
				}
			}
			return "";
		}

	private:
		std::size_t findNextAvailableElementId() const {
			return namedMapCache.size();
		}

		TRaw* atOrNull(std::size_t index) {
			if (!exist(index)) {
				return nullptr;
			}
			return cache[index].get();
		}

		void pushCachePtr(std::size_t index, TPtr element) {
			resizeIfTooSmall(index + 1);
			cache[index] = std::move(element);
		}

		void pushCache(std::size_t index, T element) {
			resizeIfTooSmall(index + 1);
			if constexpr(is_smart_ptr<T>::value) {
				cache[index] = std::move(element);
			} else {
				cache[index] = std::make_unique<T>(std::move(element));
			}
		}

		std::unordered_map<std::string, std::size_t> namedMapCache;
		LinearContainer cache;
	};
}