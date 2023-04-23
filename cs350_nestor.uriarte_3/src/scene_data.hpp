#pragma once

#include "geometry.hpp"
#include "material.hpp"

namespace cs350 {
    struct scene_triangle
    {
        cs350::triangle geometry;
        cs350::material material;

        glm::vec3 const& operator[](int index) const { return geometry[index]; }
        glm::vec3&       operator[](int index) { return geometry[index]; }
    };

    struct scene_light
    {
        glm::vec3 position;
        glm::vec3 intensity;
        float     radius;
    };

    struct scene_intersection
    {
        scene_triangle const* triangle;
        float                 intersection_time;

        bool operator()() const { return triangle != nullptr && intersection_time >= 0.0f; }
    };
    static_assert(std::is_trivial<scene_intersection>());
    static_assert(std::is_standard_layout<scene_intersection>());
}