#pragma once

#include "Thirdparty/glm/glm/vec3.hpp"
#include <iostream>

namespace cw
{
	class Vec3 : public glm::vec3
	{
	public:
		Vec3();
		Vec3(float x, float y, float z);
		~Vec3();

		void print() const;
	};

	std::ostream& operator<< (std::ostream& os, const Vec3& v);
}


