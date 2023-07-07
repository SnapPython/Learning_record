#include <opencv2/opencv.hpp>

int main() {
    // Load the image
    cv::Mat image = cv::imread("apple.png");

    // Convert the image to HSV color space
    cv::Mat hsv_image;
    cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);

    // Define color ranges for different color blocks
    cv::Scalar lower_blue(100, 50, 50);
    cv::Scalar upper_blue(130, 255, 255);

    // Create a binary mask for blue color range
    cv::Mat mask;
    cv::inRange(hsv_image, lower_blue, upper_blue, mask);

    // Perform morphological operations (optional)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    // Find contours in the binary image
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Filter and analyze contours
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 100) {
            // Draw bounding rectangle around the color block
            cv::Rect boundingRect = cv::boundingRect(contour);
            cv::rectangle(image, boundingRect, cv::Scalar(0, 255, 0), 2);
        }
    }

    // Display the results
    cv::imshow("Color Blocks", image);
    cv::waitKey(0);

    return 0;
}
