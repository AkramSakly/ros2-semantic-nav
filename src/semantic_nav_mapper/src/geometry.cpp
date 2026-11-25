#include "semantic_nav_mapper/geometry.hpp"

#include <cmath>

namespace semantic_nav
{

double pixelToBearing(double u, double fx, double cx)
{
  // TODO(M3): pinhole model. Think about the sign: image u grows to the right,
  // but a positive ROS angle is to the left.
  (void)u; (void)fx; (void)cx;
  return 0.0;
}

double rangeAtBearing(
  const std::vector<float> & ranges, double angle_min, double angle_increment,
  double bearing, double range_min, double range_max)
{
  // TODO(M3): convert the bearing to an index in `ranges`.
  //   - normalise the angle first (TurtleBot3 scans go from 0 to 2*pi)
  //   - reject inf / nan / out of [range_min, range_max]
  //   - robustness bonus: take the median of a few neighbouring beams
  (void)ranges; (void)angle_min; (void)angle_increment;
  (void)bearing; (void)range_min; (void)range_max;
  return -1.0;
}

int associate(
  const std::vector<TrackedObject> & objects, const std::string & label,
  const Point2D & p, double max_dist)
{
  // TODO(M4): nearest neighbour gated by label and distance.
  (void)objects; (void)label; (void)p; (void)max_dist;
  return -1;
}

void fuse(TrackedObject & obj, const Point2D & p, double confidence)
{
  // TODO(M4): incremental mean, e.g. x_new = x + (p.x - x) / n.
  (void)obj; (void)p; (void)confidence;
}

Pose2D goalNearObject(const Point2D & object, const Point2D & robot, double standoff)
{
  // TODO(M5): put the goal on the segment object->robot, `standoff` metres from
  // the object, and set yaw with std::atan2 so the robot faces the object.
  (void)object; (void)robot; (void)standoff;
  return Pose2D{};
}

}  // namespace semantic_nav
