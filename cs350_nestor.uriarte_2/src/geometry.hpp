/**
* @file geometry.h
* @author Nestor Uriarte, 54000817, nestor.uriarte@digipen.edu
* @date 2020/10/05
* @brief The definition for the basic geometrical
*		calculus for the primitives
*
* This file serves as an example of a documented file .
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once
#include "math.hpp"
#include "mesh.hpp"

namespace cs350 {

    enum class classification_t
    {
        inside,
        outside,
        overlapping
        
    };

    std::ostream& operator<<(std::ostream& os, const  classification_t& c);
    std::istream& operator>>(std::istream& is, classification_t& c);

    struct segment
    {
        //constructor
        segment(glm::vec3 p0 = glm::vec3(0), glm::vec3 p1 = glm::vec3(0));

        std::shared_ptr<mesh> mMesh;

        glm::vec3 mP0;
        glm::vec3 mP1;

        const glm::vec3& operator[](int i) const;

        friend std::ostream& operator<<(std::ostream& os, const  segment& s);
        friend std::istream& operator>>(std::istream& is, segment& s);

    };

    segment closest_segment_segment(const segment& s0, const segment& s1);

    struct plane
    {
        //constructor
        plane(glm::vec3 pos = glm::vec3(0), glm::vec3 norm = glm::vec3(0));

        //the mesh
        std::shared_ptr<mesh> mMesh;

        //necessary data
        glm::vec3 mPosition;
        glm::vec3 mNormal;
        glm::vec4 mColor;

        friend std::ostream& operator<<(std::ostream& os, const plane& p);
        friend std::istream& operator>>(std::istream& is, plane& p);

    };

    glm::vec3 closest_point_plane(const glm::vec3& p, const plane& plane);
    classification_t classify_plane_point(const plane& plane, const glm::vec3& p, const float thickness);

    struct triangle
    {
        //constructor
        triangle(glm::vec3 v0 = glm::vec3(0), glm::vec3 v1 = glm::vec3(0), glm::vec3 v2 = glm::vec3(0));

        //the mesh
        std::shared_ptr<mesh> mMesh;

        //necessary data
        glm::vec3 mV0;
        glm::vec3 mV1;
        glm::vec3 mV2;

        const glm::vec3& operator[](int i) const;

        friend std::ostream& operator<<(std::ostream& os, const triangle& t);
        friend std::istream& operator>>(std::istream& is, triangle& t);

    };

    classification_t classify_plane_triangle(const plane& plane, const triangle& t, const float thickness);

    struct aabb
    {
        //constructor
        aabb(glm::vec3 min = glm::vec3(0), glm::vec3 max = glm::vec3(0));
        aabb(glm::vec3 pos, float scale);

        //the mesh
        std::shared_ptr<mesh> mMesh;

        friend std::ostream& operator<<(std::ostream& os, const aabb& a);
        friend std::istream& operator>>(std::istream& is, aabb& a);

        //necessary data
        glm::vec3 mMin;
        glm::vec3 mMax;
    };

    bool intersection_point_aabb(const glm::vec3& p, const aabb& b);
    bool intersection_aabb_aabb(const aabb& a, const aabb& b);
    classification_t classify_plane_aabb(const plane& plane, const aabb& a, const float thickness);

    struct sphere
    {
        //constructor
        sphere(glm::vec3 pos = glm::vec3(0), float radius = 1.0F);

        //the mesh
        std::shared_ptr<mesh> mMesh;

        //necessary data
        glm::vec3 mPosition;
        float mRadius;

        friend std::ostream& operator<<(std::ostream& os, const sphere& s);
        friend std::istream& operator>>(std::istream& is, sphere& s);

    };

    bool intersection_point_sphere(const glm::vec3& p, const sphere& s);
    bool intersection_sphere_sphere(const sphere& a, const sphere& b);
    classification_t classify_plane_sphere(const plane& plane, const sphere& s, const float thickness);

    struct ray
    {
        ray(glm::vec3& p = glm::vec3(0), glm::vec3 v = glm::vec3(0));

        friend std::ostream& operator<<(std::ostream& os, const  ray& r);
        friend std::istream& operator>>(std::istream& is, ray& r);

        glm::vec3 mP;
        glm::vec3 mVec;
    };

    float intersection_ray_plane(const ray& r, const plane& p);
    float intersection_ray_aabb(const ray& r, const aabb& a);
    float intersection_ray_sphere(const ray& r, const sphere& s);
    float intersection_ray_triangle(const ray& r, const triangle& t);

    struct frustum
    {

        frustum(plane* planes = nullptr);

        //the mesh
        std::shared_ptr<mesh> mMesh;

        //the six planes that form the frustum
        plane mPlanes[6] = {};

        friend std::ostream& operator<<(std::ostream& os, const frustum& f);
        friend std::istream& operator>>(std::istream& is, frustum& f);

    };

    classification_t classify_frustum_sphere_naive(const frustum& frustum, const sphere& a);
    classification_t classify_frustum_aabb_naive(const frustum& frustum, const aabb& a);

}