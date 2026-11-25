// Unit tests for the pure geometry functions.
// They are DISABLED_ until you implement the functions: remove the DISABLED_
// prefix as you go, and add your own cases (edge cases are what recruiters look at).
// Run: colcon test --packages-select semantic_nav_mapper && colcon test-result --verbose

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

#include "semantic_nav_mapper/geometry.hpp"

using semantic_nav::Point2D;

TEST(Geometry, DISABLED_PixelOnOpticalAxisHasZeroBearing)
{
  EXPECT_NEAR(semantic_nav::pixelToBearing(320.0, 500.0, 320.0), 0.0, 1e-9);
}

TEST(Geometry, DISABLED_PixelOnTheRightHasNegativeBearing)
{
  EXPECT_LT(semantic_nav::pixelToBearing(600.0, 500.0, 320.0), 0.0);
}

TEST(Geometry, DISABLED_RangeAtBearingRejectsInf)
{
  std::vector<float> ranges(360, std::numeric_limits<float>::infinity());
  EXPECT_LT(semantic_nav::rangeAtBearing(ranges, 0.0, M_PI / 180.0, 0.0, 0.12, 3.5), 0.0);
}

TEST(Geometry, DISABLED_GoalIsAtStandoffAndFacesObject)
{
  auto g = semantic_nav::goalNearObject(Point2D{2.0, 0.0}, Point2D{0.0, 0.0}, 0.5);
  EXPECT_NEAR(g.x, 1.5, 1e-6);
  EXPECT_NEAR(g.y, 0.0, 1e-6);
  EXPECT_NEAR(g.yaw, 0.0, 1e-6);
}

TEST(Geometry, BuildSmokeTest)
{
  SUCCEED();
}
