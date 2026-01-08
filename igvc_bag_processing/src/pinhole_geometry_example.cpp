#include <rclcpp/rclcpp.hpp>
#include "PinholeGeometry.hpp"

namespace igvc_bag_processing {

  class PinholeGeometryExample : public rclcpp::Node {
    public:
      PinholeGeometryExample()
      : rclcpp::Node("pinhole_geometry_example")
      {
        vehicle_frame_ = this->declare_parameter<std::string>("vehicle_frame", "base_footprint");
        camera_frame_ = this->declare_parameter<std::string>("camera_frame_", "stereo_left");
        lidar_frame_ = this->declare_parameter<std::string>("lidar_frame_", "laser");
        pinhole_geometry_ = std::make_shared<igvc_bag_processing::PinholeGeometry>((rclcpp::Node*)this, vehicle_frame_, camera_frame_, lidar_frame_);
        sub_camera_info_ = this->create_subscription<sensor_msgs::msg::CameraInfo>(
          "camera_info",
          10,
          std::bind(&PinholeGeometryExample::recv_camera_info, this, std::placeholders::_1)
        );

        timer_ = rclcpp::create_timer(
          this,
          this->get_clock(),
          std::chrono::milliseconds(500),
          std::bind(&PinholeGeometryExample::timerCallback, this)
        );
      }

    private:

      rclcpp::TimerBase::SharedPtr timer_;
      void timerCallback() {
        if (!pinhole_geometry_->lookup_static_transforms()) {
          RCLCPP_ERROR(this->get_logger(), "Failed to lookup static transforms");
          return;
        }

        // Projecting a point that could come from a LaserScan into the image and finding the pixel it hits
        tf2::Vector3 example_lidar_point(1.5, 0.0, 0.0);
        cv::Point output_pixel = pinhole_geometry_->lidar_to_pixel(example_lidar_point);
        RCLCPP_INFO_STREAM(get_logger(), "Lidar point: " << example_lidar_point.x() << ", " << example_lidar_point.y() << ", " << example_lidar_point.z());
        RCLCPP_INFO_STREAM(get_logger(), "Corresponding pixel: " << output_pixel.x << ", " << output_pixel.y);

        // Projecting a pixel of interest into the vehicle frame, assuming the detected pixel represents a point on the ground.
        cv::Point example_pixel_point(400, 300);
        geometry_msgs::msg::Point32 output_footprint_point = pinhole_geometry_->pixel_to_footprint(example_pixel_point);
        RCLCPP_INFO_STREAM(get_logger(), "Image pixel: " << example_pixel_point.x << ", " << example_pixel_point.y);
        RCLCPP_INFO_STREAM(get_logger(), "Corresponding point in footprint frame: " << output_footprint_point.x << ", " << output_footprint_point.y << ", " << output_footprint_point.z << "\n\n\n");
      }

      rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr sub_camera_info_;
      void recv_camera_info(const sensor_msgs::msg::CameraInfo::ConstSharedPtr msg) {
        // The CameraInfo message from the bag must be sent to the PinholeGeometry class for it to work
        pinhole_geometry_->set_camera_info(*msg);
      }

      // Test instance of PinholeGeometry class
      std::shared_ptr<igvc_bag_processing::PinholeGeometry> pinhole_geometry_;

      // ROS Parameters
      std::string vehicle_frame_;
      std::string camera_frame_;
      std::string lidar_frame_;
  };
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<igvc_bag_processing::PinholeGeometryExample>());
  rclcpp::shutdown();
  return 0;
}