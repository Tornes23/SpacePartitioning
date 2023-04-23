#include <array>
#include "scene.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "raytracer.hpp"
#include "kdtree.hpp"
#include <iostream>

namespace cs350 {

    scene::scene()
    {
        // Air
        material material      = {};
        material.electric_perm = 1;
        material.magnetic_perm = 1;
        material.compute_n();
        material.attenuation = {1, 1, 1};
        set_air_material(material);
    }

    void scene::add_material(material mat)
    {
        mat.compute_n();
        m_materials.push_back(mat);
    }

    void scene::add_light(scene_light light)
    {
        m_lights.push_back(light);
    }

    void scene::add_mesh(char const* path, glm::mat4 const& m2w, unsigned material_index)
    {
        // Load mesh
        auto triangles = load_obj(path);
        // Transform triangles
        for (auto& geometry : triangles) {
            for (auto& pt : geometry.points) {
                // Transform point
                pt = glm::vec3(m2w * glm::vec4(pt, 1.0f));
            }

            // Submit triangle
            scene_triangle triangle{};
            triangle.geometry = geometry;
            triangle.material = m_materials.at(material_index);
            m_triangles.push_back(triangle);
        }
    }

    void scene::add_mesh(char const* path, glm::vec3 p, glm::vec3 r, glm::vec3 s, unsigned material_index)
    {
        glm::mat4 translation = glm::translate(p);
        glm::mat4 rotation    = glm::toMat4(glm::normalize(glm::quat(glm::radians(r))));
        glm::mat4 scale       = glm::scale(s);
        glm::mat4 m2w         = translation * rotation * scale;
        add_mesh(path, m2w, material_index);
    }

    void scene::add_mesh(char const* path, glm::vec3 p, glm::vec3 r, glm::vec3 s, material const& material)
    {
        add_material(material);
        add_mesh(path, p, r, s, m_materials.size() - 1);
    }

    void scene::add_cubemap(const char* dir)
    {
        for (auto& t : m_cubemap) {
            t.destroy();
        }

        int i = 0;
        m_cubemap[i++].load((std::string(dir) + "posx.png").c_str());
        m_cubemap[i++].load((std::string(dir) + "negx.png").c_str());
        m_cubemap[i++].load((std::string(dir) + "posy.png").c_str());
        m_cubemap[i++].load((std::string(dir) + "negy.png").c_str());
        m_cubemap[i++].load((std::string(dir) + "posz.png").c_str());
        m_cubemap[i++].load((std::string(dir) + "negz.png").c_str());
    }

    void scene::build_kdtree(cs350::kdtree::config config)
    {
        m_kdtree.build(m_triangles, config);
    }

    scene_intersection scene::get_closest_bf(ray const& r, raytracing_stats& stats) const
    {
        stats.queries++;
        // Get closest intersection
        scene_intersection result{};
        result.intersection_time = std::numeric_limits<float>::max();
        for (auto const& t : m_triangles) {
            stats.intersection_tests++;
            float intersection = intersection_ray_triangle(r, t.geometry);
            if (intersection >= 0.0f) {
                if (intersection < result.intersection_time) {
                    result.triangle          = &t;
                    result.intersection_time = intersection;
                }
                stats.positive_tests++;
            } else {
                stats.negative_tests++;
            }
        }
        return result;
    }

    scene_intersection scene::get_closest_kdtree(ray const& r, raytracing_stats& stats) const
    {
        stats.queries++;

        // Get closest intersection
        kdtree::debug_stats kdstats      = {};
        auto                intersection = m_kdtree.get_closest(r, &kdstats);

        stats.intersection_tests += kdstats.intersection_queries;
        stats.positive_tests += kdstats.intersection_positive_queries;

        // Convert
        scene_intersection result{};
        result.triangle          = &m_triangles[intersection.triangle_index];
        result.intersection_time = intersection.t;

        return result;
    }
}