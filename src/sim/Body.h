#include <glm/glm/glm.hpp>

namespace Ouroboros{
    struct Body
    {
        glm::vec3 loc{0.0};
        float mass{1.0};
        float invMass{1.0};
        glm::vec3 vel{0.0};
        glm::vec3 force{0.0};
       
    };
}