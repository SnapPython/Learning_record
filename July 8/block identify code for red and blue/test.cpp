#include <opencv2/opencv.hpp>

int main() {
    // Load the image
    cv::Mat image1 = cv::imread("indoor.png");
    cv::Mat image2 = cv::imread("outdoor.png");
    // Convert the image to HSV color space
    cv::Mat hsv_image1, hsv_image2;
    cv::cvtColor(image1, hsv_image1, cv::COLOR_BGR2HSV);   //using HSV color space
    cv::cvtColor(image2, hsv_image2, cv::COLOR_BGR2HSV);   //using HSV color space

    // Define color ranges for different color blocks
    cv::Scalar lower_blue(100, 50, 50);
    cv::Scalar upper_blue(130, 255, 255);
    
    
    cv::Scalar lower_red1(0, 70, 50);    // Minimum HSV values for red (lower range)
    cv::Scalar upper_red1(5, 255, 255); // Maximum HSV values for red (lower range)

    cv::Scalar lower_red2(175, 70, 50);  // Minimum HSV values for red (upper range)
    cv::Scalar upper_red2(180, 255, 255);// Maximum HSV values for red (upper range)
    


    // Create a binary mask for blue color range
    cv::Mat mask10;
    cv::inRange(hsv_image1, lower_blue, upper_blue, mask10);

    // Create a binary mask for red color range
    cv::Mat mask11, mask12;
    cv::inRange(hsv_image1, lower_red1, upper_red1, mask11);
    cv::inRange(hsv_image1, lower_red2, upper_red2, mask12);


    // Create a binary mask for blue color range
    cv::Mat mask20;
    cv::inRange(hsv_image2, lower_blue, upper_blue, mask20);

    // Create a binary mask for red color range
    cv::Mat mask21, mask22;
    cv::inRange(hsv_image2, lower_red1, upper_red1, mask21);
    cv::inRange(hsv_image2, lower_red2, upper_red2, mask22);

    // Combine the masks
    cv::Mat mask1 = mask11 | mask12;
    cv::Mat mask2 = mask21 | mask22;


    // Perform morphological operations (optional)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::erode(mask1, mask1, kernel);
    cv::dilate(mask1, mask1, kernel);

    cv::erode(mask2, mask2, kernel);
    cv::dilate(mask2, mask2, kernel);
	
    cv::erode(mask10, mask10, kernel);
    cv::dilate(mask10, mask10, kernel);
 cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(80, 80));
    cv::erode(mask20, mask20, kernel1);
    cv::dilate(mask20, mask20, kernel1);



    // Find contours in the binary image
    std::vector<std::vector<cv::Point>> contours1,contours2,contours3,contours4;
    std::vector<cv::Vec4i> hierarchy;
 

    cv::findContours(mask1, contours1, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(mask2, contours2, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::findContours(mask10, contours3, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(mask20, contours4, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);


    // Filter and analyze contours
    for (const auto& contour : contours3) {
        double area = cv::contourArea(contour);
        if (area > 1000) {
            // Draw bounding rectangle around the color block
            cv::Rect boundingRect = cv::boundingRect(contour);
            cv::rectangle(image1, boundingRect, cv::Scalar(0, 255, 0), 2);
        }
    }

        // Filter and analyze contours
    for (const auto& contour : contours1) {
        double area = cv::contourArea(contour);
        if (area > 10000 ){
            // Draw bounding rectangle around the color block
            cv::Rect boundingRect = cv::boundingRect(contour);
            cv::rectangle(image1, boundingRect, cv::Scalar(100, 105, 30), 2);
        }
    }

        // Filter and analyze contours
        for (const auto& contour : contours4) {
            double area = cv::contourArea(contour);
            if (area > 1000) {
                // Draw bounding rectangle around the color block
                cv::Rect boundingRect = cv::boundingRect(contour);
                cv::rectangle(image2, boundingRect, cv::Scalar(0, 255, 0), 2);
            }
        }


      


        // Filter and analyze contours
    for (const auto& contour : contours2) {
        double area = cv::contourArea(contour);
        if (area > 10000 ){
            // Draw bounding rectangle around the color block
            cv::Rect boundingRect = cv::boundingRect(contour);
            cv::rectangle(image2, boundingRect, cv::Scalar(100, 105, 30), 2);
        }
    }



    // Display the results
    cv::imshow("Color Blocks", image1);
    cv::waitKey(0);
    cv::imshow("Color Blocks", image2);
    cv::waitKey(0);

    return 0;
}
