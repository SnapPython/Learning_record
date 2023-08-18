int ArmorDetector::detect()
{
	/*
	*	Detect lights and build light bars' desciptors
	*/
	_armors.clear();
	std::vector<LightDescriptor> lightInfos;
	{
		cv::Mat binBrightImg;
        cvtColor(_roiImg, _grayImg, COLOR_BGR2GRAY, 1);
		cv::threshold(_grayImg, binBrightImg, _param.brightness_threshold, 255, cv::THRESH_BINARY);

        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        dilate(binBrightImg, binBrightImg, element);

    #ifdef DEBUG_PRETREATMENT
        imshow("brightness_binary", binBrightImg);
        waitKey();
    #endif // DEBUG_PRETREATMENT

		/*
		*	find and filter light bars
		*/
		vector<vector<Point>> lightContours;
		cv::findContours(binBrightImg.clone(), lightContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		for(const auto& contour : lightContours)
		{
			float lightContourArea = contourArea(contour);
			if(contour.size() <= 5 ||
			   lightContourArea < _param.light_min_area) continue;

			RotatedRect lightRec = fitEllipse(contour);
			adjustRec(lightRec, ANGLE_TO_UP);

			//float solidity = lightContourArea / lightRec.size.area();
            		if(lightRec.size.width / lightRec.size.height > _param.light_max_ratio ||
			   lightContourArea / lightRec.size.area() < _param.light_contour_min_solidity) continue;

			//Mat temp;
			//cvex::showRectangle("light_right_position", _srcImg, temp, lightRec, cvex::GREEN,0, _roi.tl());

			lightRec.size.width *= _param.light_color_detect_extend_ratio;
			lightRec.size.height *= _param.light_color_detect_extend_ratio;
			Rect lightRect = lightRec.boundingRect();
			const Rect srcBound(Point(0, 0), _roiImg.size());
			lightRect &= srcBound;
			Mat lightImg = _roiImg(lightRect);
			Mat lightMask = Mat::zeros(lightRect.size(), CV_8UC1);
			Point2f lightVertexArray[4];
			lightRec.points(lightVertexArray);
			std::vector<Point> lightVertex;
			for(int i = 0; i < 4; i++)
			{
				lightVertex.emplace_back(Point(lightVertexArray[i].x - lightRect.tl().x,
											   lightVertexArray[i].y - lightRect.tl().y));
			}
			fillConvexPoly(lightMask, lightVertex, 255);

            		if(lightImg.size().area() <= 0 || lightMask.size().area() <= 0) continue;
            		cv::dilate(lightMask, lightMask, element);
			const Scalar meanVal = mean(lightImg, lightMask);

			//Mat debugColorImg = _srcImg.clone();
			//const String BGR = "(" + std::to_string(int(meanVal[0])) + ", " + std::to_string(int(meanVal[1])) + ", " + std::to_string(int(meanVal[2])) + ")";

			if(((_enemy_color == BLUE) && (meanVal[BLUE] - meanVal[RED] > 20.0)) || (_enemy_color == RED && meanVal[RED] - meanVal[BLUE] > 20.0))
			{
				lightInfos.push_back(LightDescriptor(lightRec));

				//putText(debugColorImg, BGR, Point(lightVertexArray[0]) + _roi.tl(), FONT_HERSHEY_SIMPLEX, 0.4, cvex::GREEN, 1); //fontScalar 0.34
			}
			//else
			//{
			//	putText(debugColorImg, BGR, Point(lightVertexArray[0]) + _roi.tl(), FONT_HERSHEY_SIMPLEX, 0.4, cvex::CYAN, 1); //fontScalar 0.34
			//}
			//imshow("BGR", debugColorImg);
			//waitKey(0);

		}

	#ifdef DEBUG_DETECTION
		vector<RotatedRect> lightsRecs;
		for(auto & light : lightInfos)
		{
			lightsRecs.emplace_back(light.rec());
		}
        cvex::showRectangles(_debugWindowName, _debugImg, _debugImg, lightsRecs, cvex::MAGENTA, 1, _roi.tl());
	#endif //DEBUG_DETECTION

		if(lightInfos.empty())
		{
			return _flag = ARMOR_NO;
		}
	}


	/*
	*	find and filter light bar pairs
	*/

	{
		sort(lightInfos.begin(), lightInfos.end(), [](const LightDescriptor& ld1, const LightDescriptor& ld2)
		{
			return ld1.center.x < ld2.center.x;
		});
		vector<int> minRightIndices(lightInfos.size(), -1);
		for(size_t i = 0; i < lightInfos.size(); i++)
		{
			for(size_t j = i + 1; (j < lightInfos.size()); j++)
			{
				const LightDescriptor& leftLight  = lightInfos[i];
				const LightDescriptor& rightLight = lightInfos[j];

		#ifdef DEBUG_DETECTION
				Mat pairImg = _debugImg.clone();
				vector<RotatedRect> curLightPair{leftLight.rec(), rightLight.rec()};
                		cvex::showRectangles("debug pairing",  pairImg, pairImg,curLightPair, cvex::CYAN, 1, _roi.tl());
		#endif // DEBUG_DETECTION

				/*
				*	Works for 2-3 meters situation
				*	morphologically similar: // parallel 
								 // similar height
				*/
				float angleDiff_ = abs(leftLight.angle - rightLight.angle);
				float LenDiff_ratio = abs(leftLight.length - rightLight.length) / max(leftLight.length, rightLight.length);
				if(angleDiff_ > _param.light_max_angle_diff_ ||
				   LenDiff_ratio > _param.light_max_height_diff_ratio_)
				{
					continue;
				}

				/*
				*	proper location: // y value of light bar close enough 
				*			 // ratio of length and width is proper
				*/
				float dis = cvex::distance(leftLight.center, rightLight.center);
				float meanLen = (leftLight.length + rightLight.length) / 2;
				float yDiff = abs(leftLight.center.y - rightLight.center.y);
				float yDiff_ratio = yDiff / meanLen;
				float xDiff = abs(leftLight.center.x - rightLight.center.x);
				float xDiff_ratio = xDiff / meanLen;
				float ratio = dis / meanLen;
				if(yDiff_ratio > _param.light_max_y_diff_ratio_ ||
				   xDiff_ratio < _param.light_min_x_diff_ratio_ ||
				   ratio > _param.armor_max_aspect_ratio_ ||
				   ratio < _param.armor_min_aspect_ratio_)
				{
					continue;
				}

				// calculate pairs' info 
				int armorType = ratio > _param.armor_big_armor_ratio ? BIG_ARMOR : SMALL_ARMOR;
				// calculate the rotation score
				float ratiOff = (armorType == BIG_ARMOR) ? max(_param.armor_big_armor_ratio - ratio, float(0)) : max(_param.armor_small_armor_ratio - ratio, float(0));
				float yOff = yDiff / meanLen;
				float rotationScore = -(ratiOff * ratiOff + yOff * yOff);

				ArmorDescriptor armor(leftLight, rightLight, armorType, _grayImg, rotationScore, _param);
				_armors.emplace_back(armor);
				break;
			}
		}

	#ifdef  DEBUG_DETECTION
		vector<vector<Point>> armorVertexs;
		for(const auto & armor : _armors)
		{
			vector<Point> intVertex;
			for(const auto& point : armor.vertex)
			{
				intVertex.emplace_back(Point(point.x,point.y));
			}
			armorVertexs.emplace_back(intVertex);
		}
        cvex::showContours(_debugWindowName, _debugImg, _debugImg,armorVertexs,  cvex::WHITE, 1, _roi.tl());
	#endif //  DEBUG_DETECTION
		
		if(_armors.empty())
		{
			return _flag = ARMOR_NO;
		}
	}

#ifdef GET_ARMOR_PIC
	_allCnt++;
	int i = 0;
	for(const auto & armor : _armors)
    	{
        	Mat regulatedFrontImg = armor.frontImg;
        	if(armor.type == BIG_ARMOR)
        	{
       		     	regulatedFrontImg = regulatedFrontImg(Rect(21, 0, 50, 50));
        	}
        	imwrite("/home/nvidia/Documents/ArmorTrainingSample/" + to_string(_allCnt) + "_" + to_string(i) + ".bmp", regulatedFrontImg);
		i++;
	}
#endif // GET_ARMOR_PIC

	//delete the fake armors
	_armors.erase(remove_if(_armors.begin(), _armors.end(), [](ArmorDescriptor& i)
	{
		return !(i.isArmorPattern());
	}), _armors.end());

	if(_armors.empty())
	{
		_targetArmor.clear();

		if(_flag == ARMOR_LOCAL)
		{
			//cout << "Tracking lost" << endl;
			return _flag = ARMOR_LOST;
		}
		else
		{
			//cout << "No armor pattern detected." << endl;
			return _flag = ARMOR_NO;
		}
	}

	//calculate the final score
	for(auto & armor : _armors)
	{
		armor.finalScore = armor.sizeScore + armor.distScore + armor.rotationScore;
	}

	//choose the one with highest score, store it on _targetArmor
	std::sort(_armors.begin(), _armors.end(), [](const ArmorDescriptor & a, const ArmorDescriptor & b)
	{
		return a.finalScore > b.finalScore;
	});
	_targetArmor = _armors[0];

	//update the flag status	
	_trackCnt++;

#if defined(DEBUG_DETECTION) || defined(SHOW_RESULT)
	vector<Point> intVertex;
	for(const auto& point : _targetArmor.vertex)
	{
		Point fuckPoint = point;
		intVertex.emplace_back(fuckPoint);
	}
   	cvex::showContour(_debugWindowName, _debugImg, _debugImg, intVertex, cvex::GREEN, -1, _roi.tl());
#endif //DEBUG_DETECTION || SHOW_RESULT

	return _flag = ARMOR_LOCAL;
}