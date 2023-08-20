#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    
    cv::Mat image_points = (cv::Mat_<float>(4, 2) << 100, 150, 300, 150, 300, 300, 100, 300);
    cv::Mat object_points = (cv::Mat_<float>(4, 3) << 0, 0, 0, 10, 0, 0, 10, 10, 0, 0, 10, 0);

    cv::Mat camera_matrix = (cv::Mat_<float>(3, 3) << 500, 0, 0.5,
                                                    0, 500, 0.5,
                                                    0, 0, 1);

    cv::Mat dist_coeffs = cv::Mat();

    cv::Mat rotation_vector, translation_vector;
    cv::solvePnP(object_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

    cv::Mat rotation_matrix;
    cv::Rodrigues(rotation_vector, rotation_matrix);

    std::cout << "Rotation Matrix:\n" << rotation_matrix << std::endl;
    std::cout << "Translation Vector:\n" << translation_vector << std::endl;

    return 0;
}
