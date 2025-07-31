
#include "entity.hpp"

namespace Demoph {

uint32_t Entity::s_nextID = 1;

Entity::Entity(const std::string& name) 
    : m_id(s_nextID++)
    , m_name(name)
    , m_active(true) {
}

} // namespace Demoph
