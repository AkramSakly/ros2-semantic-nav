"""Send the robot to an object of the semantic map.

Usage:
    ros2 run semantic_nav_commander go_to_object chair

Flow:
    1. call /get_object_pose (served by object_mapper_node) with the label
    2. if found: send the returned goal to Nav2 and report progress
    3. if not found: explore (M6), then try again

Milestones 5 and 6 in the README. Everything marked TODO is yours to write.
"""

import sys

import rclpy
from rclpy.node import Node
from semantic_nav_interfaces.srv import GetObjectPose


class GoToObject(Node):

    def __init__(self):
        super().__init__('go_to_object')
        self.client = self.create_client(GetObjectPose, '/get_object_pose')

    def query(self, label: str):
        if not self.client.wait_for_service(timeout_sec=5.0):
            self.get_logger().error('/get_object_pose not available: is object_mapper_node running?')
            return None
        req = GetObjectPose.Request()
        req.label = label
        future = self.client.call_async(req)
        rclpy.spin_until_future_complete(self, future)
        return future.result()


def main(args=None):
    rclpy.init(args=args)
    label = sys.argv[1] if len(sys.argv) > 1 else 'chair'
    node = GoToObject()

    res = node.query(label)
    if res is None:
        rclpy.shutdown()
        return

    if not res.found:
        # TODO(M6): exploration. Simplest version: rotate in place 360 degrees, then visit a
        #   list of waypoints from config/waypoints.yaml, querying again after each one.
        #   Better version: frontier exploration on the /map occupancy grid.
        node.get_logger().warn(f'No "{label}" in the semantic map yet')
    else:
        # TODO(M5): send res.goal to Nav2.
        #   Hint: from nav2_simple_commander.robot_navigator import BasicNavigator, TaskResult
        #         nav = BasicNavigator(); nav.goToPose(res.goal)
        #         loop on nav.isTaskComplete() and log nav.getFeedback().distance_remaining
        #         then check nav.getResult() == TaskResult.SUCCEEDED
        node.get_logger().info(f'Found "{label}" at ({res.object.position.x:.2f}, '
                               f'{res.object.position.y:.2f}): navigation not implemented yet')

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
