#pragma once

#include "math.hpp"

namespace cs350 {
    struct material
    {
        glm::vec3 diffuse;
        float     specular_reflection;
        float     specular_exponent;
        glm::vec3 attenuation;
        float     electric_perm;
        float     magnetic_perm;
        float     n;

        void compute_n()
        {
            n = glm::sqrt(electric_perm * magnetic_perm);
        }
    };
}