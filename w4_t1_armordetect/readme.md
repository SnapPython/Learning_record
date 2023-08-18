# Structure:

* ## header:
    * General.h
    * ArmorDetector.h
    * numeric_rm.h
    * opencv_extended.h

* ## code:
    * ArmorDetector.cpp
    * numeric_rm.cpp
    * opencv_extended.cpp
    * rec.cpp

* ## .xml files

* ## CMake files

* ## Photos

* ## License

* ## Others
    dec.cpp(used to test ArmorDector function)


# Changes:
* Modified include paths for the head files
* line 269 in ArmorDetector.h delete:"init"
* line 263 in ArmorDetector.cpp
    * replace 
    * cv::findContours(binBrightImg.clone(), lightContours,  CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    * with 
    * cv::findContours(binBrightImg.clone(), lightContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

* function int ArmorDetector::detect() in ArmorDetector.cpp 
* opencv_extended.cpp line 30 	normalize(hist, hist, 0, histImage.rows, CV_MINMAX, CV_32F);  No definaton of CV_MINMAX changed to NORM_MINMAX
* ArmorDetector::detect() in ArmorDetector.cpp  line 301 302 306 deleted