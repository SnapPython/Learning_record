#include <opencv2/opencv.hpp>

int main() {
    // Load the image
    cv::Mat image = cv::imread("apple.png");

    // Convert the image to HSV color space
    cv::Mat hsv_image;
    cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);   //using HSV color space

    // Define color ranges for different color blocks
 
    
    cv::Scalar lower_blue(100, 50, 50);
    cv::Scalar upper_blue(130, 255, 255);
    
    
    cv::Scalar lower_red1(0, 70, 50);    // Minimum HSV values for red (lower range)
    cv::Scalar upper_red1(5, 255, 255); // Maximum HSV values for red (lower range)

    cv::Scalar lower_red2(175, 70, 50);  // Minimum HSV values for red (upper range)
    cv::Scalar upper_red2(180, 255, 255);// Maximum HSV values for red (upper range)
    

    //cv::Scalar lower_red2(158, 33, 24);  // Minimum HSV values for red (upper range)
    //cv::Scalar upper_red2(180, 255, 255);// Maximum HSV values for red (upper range)




    // Create a binary mask for blue color range
    cv::Mat mask;
    cv::inRange(hsv_image, lower_blue, upper_blue, mask);


    // Create a binary mask for red color range
    cv::Mat mask1, mask2;
    cv::inRange(hsv_image, lower_red1, upper_red1, mask1);
    cv::inRange(hsv_image, lower_red2, upper_red2, mask2);
    // Combine the masks
    cv::Mat mask0 = mask1 | mask2;


    // Perform morphological operations (optional)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

	
    // Find contours(blue) in the binary image
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Find contours(red) in the binary image
    std::vector<std::vector<cv::Point>> contours0;
    cv::findContours(mask0, contours0, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);




    // Filter and analyze contours
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 1000) {
            // Draw bounding rectangle around the color block
            cv::Rect boundingRect = cv::boundingRect(contour);
            cv::rectangle(image, boundingRect, cv::Scalar(0, 255, 0), 2);
        }
    }

        // Filter and analyze contours
    for (const auto& contour : contours0) {
        double area = cv::contourArea(contour);
        if (area > 10000 ){
            // Draw bounding rectangle around the color block
            cv::Rect boundingRect = cv::boundingRect(contour);
            cv::rectangle(image, boundingRect, cv::Scalar(100, 105, 30), 2);
        }
    }



    // Display the results
    cv::imshow("Color Blocks", image);
    cv::waitKey(0);

    return 0;
}
