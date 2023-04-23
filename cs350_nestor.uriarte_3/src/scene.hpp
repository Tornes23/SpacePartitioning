#pragma once

#include <vector>
#include "camera.hpp"
#include "kdtree.hpp"
#include "texture.hpp"
#include "scene_data.hpp"

namespace cs350 {
    struct raytracing_stats;

    class scene
    {
      private:
        std::vector<scene_triangle> m_triangles{};
        std::vector<material>       m_materials;
        std::vector<scene_light>    m_lights;
        cs350::kdtree               m_kdtree;
        std::array<texture, 6>      m_cubemap;
        material                    m_air_material{};

      public:
        scene();
        // Disallow copy assign
        scene(const scene&) = delete;
        void operator=(const scene&) = delete;

        void add_material(material mat);
        void add_light(scene_light light);
        void add_mesh(char const* path, glm::mat4 const& m2w, unsigned material_index);
        void add_mesh(char const* path, glm::vec3 p, glm::vec3 r, glm::vec3 s, unsigned material_index);
        void add_mesh(char const* path, glm::vec3 p, glm::vec3 r, glm::vec3 s, material const& material);
        void add_cubemap(char const* dir);
        void build_kdtree(cs350::kdtree::config config);

        scene_intersection get_closest_bf(ray const& r, raytracing_stats& stats) const;
        scene_intersection get_closest_kdtree(ray const& r, raytracing_stats& stats) const;

        void set_air_material(decltype(m_air_material) const& m) { m_air_material = m; }

        [[nodiscard]] decltype(m_triangles) const&    triangles() const noexcept { return m_triangles; }
        [[nodiscard]] decltype(m_triangles)&          triangles() noexcept { return m_triangles; }
        [[nodiscard]] decltype(m_materials) const&    materials() const noexcept { return m_materials; }
        [[nodiscard]] decltype(m_materials)&          materials() noexcept { return m_materials; }
        [[nodiscard]] decltype(m_lights)&             lights() noexcept { return m_lights; }
        [[nodiscard]] decltype(m_lights) const&       lights() const noexcept { return m_lights; }
        [[nodiscard]] decltype(m_air_material) const& air_material() const noexcept { return m_air_material; }
        [[nodiscard]] decltype(m_kdtree) const&       kdtree() const noexcept { return m_kdtree; }
        [[nodiscard]] decltype(m_kdtree)&             kdtree() noexcept { return m_kdtree; }
        [[nodiscard]] decltype(m_cubemap) const&      cubemap() const noexcept { return m_cubemap; }
    };
}