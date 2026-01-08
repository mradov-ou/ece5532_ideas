# IGVC Bag Processing

This project involves processing data from a ROS bag recorded during one of the official runs by Oakland University's team in the 2014 IGVC competition and implementing the perception and mapping systems.

## Objectives

### Level 1: Implement LIDAR-Only Costmap

- Build an obstacle layer costmap using just the LIDAR data. Start with using just dead reckoning odometry as the localization source.
- Combine wheel encoders with GPS using the Kalman filter example from lecture to improve localization.

### Level 2: Add Lines to Costmap

- Detect the white lines in the camera image using the `RgbHough` image processing example from lecture, with possibly some modifications.
- Project the Hough transform lines into `base_footprint` frame using the `PinholeGeometry` class in this package.
- Combine the projected points into a `sensor_msgs/PointCloud` message, and add a corresponding costmap layer to the navigation stack configuration from Level 1.

### Level 3: Use LIDAR to Filter Image Processing

- A simple color-based line detection algorithm detects the white part of obstacles and incorrectly projects them into the costmap.
- Using the `PinholeGeometry` class, project the LIDAR points into the image and use the pixel coordinates to exclude regions of the image from the line detection algorithm using a pixel ROI mask.

## Getting Started

The provided `start_system.launch.xml` publishes static TF transforms from vehicle frame to the lidar and camera frames and loads an example RViz configuration that visualizes the LIDAR and camera data, and shows the LIDAR data overlaid on the camera image.

```
ros2 launch igvc_bag_processing start_system.launch.xml
```

Then, play back the bag. The bag is part of the collection of bags for ECE 5532.
Play the bag file back and configure it to publish the clock source for the system:
```
ros2 bag play igvc_bag --clock
```

## Example Image Projection Geometry

To get familiar with how the `PinholeGeometry` class works, an example `pinhole_geometry_example` node is provided in this package. After launching `start_system.launch` and playing the bag, run this node to see the example outputs from the `PinholeGeometry` methods.

```
ros2 run igvc_bag_processing pinhole_geometry_example
```
