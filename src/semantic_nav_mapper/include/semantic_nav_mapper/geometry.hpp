// Pure functions used by the object mapper. No ROS types here on purpose,
// so you can test everything with gtest (see test/test_geometry.cpp).
#pragma once

#include <string>
#include <vector>

namespace semantic_nav
{

struct Point2D
{
  double x{0.0};
  double y{0.0};
};

struct Pose2D
{
  double x{0.0};
  double y{0.0};
  double yaw{0.0};
};

// One object kept in the semantic map (map frame).
struct TrackedObject
{
  unsigned int id{0};
  std::string label;
  Point2D position;
  double confidence{0.0};
  unsigned int observations{0};
};

// Horizontal angle (radians) of the ray through pixel column `u`, using the
// pinhole model. Positive = to the LEFT of the optical axis (ROS convention).
// fx and cx come from sensor_msgs/CameraInfo (K[0] and K[2]).
double pixelToBearing(double u, double fx, double cx);

// Range (metres) measured by a LaserScan at `bearing` (radians, in the scan frame).
// Return a negative value if the ray falls outside the scan or the reading is invalid.
double rangeAtBearing(
  const std::vector<float> & ranges, double angle_min, double angle_increment,
  double bearing, double range_min, double range_max);

// Index of the existing object with the same label closest to `p`, if it is
// closer than `max_dist`. Return -1 if none: the caller creates a new object.
int associate(
  const std::vector<TrackedObject> & objects, const std::string & label,
  const Point2D & p, double max_dist);

// Merge a new observation into an existing object (running mean of position
// and confidence, observations += 1).
void fuse(TrackedObject & obj, const Point2D & p, double confidence);

// A navigation goal `standoff` metres away from `object`, on the side of `robot`,
// with yaw pointing at the object. This is what Nav2 receives.
Pose2D goalNearObject(const Point2D & object, const Point2D & robot, double standoff);

}  // namespace semantic_nav
