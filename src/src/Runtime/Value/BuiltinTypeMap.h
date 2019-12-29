#pragma once

#include "StringShared.h"

namespace ska {

	template <class ... Types>
	class BuiltinTypeMap {
	public:
		static const std::vector<std::string>& types() {
			static const auto ss = std::vector<std::string>{};
			int _[] = { 0, (buildType<Types>(ss), 0)... };
			(void)_;
			return ss;
		}
	private:
		BuiltinTypeMap() = default;
		~BuiltinTypeMap() = default;

		template <class T>
		static void buildType(std::vector<std::string>& ss) {
			if constexpr (std::is_same<T, StringShared>()) {
				ss.push_back("string");
			} else if constexpr (std::is_same<T, int>()) {
				ss.push_back("int");
			} else if constexpr (std::is_same<T, long>()) {
				ss.push_back("int");
			} else if constexpr (std::is_same<T, std::size_t>()) {
				ss.push_back("int");
			} else if constexpr (std::is_same<T, float>()) {
				ss.push_back("float");
			} else if constexpr (std::is_same<T, bool>()) {
				ss.push_back("bool");
			} else if constexpr (std::is_same<T, double>()) {
				ss.push_back("float");
			} else if constexpr (std::is_same<T, void>()) {
				ss.push_back("void");
			} else {
				throw std::runtime_error("Invalid type for bridge function");
			}
		}
	};
}
