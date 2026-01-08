#pragma once
// Message types
#include <geometry_msgs/msg/point32.hpp>

// TF lookup headers
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_ros/transform_listener.hpp>
#include <tf2_ros/buffer.hpp>

// Image processing and camera geometry headers
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <image_geometry/pinhole_camera_model.hpp>

namespace igvc_bag_processing {

  class PinholeGeometry {
    public:
      PinholeGeometry(rclcpp::Node* n, const std::string& vehicle_frame, const std::string& camera_frame, const std::string& lidar_frame);
      bool lookup_static_transforms();
      void set_camera_info(const sensor_msgs::msg::CameraInfo& camera_info);
      cv::Point lidar_to_pixel(const tf2::Vector3& lidar_point);
      geometry_msgs::msg::Point32 pixel_to_footprint(const cv::Point& pixel_point);

    private:

      std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
      std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
      tf2::Transform camera_transform_; // Coordinate transformation from footprint to camera
      tf2::Transform lidar_transform_;  // Coordinate transformation from footprint to lidar
      std::string vehicle_frame_;
      std::string camera_frame_;
      std::string lidar_frame_;
      bool looked_up_transforms_;
      image_geometry::PinholeCameraModel pinhole_model_;

      bool geometry_ok();
  };

}