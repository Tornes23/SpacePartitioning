#pragma once

#include <vector>
#include "geometry.hpp"

namespace cs350 {
    /**
     * Load the triangles from an obj
     * @param filename 
     * @return std::vector<triangle> 
     */
    std::vector<triangle> load_obj(const char* filename);
}