#pragma once

#include "ScriptHandleAST.h"
#include "ScriptCacheBase.h"

namespace ska {
	struct ScriptCacheAST :
		public ScriptCacheBase<ScriptHandleAST> {
	};
}
