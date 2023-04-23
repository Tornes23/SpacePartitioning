#pragma once

namespace cs350 {
    class texture;
    struct material;
    class scene;
    class camera;

    struct raytracing_stats
    {
        unsigned queries{};
        unsigned intersection_tests{};
        unsigned positive_tests{};
        unsigned negative_tests{};
        unsigned duration_ms{};
        unsigned kdtree_build_duration_ms{};
    };

    struct raytracing_config
    {
        raytracing_stats stats;

        int   shadow_samples{};
        int   reflection_samples{};
        float reflection_fuzziness{};
        float shadow_bias            = 1e-2f;
        float refl_bias              = 1e-2f;
        float transmission_threshold = 1e-2f;
        int   recursion{};

        // Debug
        bool show_progress{};
        bool use_kdtree{};
    };

    void raytrace(texture* dst, scene const& scene, camera const& camera, raytracing_config& cfg);
}