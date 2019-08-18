#pragma once

#include <string>
#include "Type.h"
#include "NodeValueArray.h"
#include "StringShared.h"

namespace ska {
	template<class T>
	struct TypeFromNative;

	template<>
	struct TypeFromNative<int> {
		static const Type value();
	};
    
    template<>
	struct TypeFromNative<bool> {
		static const Type value();
	};

	template<>
	struct TypeFromNative<float> {
		static const Type value();
	};

	template<>
	struct TypeFromNative<double> {
		static const Type value();
	};

	template<>
	struct TypeFromNative<StringShared> {
		static const Type value();
	};

	template<>
	struct TypeFromNative<NodeValueArray> {
		static const Type value();
	};

}
