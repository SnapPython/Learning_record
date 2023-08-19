#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // Define image dimensions(I don't know TOT)
    int image_width = 500;
    int image_height = 500;

    // Define image points and object points
    cv::Mat image_points = (cv::Mat_<float>(4, 2) << 100, 150, 300, 150, 300, 300, 100, 300);
    cv::Mat object_points = (cv::Mat_<float>(4, 3) << 0, 0, 0, 10, 0, 0, 10, 10, 0, 0, 10, 0);

    // Define camera intrinsics
    float focal_length = 500;  // Focal length
    float center_x = 0.5 * image_width;
    float center_y = 0.5 * image_height;

    cv::Mat camera_matrix = (cv::Mat_<float>(3, 3) << focal_length, 0, center_x,
                                                    0, focal_length, center_y,
                                                    0, 0, 1);

    // Set distortion coefficients (No information provided)
    cv::Mat dist_coeffs = cv::Mat();

    // Solve PnP for rotation and translation
    cv::Mat rotation_vector, translation_vector;
    cv::solvePnP(object_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

    // Convert rotation vector to rotation matrix
    cv::Mat rotation_matrix;
    cv::Rodrigues(rotation_vector, rotation_matrix);

    // Print results
    std::cout << "Rotation Matrix:\n" << rotation_matrix << std::endl;
    std::cout << "Translation Vector:\n" << translation_vector << std::endl;

    return 0;
}
