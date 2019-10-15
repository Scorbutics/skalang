#pragma once
#include "Interpreter/Value/TypedNodeValue.h"

namespace ska {
	
	NodeValue InterpretLogicLesser(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);
	NodeValue InterpretLogicLesserOrEqual(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);
	NodeValue InterpretLogicGreaterOrEqual(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);
	NodeValue InterpretLogicGreater(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);
	NodeValue InterpretLogicCondition(TypedNodeValue firstValue, TypedNodeValue secondValue, const Type& destinationType);
}
