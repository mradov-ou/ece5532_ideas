#include <rclcpp/rclcpp.hpp>
#include "PinholeGeometry.hpp"

namespace igvc_bag_processing {

  PinholeGeometry::PinholeGeometry(rclcpp::Node* n, const std::string& vehicle_frame, const std::string& camera_frame, const std::string& lidar_frame)
  : vehicle_frame_(vehicle_frame),
    camera_frame_(camera_frame),
    lidar_frame_(lidar_frame),
    looked_up_transforms_(false)
  {
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(n->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
  }

  bool PinholeGeometry::lookup_static_transforms()
  {
    if (looked_up_transforms_) {
      return true;
    }

    geometry_msgs::msg::TransformStamped transform_lookup;
    try {
      transform_lookup = tf_buffer_->lookupTransform(vehicle_frame_, camera_frame_, rclcpp::Time());
      tf2::convert(transform_lookup.transform, camera_transform_);
      transform_lookup = tf_buffer_->lookupTransform(vehicle_frame_, lidar_frame_, rclcpp::Time());
      tf2::convert(transform_lookup.transform, lidar_transform_);
    } catch (tf2::TransformException& ex) {
      RCLCPP_ERROR(rclcpp::get_logger("PinholeGeometry"), ex.what());
      return false;
    }
    looked_up_transforms_ = true;
    return true;
  }

  void PinholeGeometry::set_camera_info(const sensor_msgs::msg::CameraInfo& camera_info) {
    pinhole_model_.fromCameraInfo(camera_info);
  }

  cv::Point PinholeGeometry::lidar_to_pixel(const tf2::Vector3& lidar_point)
  {
    if (!geometry_ok()) {
      return cv::Point();
    }

    auto lidar_camera_point = (camera_transform_.inverse() * lidar_transform_) * lidar_point;
    return (cv::Point)pinhole_model_.project3dToPixel(cv::Point3d(lidar_camera_point.x(), lidar_camera_point.y(), lidar_camera_point.z()));
  }

  geometry_msgs::msg::Point32 PinholeGeometry::pixel_to_footprint(const cv::Point& pixel_point)
  {
    if (!geometry_ok()) {
      return geometry_msgs::msg::Point32();
    }

    // Convert the input pixel coordinates into a 3d ray, where x and y are projected to the point where z is equal to 1.0
    cv::Point3d cam_frame_ray = pinhole_model_.projectPixelTo3dRay((cv::Point2d)pixel_point);

    // Represent camera frame ray in footprint frame
    tf2::Vector3 footprint_frame_ray = camera_transform_.getBasis() * tf2::Vector3(cam_frame_ray.x, cam_frame_ray.y, cam_frame_ray.z);

    // Using the concept of similar triangles, scale the unit vector such that the end is on the ground plane.
    double s = -camera_transform_.getOrigin().z() / footprint_frame_ray.z();
    tf2::Vector3 ground_plane_ray = s * footprint_frame_ray;

    // Then add camera position offset to obtain the final coordinates in footprint frame
    tf2::Vector3 vehicle_frame_point = ground_plane_ray + camera_transform_.getOrigin();
    // Fill output point with the result of the projection
    geometry_msgs::msg::Point32 output;
    output.x = vehicle_frame_point.x();
    output.y = vehicle_frame_point.y();
    output.z = vehicle_frame_point.z();
    return output;
  }

  bool PinholeGeometry::geometry_ok()
  {
    bool is_ok = true;
    if (!looked_up_transforms_) {
      RCLCPP_ERROR(rclcpp::get_logger("PinholeGeometry"), "Static TF lookups have not been done");
      is_ok = false;
    }
    if (!pinhole_model_.initialized()) {
      RCLCPP_ERROR(rclcpp::get_logger("PinholeGeometry"), "Pinhole camera model not initialized with CameraInfo message");
      is_ok = false;
    }
    return is_ok;
  }

}