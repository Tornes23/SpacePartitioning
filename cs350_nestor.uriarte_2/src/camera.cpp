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
- void camera::camera();
- void camera::create(glm::vec3 direction, glm::ivec2 viewport);
- void camera::update();
- void camera::set_position(glm::vec3 pos);
- void camera::set_target(glm::vec3 targ);
- void camera::set_speed(float speed);
- void camera::set_projection(float fov, glm::ivec2 view, float near, float far);
- void camera::move(float delta, directions dir);
- void camera::updateVectors();
- void camera::rotate(float xoffset, float yoffset);
- glm::mat4x4 camera::view();
- glm::mat4x4 camera::projection();

***************************************************************************/


#include "pch.hpp"
#include "imgui.hpp"
#include "camera.hpp"

namespace cs350 {

	/**************************************************************************
	*!
	\fn     camera::camera

	\brief
	Constructor of the class

	*
	**************************************************************************/
	camera::camera() {}

	/**************************************************************************
	*!
	\fn     camera::create

	\brief
	Creates the camera

	\param  glm::vec3 target
	the target pos

	\param  glm::ivec2 viewport
	the viewport size

	*
	**************************************************************************/
	void camera::create(glm::vec3 target, glm::ivec2 viewport)
	{
		mPosition = glm::vec3(0, 0, 5);

		//setting the view vector
		mView = glm::normalize(target - mPosition);

		//setting the up vector
		mUp = glm::vec3(0, 1, 0);

		//computing the right vector
		mRightVector = glm::normalize(glm::cross(mView, mUp));

		//initializing all the values
		mNear = 0.1F;
		mFar = 500.0F;

		mFOV = 60.0F;
		mSpeed = 2.5F;

		mYaw = -90.0F;
		mPitch = 0.0F;

		//setting the viewport
		mViewport = viewport;

		//creating the camera and perspective matrices
		mCameraMatrix = glm::lookAt(mPosition, mPosition + mView, mUp);
		mPerspective = glm::perspective(glm::radians(mFOV), static_cast<float>(mViewport.x) / static_cast<float>(mViewport.y), mNear, mFar);

	}

	/**************************************************************************
	*!
	\fn     camera::update

	\brief 
	the update function of the camera, 
	(does nothing because i update the matrices on the setter functions)

	*
	**************************************************************************/
	void camera::update()
	{
	}

	/**************************************************************************
	*!
	\fn     camera::set_position

	\brief 
	sets the position of the camera

	\param  glm::vec3 pos
	the position

	*
	**************************************************************************/
	void camera::set_position(glm::vec3 pos)
	{
		mPosition = pos;
	}

	/**************************************************************************
	*!
	\fn     camera::set_speed

	\brief
	sets the camnera matrix based on a target

	\param  float speed
	the speed to be set

	*
	**************************************************************************/
	void camera::set_speed(float speed)
	{
		mSpeed = speed;
	}
	/**************************************************************************
	*!
	\fn     camera::set_target

	\brief
	sets the camnera matrix based on a target

	\param  glm::vec3 targ
	the target

	*
	**************************************************************************/
	void camera::set_target(glm::vec3 targ)
	{
		mCameraMatrix = glm::lookAt(mPosition, targ, mUp);
	}

	/**************************************************************************
	*!
	\fn     camera::set_projection

	\brief 
	sets the projection matrix

	\param  float fov
	the field of viewe

	\param  glm::ivec2 view
	the view vector

	\param  float near
	the near value

	\param  float far
	the far value

	*
	**************************************************************************/
	void camera::set_projection(float fov, glm::ivec2 view, float near, float far)
	{
		//set the variables and create the matrix
		mFOV = fov;
		mViewport = view;
		mNear = near;
		mFar = far;

		mPerspective = glm::perspective(glm::radians(mFOV), static_cast<float>(mViewport.x) / static_cast<float>(mViewport.y), mNear, mFar);
	}

	/**************************************************************************
	*!
	\fn     camera::move

	\brief 
	moves the camera

	\param  float delta
	the delta time for the updates

	\param  directions dir
	the direction in which it moves

	*
	**************************************************************************/
	void camera::move(float delta, camera::directions dir)
	{
		//computing the speed
		float vel = mSpeed * delta;

		//depending on in which direction move modify the position
		switch (dir)
		{
		case cs350::camera::directions::Forward:
			mPosition += mView * vel;
			break;
		case cs350::camera::directions::Backward:
			mPosition -= mView * vel;
			break;
		case cs350::camera::directions::Left:
			mPosition -= mRightVector * vel;
			break;
		case cs350::camera::directions::Right:
			mPosition += mRightVector * vel;
			break;
		default:
			break;
		}

		//update the camera matrix
		mTarget = mPosition + mView;

		set_target(mTarget);

	}

	/**************************************************************************
	*!
	\fn     camera::updateVectors

	\brief 
	updates the view, up and right vectors of the camera

	*
	**************************************************************************/
	void camera::updateVectors()
	{
		glm::vec3 forward;

		//computing the new view vector based on rotations
		forward.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		forward.y = sin(glm::radians(mPitch));
		forward.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		//setting the vector
		mView = glm::normalize(forward);

		// also re-calculating the Right and Up vector
		mRightVector = glm::normalize(glm::cross(mView, {0,1,0}));  
		mUp = glm::normalize(glm::cross(mRightVector, mView));

		//updating the camera matrix
		set_target(mPosition + mView);
	}

	/**************************************************************************
	*!
	\fn     camera::rotate

	\brief 
	rotates the camera

	\param  float xoffset
	the offset of mouse movement on x axis

	\param  float yoffset
	

	*
	**************************************************************************/
	void camera::rotate(float xoffset, float yoffset)
	{
		//sentivity for the movement
		const float sentitivity = 0.1F;

		//computing the offset increment with the senitivity
		xoffset *= sentitivity;
		yoffset *= sentitivity;

		//modify the rotation values
		mYaw += xoffset;
		mPitch += yoffset;

		//rotations of less tha 90 degrees to avoid gimbal locks
		if (mPitch > 89.0f)
			mPitch = 89.0f;
		if (mPitch < -89.0f)
			mPitch = -89.0f;

		// update Front, Right and Up Vectors using the updated Euler angles
		updateVectors();
	}

	/**************************************************************************
	*!
	\fn     camera::view

	\brief 
	gets the camera matrix

	\return glm::mat4x4
	the matrix

	*
	**************************************************************************/
	glm::mat4x4 camera::view()
	{
		return mCameraMatrix;
	}

	/**************************************************************************
	*!
	\fn     camera::projection

	\brief 
	gets the projection matrix

	\return glm::mat4x4
	the matrix

	*
	**************************************************************************/
	glm::mat4x4 camera::projection()
	{
		return mPerspective;
	}

	/**************************************************************************
	*!
	\fn     camera::target

	\brief 
	gets the target

	\return glm::vec3
	the target

	*
	**************************************************************************/
	glm::vec3 camera::target() const
	{
		return mTarget;
	}

	/**************************************************************************
	*!
	\fn     camera::position

	\brief
	gets the position

	\return glm::vec3
	the position

	*
	**************************************************************************/
	glm::vec3 camera::position() const
	{
		return mPosition;
	}

	/**************************************************************************
	*!
	\fn     camera::viewVec

	\brief
	gets the view vector

	\return glm::vec3
	the view vector

	*
	**************************************************************************/
	glm::vec3 camera::viewVec() const
	{
		return mView;
	}
}
