#pragma once

#include "ScriptHandleAST.h"
#include "ScriptCacheBase.h"

namespace ska {
	struct ScriptCacheAST :
		public ScriptCacheBase<std::unique_ptr<ScriptHandleAST>> {
	};
}
