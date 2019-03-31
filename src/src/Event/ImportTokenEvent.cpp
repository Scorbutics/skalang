#include <cassert>
#include "ImportTokenEvent.h"
#include "Service/Script.h"

ska::ImportTokenEvent::ImportTokenEvent(ASTNode& node, Script& s) : 
    m_node(node), 
    m_script(s),
    m_boundScript(m_script.subScript(node[0].name() + ".miniska")) {
    assert(m_boundScript != nullptr && "bad parameter : null bound script to an import event");
}