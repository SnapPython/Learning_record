#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp> 
#include "ArmorDetector.h"
#include"opencv_extended.h"

int main() {
    cv::Mat srcImg = cv::imread("red.png"); // you can change to "blue.png"
    if (srcImg.empty()) {
        std::cout << "Could not open or find the image." << std::endl;
        return -1;
    }

    // Initialize the Armor Detector 
    rm::ArmorParam armorParam; 
    rm::ArmorDetector armorDetector;
    armorDetector.init(armorParam);

    // Set the Enemy Color to Red
    armorDetector.setEnemyColor(0); // Set enemy color to red (1) blue (0)

    // Provide the source image to the ArmorDetector
    armorDetector.loadImg(srcImg);

    // Detect the Armor
    int detectionResult = armorDetector.detect(); // Perform armor detection



    if (detectionResult == rm::ArmorDetector::ARMOR_NO) {
        std::cout << "Armor not found." << std::endl;
    } else if (detectionResult == rm::ArmorDetector::ARMOR_LOST) {
        std::cout << "Armor tracking lost." << std::endl;
    } else if (detectionResult == rm::ArmorDetector::ARMOR_GLOBAL || detectionResult == rm::ArmorDetector::ARMOR_LOCAL) {
        std::cout << "Armor detected!" << std::endl;

        //Retrieve Detected Armor Properties
        std::vector<cv::Point2f> armorVertices = armorDetector.getArmorVertex(); // Get corner points of the detected armor

        // Draw Bounding Box on the Image
        cv::Point2f topLeft = armorVertices[0];
        cv::Point2f bottomRight = armorVertices[2];
        cv::rectangle(srcImg, topLeft, bottomRight, cv::Scalar(0, 255, 0), 2); // Draw a green rectangle

        // Display the Image with Detected Armor
        cv::imshow("Detected Armor", srcImg);
        cv::waitKey(0); // Wait for a key press to close the window
    }

    return 0;
}