#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "../../glm/glm/glm.hpp"
#include "Observer.hpp"

namespace lights{

    struct LightStrength {
        glm::vec3 ambient{0.1f};
        glm::vec3 specular{1.0f};
        glm::vec3 diffuse{10.f};
    };

    class PointLight : public Subject<PointLight>{
        public:
        glm::vec3 position{0.0f};
        LightStrength LightStrength;
        
        void SetPosition(glm::vec3 position){
            this->position = position;
        }
    };
} // namespace lights

#endif