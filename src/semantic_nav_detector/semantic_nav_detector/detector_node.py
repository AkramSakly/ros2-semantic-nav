"""YOLOv8 detector node.

Subscribes : /camera/image_raw        (sensor_msgs/Image)
Publishes  : /detections              (vision_msgs/Detection2DArray)
             /detections/image        (sensor_msgs/Image, annotated, for RViz)

Milestone 2 in the README. Everything marked TODO is yours to write.
"""

import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import Image
from vision_msgs.msg import Detection2DArray


class DetectorNode(Node):

    def __init__(self):
        super().__init__('detector_node')

        # Parameters (override them in config/detector.yaml)
        self.declare_parameter('model_path', 'yolov8n.pt')
        self.declare_parameter('confidence_threshold', 0.5)
        self.declare_parameter('classes_of_interest', ['chair', 'couch', 'potted plant',
                                                       'bed', 'tv', 'bottle', 'person'])
        self.declare_parameter('publish_annotated_image', True)

        # TODO(M2): load the YOLOv8 model once here.
        #   Hint: `from ultralytics import YOLO; self.model = YOLO(path)`.
        #   Bonus (M6): export to ONNX and run it with onnxruntime, like your lane follower,
        #   then compare FPS in the README.
        self.model = None

        # TODO(M2): create a CvBridge to convert sensor_msgs/Image <-> numpy.

        self.det_pub = self.create_publisher(Detection2DArray, '/detections', 10)
        self.img_pub = self.create_publisher(Image, '/detections/image', 10)
        self.create_subscription(Image, '/camera/image_raw', self.on_image,
                                 qos_profile_sensor_data)

        self.get_logger().info('detector_node started (model not loaded yet: see TODOs)')

    def on_image(self, msg: Image) -> None:
        out = Detection2DArray()
        out.header = msg.header  # keep the camera frame and stamp: the mapper needs both

        # TODO(M2): run the model and fill `out.detections`.
        #   For each box keep only classes in `classes_of_interest` and score >= threshold.
        #   vision_msgs on Humble: det.bbox.center.position.x / .y, det.bbox.size_x / size_y,
        #   and a vision_msgs/ObjectHypothesisWithPose in det.results with
        #   hypothesis.class_id = label name and hypothesis.score = confidence.
        #   Tip: write the box conversion as a pure function so you can unit test it
        #   without ROS (see test/test_detector_utils.py).

        # TODO(M2): if publish_annotated_image, draw the boxes and publish on img_pub.

        self.det_pub.publish(out)


def main(args=None):
    rclpy.init(args=args)
    node = DetectorNode()
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
