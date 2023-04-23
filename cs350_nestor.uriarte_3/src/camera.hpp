#pragma once

#include "math.hpp"

namespace cs350 {
    class camera
    {
      public:
        glm::vec3 center;
        glm::vec3 view;
        glm::vec3 up;
    };
}