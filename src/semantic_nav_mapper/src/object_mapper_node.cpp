// object_mapper_node
//
// Turns 2D detections into objects on the map:
//   /detections (vision_msgs/Detection2DArray)  -> bearing of each box (camera intrinsics)
//   /scan       (sensor_msgs/LaserScan)         -> distance along that bearing
//   TF camera -> base_scan -> map               -> object position in the map frame
//
// Publishes : /semantic_map          (semantic_nav_interfaces/SemanticObjectArray)
//             /semantic_map/markers  (visualization_msgs/MarkerArray, for RViz)
// Service   : /get_object_pose       (semantic_nav_interfaces/srv/GetObjectPose)
//
// Milestones 3, 4 and 5 in the README. Everything marked TODO is yours to write.

#include <memory>
#include <string>
#include <vector>

#include "geometry_msgs/msg/pose_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "semantic_nav_interfaces/msg/semantic_object_array.hpp"
#include "semantic_nav_interfaces/srv/get_object_pose.hpp"
#include "semantic_nav_mapper/geometry.hpp"
#include "sensor_msgs/msg/camera_info.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "vision_msgs/msg/detection2_d_array.hpp"
#include "visualization_msgs/msg/marker_array.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

namespace semantic_nav
{

class ObjectMapperNode : public rclcpp::Node
{
public:
  ObjectMapperNode()
  : Node("object_mapper_node")
  {
    map_frame_ = declare_parameter<std::string>("map_frame", "map");
    association_distance_ = declare_parameter<double>("association_distance", 0.6);
    min_observations_ = declare_parameter<int>("min_observations", 3);
    goal_standoff_ = declare_parameter<double>("goal_standoff", 0.6);

    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    info_sub_ = create_subscription<sensor_msgs::msg::CameraInfo>(
      "/camera/camera_info", rclcpp::SensorDataQoS(),
      std::bind(&ObjectMapperNode::onCameraInfo, this, _1));
    scan_sub_ = create_subscription<sensor_msgs::msg::LaserScan>(
      "/scan", rclcpp::SensorDataQoS(),
      std::bind(&ObjectMapperNode::onScan, this, _1));
    det_sub_ = create_subscription<vision_msgs::msg::Detection2DArray>(
      "/detections", 10, std::bind(&ObjectMapperNode::onDetections, this, _1));

    map_pub_ = create_publisher<semantic_nav_interfaces::msg::SemanticObjectArray>(
      "/semantic_map", 10);
    marker_pub_ = create_publisher<visualization_msgs::msg::MarkerArray>(
      "/semantic_map/markers", 10);

    srv_ = create_service<semantic_nav_interfaces::srv::GetObjectPose>(
      "/get_object_pose", std::bind(&ObjectMapperNode::onGetObjectPose, this, _1, _2));

    timer_ = create_wall_timer(std::chrono::milliseconds(500), [this]() {publishMap();});

    RCLCPP_INFO(get_logger(), "object_mapper_node started");
  }

private:
  void onCameraInfo(const sensor_msgs::msg::CameraInfo::SharedPtr msg)
  {
    // Intrinsics do not change in simulation: keep the first one.
    if (!camera_info_) {
      camera_info_ = msg;
    }
  }

  void onScan(const sensor_msgs::msg::LaserScan::SharedPtr msg)
  {
    last_scan_ = msg;
  }

  void onDetections(const vision_msgs::msg::Detection2DArray::SharedPtr msg)
  {
    if (!camera_info_ || !last_scan_) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000, "waiting for camera_info and scan");
      return;
    }

    for (const auto & det : msg->detections) {
      (void)det;
      // TODO(M3): for each detection
      //   1. bearing in the camera frame: pixelToBearing(det.bbox.center.position.x, K[0], K[2])
      //   2. express that ray in the LiDAR frame. The camera and base_scan are not at the same
      //      place: use tf_buffer_->lookupTransform(scan_frame, camera_frame, stamp).
      //      Careful: the camera *optical* frame has z forward, x right.
      //   3. range = rangeAtBearing(...); skip the detection if range < 0
      //   4. build the point in base_scan, transform it to map_frame_ (tf2::doTransform)
      //      Catch tf2::TransformException and log it with RCLCPP_WARN_THROTTLE.
      //
      // TODO(M4): data association
      //   int idx = associate(objects_, label, p, association_distance_);
      //   idx < 0 -> push a new TrackedObject (next_id_++), else fuse(objects_[idx], p, score)
    }
  }

  void publishMap()
  {
    semantic_nav_interfaces::msg::SemanticObjectArray out;
    out.header.stamp = now();
    out.header.frame_id = map_frame_;
    // TODO(M4): copy every object with observations >= min_observations_ into `out`
    //           (filters one-frame false positives).
    map_pub_->publish(out);

    // TODO(M4): publish one sphere + one TEXT_VIEW_FACING marker per object so you
    //           can see the semantic map in RViz. This makes a great GIF for the README.
  }

  void onGetObjectPose(
    const std::shared_ptr<semantic_nav_interfaces::srv::GetObjectPose::Request> req,
    std::shared_ptr<semantic_nav_interfaces::srv::GetObjectPose::Response> res)
  {
    res->found = false;
    // TODO(M5): find the best object with label == req->label (most observations),
    //   get the robot position (TF map -> base_footprint), compute
    //   goalNearObject(object, robot, goal_standoff_), fill res->goal (frame map_frame_,
    //   yaw -> quaternion with tf2::Quaternion::setRPY) and res->object, set found = true.
    RCLCPP_INFO(get_logger(), "get_object_pose('%s'): not implemented yet", req->label.c_str());
  }

  // Parameters
  std::string map_frame_;
  double association_distance_{0.6};
  int min_observations_{3};
  double goal_standoff_{0.6};

  // State
  std::vector<TrackedObject> objects_;
  unsigned int next_id_{0};
  sensor_msgs::msg::CameraInfo::SharedPtr camera_info_;
  sensor_msgs::msg::LaserScan::SharedPtr last_scan_;

  // ROS interfaces
  std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr info_sub_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
  rclcpp::Subscription<vision_msgs::msg::Detection2DArray>::SharedPtr det_sub_;
  rclcpp::Publisher<semantic_nav_interfaces::msg::SemanticObjectArray>::SharedPtr map_pub_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
  rclcpp::Service<semantic_nav_interfaces::srv::GetObjectPose>::SharedPtr srv_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace semantic_nav

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<semantic_nav::ObjectMapperNode>());
  rclcpp::shutdown();
  return 0;
}
