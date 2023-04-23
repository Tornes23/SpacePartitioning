#include "pch.hpp"
#include "math.hpp"
#include "renderer.hpp"

namespace cs350 {
	
	struct GameObject
	{
		GameObject(meshType mesh = meshType::sphere, glm::vec3& pos = glm::vec3(0, 0, 0), glm::vec3& scale = glm::vec3(1,1,1));

		void render();
		void update();

		glm::mat4x4 model2world() const;

		bool Edit(int i);

		meshType mMesh;

		glm::vec3 mPosition;
		glm::vec3 mScale;
		glm::mat4x4 mModel2World;
		std::string mName;
	};
}