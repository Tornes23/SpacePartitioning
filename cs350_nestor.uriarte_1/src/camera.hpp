/*!**************************************************************************
\file    camera.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 12:00:15 2020

\brief	 This file containsm the implementation of the
Camera Class

The functions included are:
- void camera::create(glm::vec3 direction, glm::ivec2 viewport);
- void camera::update();
- void camera::set_position(glm::vec3 pos);
- void camera::set_target(glm::vec3 targ);
- void camera::set_projection(float fov, glm::ivec2 view, float near, float far);
- void camera::move(float delta, directions dir);
- void camera::updateVectors();
- void camera::rotate(float xoffset, float yoffset);
- glm::mat4x4 camera::view();
- glm::mat4x4 camera::projection();

***************************************************************************/

#pragma once

#include "math.hpp"
namespace cs350 {
    class camera
    {
    public:

        enum class directions
        {
            Forward,
            Backward,
            Left,
            Right

        };

        camera();

        void create(glm::vec3 direction = glm::vec3(0, 0, -1), glm::ivec2 viewport = glm::ivec2(1280, 720));

        void update();
        void set_position(glm::vec3 pos);
        void set_speed(float speed);
        void set_target(glm::vec3 targ);
        void set_projection(float fov, glm::ivec2 view, float near, float far);
        void move(float delta, directions dir);

        void updateVectors();
        void rotate(float xoffset, float yoffset);

        glm::mat4x4 view();
        glm::mat4x4 projection();

    private:
        glm::vec3 mPosition;//camera position
        glm::vec3 mView;//view vector
        glm::vec3 mUp;//up vector
        glm::vec3 mRightVector;//right vector

        //the needed matrices
        glm::mat4x4 mPerspective;
        glm::mat4x4 mCameraMatrix;

        glm::ivec2 mViewport;

        // camera options
        float mNear;
        float mFar;
        float mFOV;
        float mSpeed;
        float MouseSensitivity;

        //rotations
        float mYaw;
        float mPitch;

    };
}