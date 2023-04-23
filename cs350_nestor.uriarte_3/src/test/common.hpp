#pragma once

#include <gtest/gtest.h>
#include "geometry.hpp"

/**
 * 
 * @param lhs 
 * @param rhs 
 */
inline void ASSERT_VEC_EQ(glm::vec3 const& lhs, glm::vec3 const& rhs)
{
    ASSERT_FLOAT_EQ(lhs.x, rhs.x);
    ASSERT_FLOAT_EQ(lhs.y, rhs.y);
    ASSERT_FLOAT_EQ(lhs.z, rhs.z);
}
