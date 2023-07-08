#include <opencv2/opencv.hpp>

int main() {
    // Load the image
    cv::Mat image = cv::imread("apple.png");

    // Convert the image to HSV color space
    cv::Mat hsv_image;
    cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);

    // Display the image for color picking
    cv::imshow("Color Picker", image);
    cv::waitKey(0);

    // Select a region that represents the red color
    // Note down the HSV values of the selected region
    int lower_hue = 0;
    int upper_hue = 10;
    int lower_saturation = 100;
    int upper_saturation = 255;
    int lower_value = 100;
    int upper_value = 255;

    // Define the lower and upper bounds for red color range
    cv::Scalar lower_red(lower_hue, lower_saturation, lower_value);
    cv::Scalar upper_red(upper_hue, upper_saturation, upper_value);

    // Create a binary mask for red color range
    cv::Mat mask;
    cv::inRange(hsv_image, lower_red, upper_red, mask);

    // Apply the mask to the original image
    cv::Mat result;
    cv::bitwise_and(image, image, result, mask);

    // Display the result
    cv::imshow("Red Detection", result);
    cv::waitKey(0);

    return 0;
}

