# ros2-semantic-nav: ROS 2 Humble + Gazebo + Nav2 + TurtleBot3 + YOLOv8
#
# Build (from the repo root):
#   docker build -t semantic-nav -f docker/Dockerfile .
# Run with a display (Linux):
#   xhost +local:docker
#   docker run -it --rm --net=host -e DISPLAY=$DISPLAY \
#       -v /tmp/.X11-unix:/tmp/.X11-unix -v $(pwd)/src:/ws/src semantic-nav
#   ros2 launch semantic_nav_bringup sim.launch.py

FROM osrf/ros:humble-desktop

ENV DEBIAN_FRONTEND=noninteractive \
    TURTLEBOT3_MODEL=waffle \
    PIP_NO_CACHE_DIR=1

RUN apt-get update && apt-get install -y --no-install-recommends \
        python3-pip \
        python3-colcon-common-extensions \
        ros-humble-gazebo-ros-pkgs \
        ros-humble-turtlebot3-gazebo \
        ros-humble-turtlebot3-navigation2 \
        ros-humble-navigation2 \
        ros-humble-nav2-bringup \
        ros-humble-nav2-simple-commander \
        ros-humble-slam-toolbox \
        ros-humble-vision-msgs \
        ros-humble-cv-bridge \
        ros-humble-tf2-geometry-msgs \
    && rm -rf /var/lib/apt/lists/*

# CPU wheels keep the image small. Swap for the CUDA index if you have an NVIDIA GPU.
RUN pip3 install --index-url https://download.pytorch.org/whl/cpu torch torchvision \
    && pip3 install ultralytics onnxruntime "numpy<2"

WORKDIR /ws
COPY src/ src/
RUN . /opt/ros/humble/setup.sh \
    && rosdep update \
    && rosdep install --from-paths src --ignore-src -y || true \
    && colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release

RUN echo "source /opt/ros/humble/setup.bash" >> /root/.bashrc \
    && echo "source /ws/install/setup.bash" >> /root/.bashrc \
    && echo "source /usr/share/gazebo/setup.sh" >> /root/.bashrc

CMD ["bash"]
