#define _USE_MATH_DEFINES
#include "CalcEngine.h"
#include "shpfWriter.h"
#include <math.h>
#include <fstream>
#include <iostream>

CalcEngine::CalcEngine()
{
	// Empty
}

CalcEngine::~CalcEngine()
{
	// Empty
}

int
CalcEngine::getParameters(std::string paramsFilepath)
{
	std::ifstream paramsFile(paramsFilepath);
	std::string line;
	if (paramsFile.is_open())
	{
		while (std::getline(paramsFile, line))
		{
			if (line.empty() || line.at(0) == '#' || line.at(0) == '\r')
			{
				continue;
			}
			else
			{
				std::string delimiter = "=";
				int len = line.length();
				int delimiterPos = line.find(delimiter);
				std::string token = line.substr(0, delimiterPos);
				if (token == "originX_m")
				{
					const std::string lineValue = line.substr(delimiterPos + 1, len - (delimiterPos + 1));
					m_params.originX_m = std::stod(lineValue);
					std::cout << "- " + token + ": " + lineValue << std::endl;
				}
				else if (token == "originY_m")
				{
					const std::string lineValue = line.substr(delimiterPos + 1, len - (delimiterPos + 1));
					m_params.originY_m = std::stod(lineValue);
					std::cout << "- " + token + ": " + lineValue << std::endl;
				}
				else if (token == "radiusMin_m")
				{
					const std::string lineValue = line.substr(delimiterPos + 1, len - (delimiterPos + 1));
					m_params.radiusMin_m = std::stod(lineValue);
					std::cout << "- " + token + ": " + lineValue << std::endl;
				}
				else if (token == "radiusMax_m")
				{
					const std::string lineValue = line.substr(delimiterPos + 1, len - (delimiterPos + 1));
					m_params.radiusMax_m = std::stod(lineValue);
					std::cout << "- " + token + ": " + lineValue << std::endl;
				}
				else if (token == "segmentLength_m")
				{
					const std::string lineValue = line.substr(delimiterPos + 1, len - (delimiterPos + 1));
					m_params.segmentLength_m = std::stod(lineValue);
					std::cout << "- " + token + ": " + lineValue << std::endl;
				}
				else if (token == "segmentAngle_deg")
				{
					const std::string lineValue = line.substr(delimiterPos + 1, len - (delimiterPos + 1));
					m_params.segmentAngle_deg = std::stod(lineValue);
					std::cout << "- " + token + ": " + lineValue << std::endl;
				}
				else if (token == "height_m")
				{
					const std::string lineValue = line.substr(delimiterPos + 1, len - (delimiterPos + 1));
					m_params.height_m = std::stod(lineValue);
					std::cout << "- " + token + ": " + lineValue << std::endl;
				}
				else if (token == "outputFilePath")
				{
					const std::string lineValue = line.substr(delimiterPos + 1, len - (delimiterPos + 1));
					m_params.outputFilePath = lineValue;
					std::cout << "- " + token + ": " + lineValue << std::endl;
				}
				else if (token == "")
				{
					// Add parameters
				}
			}
		}
		paramsFile.close();
		std::cout << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Could not open parameters file.";
		return -1;
	}
}

void CalcEngine::runCalculation()
{
	// add half angle shift to starting point of initial slice
	const double cosHalfTheta = cos(M_PI * m_params.segmentAngle_deg / 360.0);
	const double sinHalfTheta = sin(M_PI * m_params.segmentAngle_deg / 360.0);
	const double directionVecX = sinHalfTheta * m_params.radiusMin_m;
	const double directionVecY = cosHalfTheta * m_params.radiusMin_m;

	const int numSlice = floor(360.0 / m_params.segmentAngle_deg);
	
	for (int i = 0; i < numSlice; i++) {

		// calculate starting point for i'th slice of circle
		const double sliceTheta_i = m_params.segmentAngle_deg * i;
		const double cosTheta_i = cos(M_PI * sliceTheta_i / 180.0);
		const double sinTheta_i = sin(M_PI * sliceTheta_i / 180.0);
		const double startSliceX_i = m_params.originX_m + cosTheta_i * directionVecX + sinTheta_i * directionVecY;
		const double startSliceY_i = m_params.originY_m - sinTheta_i * directionVecX + cosTheta_i * directionVecY;

		// generate polygons for i'th slice of circle
		generateSlice(startSliceX_i, startSliceY_i, m_params.segmentAngle_deg);
	}

	// add final slice if neccesary  
	const double angle_swept = numSlice * m_params.segmentAngle_deg;
	if (angle_swept < 360.0) {
		const double angle_last = 360.0 - angle_swept;
		const double cosTheta_l = cos(M_PI * angle_swept / 180.0);
		const double sinTheta_l = sin(M_PI * angle_swept / 180.0);
		const double startSliceX_l = m_params.originX_m + cosTheta_l * directionVecX + sinTheta_l * directionVecY;
		const double startSliceY_l = m_params.originY_m - sinTheta_l * directionVecX + cosTheta_l * directionVecY;
		generateSlice(startSliceX_l, startSliceY_l, angle_last);
	}
}

// generate polygons making up single slice of circle 
void CalcEngine::generateSlice(const double startSliceX, const double startSliceY, const double angle)
{
	// unit vector from origin to start point of slice
	const double unitX = (startSliceX - m_params.originX_m) / m_params.radiusMin_m;
	const double unitY = (startSliceY - m_params.originY_m) / m_params.radiusMin_m;

	// calculate list of starting points for each segment of slice
	const int numSegments = ceil((m_params.radiusMax_m - m_params.radiusMin_m) / m_params.segmentLength_m);
	for (int i = 0; i < numSegments; i++) {
		const double startSegmentX_i = startSliceX + m_params.segmentLength_m * i * unitX;
		const double startSegmentY_i = startSliceY + m_params.segmentLength_m * i * unitY;
		generateSegment(startSegmentX_i, startSegmentY_i, angle);
	}
}

// generate polygon making up segment of slice
void CalcEngine::generateSegment(const double startSegmentX, const double startSegmentY, const double angle)
{
	// define unit vector from origin to start point
	const double distOriginToBottomLeft = sqrt(pow(startSegmentX - m_params.originX_m, 2.0) + pow(startSegmentY - m_params.originY_m, 2.0));
	const double unitX = (startSegmentX - m_params.originX_m) / distOriginToBottomLeft;
	const double unitY = (startSegmentY - m_params.originY_m) / distOriginToBottomLeft;

	// define unit vector rotated clockwise by the segment angle
	const double cosTheta = cos(M_PI * angle / 180.0);
	const double sinTheta = sin(M_PI * angle / 180.0);
	const double unitXRotated = cosTheta * unitX + sinTheta * unitY;
	const double unitYRotated = -sinTheta * unitX + cosTheta * unitY;

	// initialize vectors for storing x and y values of polygon points
	std::vector<double> xCoordinates; 
	std::vector<double> yCoordinates; 

	// add start point to coordinates
	xCoordinates.push_back(startSegmentX);
	yCoordinates.push_back(startSegmentY);

	// add top left edge point
	const double topLeftX = startSegmentX + m_params.segmentLength_m * unitX;
	const double topLeftY = startSegmentY + m_params.segmentLength_m * unitY;
	xCoordinates.push_back(topLeftX);
	yCoordinates.push_back(topLeftY);

	// add top right edge point
	const double topRightX = m_params.originX_m + (distOriginToBottomLeft + m_params.segmentLength_m) * unitXRotated;
	const double topRightY = m_params.originY_m + (distOriginToBottomLeft + m_params.segmentLength_m) * unitYRotated;
	xCoordinates.push_back(topRightX);
	yCoordinates.push_back(topRightY);
	
	// add bottom right edge point
	const double bottomRightX = m_params.originX_m + distOriginToBottomLeft * unitXRotated;
	const double bottomRightY = m_params.originY_m + distOriginToBottomLeft * unitYRotated;
	xCoordinates.push_back(bottomRightX);
	yCoordinates.push_back(bottomRightY);

	// close polygon
	xCoordinates.push_back(startSegmentX);
	yCoordinates.push_back(startSegmentY);

	// add polygons to m_polygonsX and m_polygonsY
	m_polygonsX.push_back(xCoordinates);
	m_polygonsY.push_back(yCoordinates);
}

void CalcEngine::exportShapefile()
{
	// set up shapefile: 
	std::string outputFileName = m_params.outputFilePath + "outputShapefile";
	ShapefileWriter shpW(outputFileName.c_str());
	int iField = shpW.addDoubleField("BLD_HEIGHT", 7, 3);
	int panPartStart[] = { 0 };
	int numParts = 1;

	// generate shapefile:
	for (int i = 0; i < m_polygonsX.size(); i++) {
		shpW.addPolygon(m_polygonsX[i], m_polygonsY[i], numParts, panPartStart, iField, m_params.height_m);
	}
}


