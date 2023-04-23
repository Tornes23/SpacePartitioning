#include <chrono>
#include <fstream>
#include "common.hpp"
#include "scene.hpp"
#include "raytracer.hpp"
#include "texture.hpp"
#include "kdtree.hpp"

#define TEST_WORKDIR "./"

using namespace cs350;

namespace {
    const float panel_scale    = 5;
    const char* c_mesh_bunny   = "resources/bunny.obj";
    const char* c_mesh_suzanne = "resources/suzanne.obj";
    const char* c_mesh_dragon  = "resources/dragon.obj";
    const char* c_mesh_cube    = "resources/cube.obj";
    const char* c_mesh_gourd   = "resources/gourd.obj";

    /**
	 * Gets the filename for the kdtree to print
	 * @return
	 */
    std::string current_test_plot_filename()
    {
        std::string const& suite_name = ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();        
        std::string const& test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();        
        return std::string(TEST_WORKDIR "out/") + suite_name + "_" + test_name + "kdtree.png";
    }

    /**
	 * Gets the filename for the image to print
	 * @return
	 */
    std::string current_test_filename()
    {
        std::string const& suite_name = ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();        
        std::string const& test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();        
        return std::string(TEST_WORKDIR "out/") + suite_name + "_" + test_name + ".png";
    }

    /**
     * Saves the graph of the current test
     * @param kd 
     */
    void save_graph(kdtree const& kd)
    {
        //auto in  = current_test_plot_filename() + ".dot";
        //auto out = current_test_plot_filename();

        //std::ofstream graph_file(in);
        //kd.dump_graph(graph_file);
        //auto sh = std::string("dot -Tpng ") + in + " -o " + out;
        //std::cerr << sh << std::endl;
        //graph_file.close();
        //system(sh.c_str());
    }

    /**
     * 
     * @param t 
     */
    void save_image(texture const& t) 
    {
        t.save(current_test_filename().c_str());
    }

    void load_box(scene& scene)
    {
        cs350::material material{};

        // Planes
        material                     = {};
        material.specular_reflection = 0.09f;
        material.specular_exponent   = 300;
        material.attenuation         = {0.9, 0.9, 0.9};
        material.electric_perm       = 0.5;
        material.magnetic_perm       = 0.0;
        material.attenuation         = {1, 1, 1};

        material.diffuse = {0.8, 0, 0};
        scene.add_mesh(TEST_WORKDIR "resources/quad.obj", {-panel_scale, 0, 0}, {0, -90, 0}, glm::vec3{panel_scale}, material);
        scene.add_mesh(TEST_WORKDIR "resources/quad.obj", {panel_scale, 0, 0}, {0, 90, 0}, glm::vec3{panel_scale}, material);

        material.diffuse = {0, 0.8, 0};
        scene.add_mesh(TEST_WORKDIR "resources/quad.obj", {0, panel_scale, 0}, {-90, 0, 0}, glm::vec3{panel_scale}, material);
        scene.add_mesh(TEST_WORKDIR "resources/quad.obj", {0, -panel_scale, 0}, {-90, 0, 0}, glm::vec3{panel_scale}, material);

        material.diffuse = {0.8, 0.8, 0.8};
        scene.add_mesh(TEST_WORKDIR "resources/quad.obj", {0, 0, panel_scale}, {0, 0, 0}, glm::vec3{panel_scale}, material);
        scene.add_mesh(TEST_WORKDIR "resources/quad.obj", {0, 0, -panel_scale}, {0, 0, 0}, glm::vec3{panel_scale}, material);

        { // Lights
            scene.add_light({{2.45, 2.4, 2.5}, {0.4, 0.4, 0.4}, 0.1f});
            scene.add_light({{-2.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});
        }
    }

    /**
     * 
     * @param scene 
     */
    void set_material_per_node(scene& scene)
    {
        // Per node
        auto& kdtree = scene.kdtree();
        for (size_t j = 0; j < kdtree.nodes().size(); ++j) {
            auto& node = kdtree.nodes()[j];
            auto& aabb = kdtree.aabbs()[j];
            if (node.is_leaf()) {
                auto color = glm::linearRand(glm::vec3{0.01, 0.01, 0.01}, glm::vec3(1, 1, 1));
                auto hsv   = glm::hsvColor(color);
                hsv.z      = 3.0f;
                color      = glm::rgbColor(hsv);
                for (size_t i = node.primitive_start(); i < node.primitive_start() + node.primitive_count(); ++i) {
                    size_t index      = kdtree.indices()[i];
                    auto&  kdtriangle = kdtree.triangles().at(index);
                    auto&  triangle   = scene.triangles().at(kdtriangle.original_index);
                    auto&  geometry   = triangle.geometry;
                    // Random color
                    triangle.material.diffuse             = color;
                    triangle.material.specular_reflection = 0.01f;
                    triangle.material.specular_exponent   = 0;
                    triangle.material.magnetic_perm       = 0;
                    triangle.material.electric_perm       = 0;
                    triangle.material.compute_n();
                }
            }
        }
    }

    /**
     *
     * @param scene
     */
    void set_material_per_triangle(scene& scene)
    {
        // Recreates triangles and materials
        scene.triangles().clear();
        scene.materials().clear();

        auto& kdtree = scene.kdtree();
        for (size_t j = 0; j < kdtree.nodes().size(); ++j) {
            auto& node = kdtree.nodes()[j];
            auto& aabb = kdtree.aabbs()[j];
            if (node.is_leaf()) {
                for (size_t i = node.primitive_start(); i < node.primitive_start() + node.primitive_count(); ++i) {
                    // Material per triangle
                    auto color = glm::linearRand(glm::vec3{0.01, 0.01, 0.01}, glm::vec3(1, 1, 1));
                    auto hsv   = glm::hsvColor(color);
                    hsv.z      = 3.0f;
                    color      = glm::rgbColor(hsv);

                    size_t index      = kdtree.indices()[i];
                    auto&  kdtriangle = kdtree.triangles().at(index);

                    kdtriangle.original_index = scene.triangles().size();

                    scene_triangle sceneTriangle{};
                    // Random color
                    sceneTriangle.material.diffuse             = color;
                    sceneTriangle.material.specular_reflection = 0.01f;
                    sceneTriangle.material.specular_exponent   = 0;
                    sceneTriangle.material.magnetic_perm       = 0;
                    sceneTriangle.material.electric_perm       = 0;
                    sceneTriangle.material.compute_n();
                    sceneTriangle.geometry = kdtriangle.tri;
                    scene.triangles().push_back(sceneTriangle);
                }
            }
        }
    }

    camera camera_default()
    {
        // Camera
        camera camera{};
        camera.center = {0, 0, -3};
        camera.view   = glm::normalize(glm::vec3{0, 0, 1});
        camera.up     = {0, 1, 0};
        return camera;
    }

    material material_default()
    {
        material material            = {};
        material.diffuse             = {0.8, 0.8, 0.8};
        material.specular_reflection = 1;
        material.specular_exponent   = 0;
        material.attenuation         = {};
        material.electric_perm       = 0.0;
        material.magnetic_perm       = 0.0;
        return material;
    }

    material material_fancy()
    {
        material material            = {};
        material.diffuse             = {0.8, 0.8, 0.8};
        material.specular_reflection = 1.5;
        material.specular_exponent   = 10;
        material.attenuation         = {0.5, 0.5, 0.5};
        material.electric_perm       = 10.5;
        material.magnetic_perm       = 0.5;
        return material;
    }

    raytracing_config ray_config_diffuse_only()
    {
        cs350::raytracing_config config;
        config.recursion          = 1;
        config.reflection_samples = 1;
        config.shadow_samples     = 1;
        config.stats              = {};
        config.use_kdtree         = true;
        config.show_progress      = false;
        return config;
    }

    raytracing_config ray_config_fancy()
    {
        cs350::raytracing_config config;
        config.recursion            = 4;
        config.reflection_samples   = 3;
        config.reflection_fuzziness = 0.005f;
        config.shadow_samples       = 4;
        config.stats                = {};
        config.use_kdtree           = true;
        config.show_progress        = true;

        //
        config.shadow_bias            = 1e-2f;
        config.refl_bias              = 1e-2f;
        config.transmission_threshold = 1e-2f;
        return config;
    }

    texture render_custom(scene& scene, int w, int h, cs350::camera const& camera, raytracing_config config)
    {
        // Output
        texture output;
        output.resize(w, h);

        { // KdTree build
            cs350::kdtree::config kdconfig{};
            kdconfig.cost_intersection = 80;
            kdconfig.cost_traversal    = 1;
            kdconfig.max_depth         = 30;

            auto t1 = std::chrono::high_resolution_clock::now();
            scene.build_kdtree(kdconfig);
            auto t2                               = std::chrono::high_resolution_clock::now();
            auto int_ms                           = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            config.stats.kdtree_build_duration_ms = int_ms.count();
        }

        // Render
        auto& stats = config.stats;
        std::cout << std::setw(20) << "kdtree build: " << stats.kdtree_build_duration_ms << "ms" << std::endl;
        cs350::raytrace(&output, scene, camera, config);
        std::cout << std::setw(20) << "triangles: " << scene.triangles().size() << std::endl;
        std::cout << std::setw(20) << "intersection_tests: " << stats.intersection_tests << std::endl;
        std::cout << std::setw(20) << "queries: " << stats.queries << std::endl;
        std::cout << std::setw(20) << "positive_tests: " << stats.positive_tests << std::endl;
        std::cout << std::setw(20) << "negative_tests: " << stats.negative_tests << std::endl;
        std::cout << std::setw(20) << "render duration: " << stats.duration_ms << "ms" << std::endl;
        std::cout << std::setw(20) << "kdtree build: " << stats.kdtree_build_duration_ms << "ms" << std::endl;
        std::cout << std::setw(20) << "kdtree depth: " << scene.kdtree().get_depth() << std::endl;
        std::cout << std::setw(20) << "kdtree triangles: " << scene.kdtree().triangles().size() << std::endl;

        return output;
    }

    texture render_standard(scene& scene, cs350::camera const& camera)
    {
        return render_custom(scene, 128, 128, camera, ray_config_diffuse_only());
    }

    texture render_fancy(scene& scene, cs350::camera const& camera)
    {
        return render_custom(scene, 640, 640, camera, ray_config_fancy());
    }

    texture render_debug(scene& scene, cs350::camera const& camera)
    {
        // Output
        texture output;
        output.resize(1920, 1080);

        cs350::raytracing_config config;
        config.recursion          = 1;
        config.reflection_samples = 0;
        config.shadow_samples     = 0;
        config.stats              = {};
        config.use_kdtree         = true;
        config.show_progress      = false;

        auto& stats = config.stats;
        { // KDtree
            cs350::kdtree::config kdconfig;
            kdconfig.cost_intersection = 80;
            kdconfig.cost_traversal    = 1;
            kdconfig.max_depth         = 30; //[INSTRUCTOR] Play with this number!

            auto t1 = std::chrono::high_resolution_clock::now();
            scene.build_kdtree(kdconfig);
            auto t2 = std::chrono::high_resolution_clock::now();

            auto int_ms                    = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            stats.kdtree_build_duration_ms = int_ms.count();
        }

        // Render
        std::cout << std::setw(20) << "kdtree build: " << stats.kdtree_build_duration_ms << "ms" << std::endl;
        std::cout << std::setw(20) << "triangles: " << scene.triangles().size() << std::endl;        
        //scene.kdtree().dump(std::cerr);

        set_material_per_node(scene); // DEBUG
        //set_material_per_triangle(scene); // DEBUG
        cs350::raytrace(&output, scene, camera, config);
        std::cout << std::setw(20) << "intersection_tests: " << stats.intersection_tests << std::endl;
        std::cout << std::setw(20) << "queries: " << stats.queries << std::endl;
        std::cout << std::setw(20) << "positive_tests: " << stats.positive_tests << std::endl;
        std::cout << std::setw(20) << "negative_tests: " << stats.negative_tests << std::endl;
        std::cout << std::setw(20) << "render duration: " << stats.duration_ms << "ms" << std::endl;
        std::cout << std::setw(20) << "kdtree build: " << stats.kdtree_build_duration_ms << "ms" << std::endl;
        std::cout << std::setw(20) << "kdtree depth: " << scene.kdtree().get_depth() << std::endl;
        std::cout << std::setw(20) << "kdtree triangles: " << scene.kdtree().triangles().size() << std::endl;
        return output;
    }
}

TEST(raytrace_debug, dragon_only)
{
    cs350::scene scene;

    // Lights
    scene.add_light({{2.45, 2.4, 2.5}, {0.4, 0.4, 0.4}, 0.1f});
    scene.add_light({{-2.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});
    // Scene
    scene.add_mesh(c_mesh_dragon, {0, 0, -1}, {0, 90, 0}, glm::vec3{3}, material_default());

    auto output = render_debug(scene, camera_default());
    save_image(output);
}

TEST(raytrace_debug, suzanne_only)
{
    cs350::scene scene;

    // Lights
    scene.add_light({{20.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});
    scene.add_light({{-20.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});

    // Scene
    scene.add_mesh(c_mesh_suzanne, {0, 0, -1}, {0, 180, 0}, glm::vec3{1}, material_default());
    auto c = camera_default();

    c.center    = {-0.5, 1, -3}; //[INSTRUCTOR] Play with this
    c.view      = glm::normalize(-c.center);
    auto output = render_debug(scene, c);
    save_image(output);
    save_graph(scene.kdtree());
}

TEST(raytrace_debug, bunny_only)
{
    cs350::scene scene;
    // Lights
    scene.add_light({{2.45, 2.4, -1.5}, {0.4, 0.4, 0.4}, 0.1f});
    scene.add_light({{-2.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});
    // Scene
    scene.add_mesh(c_mesh_bunny, {0, -1, -2}, {0, 180, 0}, glm::vec3{10}, material_default());
    auto output = render_debug(scene, camera_default());
    save_image(output);
    save_graph(scene.kdtree());
}

TEST(raytrace_debug, cornell_suzanne)
{
    cs350::scene scene;
    load_box(scene);
    scene.lights().clear();
    scene.add_light({{-0.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});
    scene.add_mesh(c_mesh_suzanne, {panel_scale * 0.5, -panel_scale + 1, panel_scale * 0.5}, {0, 30, 0}, glm::vec3{1}, material_default());
    auto output = render_debug(scene, camera_default());
    save_image(output);
    save_graph(scene.kdtree());
}

TEST(raytrace_fast, cornell_empty)
{
    cs350::scene scene;
    load_box(scene);

    auto output = render_standard(scene, camera_default());
    save_image(output);
}

TEST(raytrace_fast, cornell_suzanne)
{
    cs350::scene scene;
    load_box(scene);
    scene.add_mesh(c_mesh_suzanne, {panel_scale * 0.5, -panel_scale + 1, panel_scale * 0.5}, {0, 30, 0}, glm::vec3{1}, material_default());
    auto output = render_standard(scene, camera_default());
    save_image(output);
}

TEST(raytrace_fast, cornell_bunny)
{
    cs350::scene scene;
    load_box(scene);
    scene.add_mesh(c_mesh_bunny, {panel_scale * 0.5, -panel_scale + 1, panel_scale * 0.5}, {0, 30, 0}, glm::vec3{5}, material_default());
    auto output = render_standard(scene, camera_default());
    save_image(output);
}

TEST(raytrace_fast, dragon_only)
{
    cs350::scene scene;

    // Lights
    scene.add_light({{2.45, 2.4, 2.5}, {0.4, 0.4, 0.4}, 0.1f});
    scene.add_light({{-2.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});
    // Scene
    scene.add_mesh(c_mesh_dragon, {0, 0, -1}, {0, 90, 0}, glm::vec3{3}, material_default());

    auto output = render_standard(scene, camera_default());
    save_image(output);
}

TEST(raytrace_fast, suzanne_only)
{
    cs350::scene scene;

    // Lights
    scene.add_light({{2.45, 2.4, 2.5}, {0.4, 0.4, 0.4}, 0.1f});
    scene.add_light({{-2.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});
    // Scene
    scene.add_mesh(c_mesh_suzanne, {0, 0, -1}, {0, 180, 0}, glm::vec3{1}, material_default());

    auto output = render_standard(scene, camera_default());
    save_image(output);
}

TEST(raytrace_fast, bunny_only)
{
    cs350::scene scene;

    // Lights
    scene.add_light({{2.45, 2.4, 2.5}, {0.4, 0.4, 0.4}, 0.1f});
    scene.add_light({{-2.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});

    // Scene
    scene.add_mesh(c_mesh_bunny, {0, -1, -1}, {0, 180, 0}, glm::vec3{10}, material_default());

    auto output = render_standard(scene, camera_default());
    save_image(output);
}

TEST(raytrace_fancy, cornell_boxes)
{
    cs350::scene scene;
    load_box(scene);
    scene.add_mesh(c_mesh_cube, {-panel_scale * 0.5, -panel_scale + 1, panel_scale * 0.7}, {0, 30, 0}, glm::vec3{2}, material_fancy());
    scene.add_mesh(c_mesh_cube, {panel_scale * 0.5, -panel_scale + 1, panel_scale * 0.5}, {0, 30, 0}, glm::vec3{2}, material_fancy());

    // Cubemap
    scene.add_cubemap(TEST_WORKDIR "resources/PalmTrees/");

    auto output = render_fancy(scene, camera_default());
    save_image(output);
}

TEST(raytrace_fancy, dragon)
{
    cs350::scene scene;

    // Lights
    scene.add_light({{2.45, 2.4, 2.5}, {0.4, 0.4, 0.4}, 0.1f});
    scene.add_light({{-2.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});

    { // Air
        material material      = {};
        material.electric_perm = 1.;
        material.magnetic_perm = 1.;
        material.attenuation   = {1, 1, 1};
        material.compute_n();
        scene.set_air_material(material);
    }

    // Scene
    auto m                = material_fancy();
    m.diffuse             = {0.8, 0.8, 0.8};
    m.specular_reflection = 1.5;
    m.specular_exponent   = 10;
    m.attenuation         = {0.0, 0.0, 0.0};
    m.electric_perm       = 10.5f;
    m.magnetic_perm       = 1.56f;

    // Scene
    scene.add_mesh(c_mesh_dragon, {0, 0, -1}, {0, 90, 0}, glm::vec3{3}, m);

    // Camera
    cs350::camera camera{};
    camera.center = {0, 0, -3};
    camera.view   = glm::normalize(glm::vec3{0, 0, 1});
    camera.up     = {0, 1, 0};

    // Cubemap
    scene.add_cubemap(TEST_WORKDIR "resources/PalmTrees/");

    auto output = render_fancy(scene, camera);
    save_image(output);
}

TEST(raytrace_fancy, suzanne)
{
    cs350::scene scene;

    // Lights
    scene.add_light({glm::vec3{-2.45, 2.4, -2.5}, glm::vec3{0.4, 0.4, 0.4}, 0.1f});
    scene.add_light({glm::vec3{1.45, 2.4, -2.5}, glm::vec3{0.2, 0.2, 0.2}, 0.1f});

    { // Air
        material material      = {};
        material.electric_perm = 1.15f;
        material.magnetic_perm = 1.1f;
        material.attenuation   = {0.79, 0.79, 0.79};
        material.compute_n();
        scene.set_air_material(material);
    }

    // Scene
    auto m                = material_fancy();
    m.diffuse             = {0.8, 0.8, 0.8};
    m.specular_reflection = 1.5;
    m.specular_exponent   = 10;
    m.attenuation         = {0.75, 0.75, 0.75};
    m.electric_perm       = 0.15f;
    m.magnetic_perm       = 0.95f;

    // Scene
    scene.add_mesh(c_mesh_suzanne, {0, 0, -1}, {0, 180, 0}, glm::vec3{1}, m);
    // Camera
    cs350::camera camera{};
    camera.center = {0, 0, -3};
    camera.view   = glm::normalize(glm::vec3{0, 0, 1});
    camera.up     = {0, 1, 0};

    // Cubemap
    scene.add_cubemap(TEST_WORKDIR "resources/PalmTrees/");

    auto output = render_fancy(scene, camera);
    save_image(output);
}

//TEST(raytrace_fancy, dragon_chapel)
//{
//    cs350::scene scene;
//
//    // Lights
//    scene.add_light({{2.45, 2.4, 2.5}, {0.4, 0.4, 0.4}, 0.1f});
//    scene.add_light({{-2.45, 2.4, -2.5}, {0.4, 0.4, 0.4}, 0.1f});
//
//    { // Air
//        material material      = {};
//        material.electric_perm = 0.225f;
//        material.magnetic_perm = 1.4f;
//        material.attenuation   = {0.79, 0.79, 0.79};
//        material.compute_n();
//        scene.set_air_material(material);
//    }
//
//    // Scene
//    auto m                = material_fancy();
//    m.diffuse             = {0.8, 0.8, 0.8};
//    m.specular_reflection = 1.75;
//    m.specular_exponent   = 5;
//    m.attenuation         = {0.75, 0.75, 0.75};
//    m.electric_perm       = 0.4f;
//    m.magnetic_perm       = 0.7f;
//    scene.add_mesh(c_mesh_dragon, {0, -1, -1}, {0, 90, 0}, glm::vec3{3}, m);
//
//    // Camera
//    cs350::camera camera{};
//    camera.center = {0, 0, -3};
//    camera.view   = glm::normalize(glm::vec3{0, 0, 1});
//    camera.up     = {0, 1, 0};
//
//    // Cubemap
//    scene.add_cubemap(TEST_WORKDIR "resources/LancellottiChapel/");
//
//    auto ray_conf      = ray_config_fancy();
//    ray_conf.recursion = 5;
//    auto output        = render_custom(scene, 1920, 1080, camera, ray_conf);
//    //auto output = render_fancy(scene, camera);
//    save_image(output);
//}
