#pragma once

#include "Thirdparty/glm/glm/vec3.hpp"

namespace cw
{
  class Vec3: public glm::vec3
  {
  public:
    Vec3();
    Vec3(float x, float y, float z);
    virtual ~Vec3();

  };

}
