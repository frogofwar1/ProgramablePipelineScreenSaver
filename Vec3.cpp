#include "pch.h"
#include "Vec3.h"


namespace cw
{
  Vec3::Vec3()
  {
  }

  Vec3::Vec3(float x, float y, float z):
    glm::vec3(x, y, z)
  {
  }


  Vec3::~Vec3()
  {
  }

}

