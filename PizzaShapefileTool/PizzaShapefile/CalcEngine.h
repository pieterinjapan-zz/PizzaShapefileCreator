#pragma once
#include <string>
#include <vector>


struct parameters
{
	double originX_m;
	double originY_m;
	double radiusMin_m;
	double radiusMax_m;
	double segmentLength_m;
	double segmentAngle_deg;
	double height_m;
	std::string outputFilePath;
};

class CalcEngine
{
private:
	parameters m_params;
	std::vector<std::vector<double>> m_polygonsX;
	std::vector<std::vector<double>> m_polygonsY;
	void generateSlice(const double startSliceX, const double startSliceY, const double angle);
	void generateSegment(const double startSegmentX, const double startSegmentY, const double angle);

public:
	CalcEngine();
	~CalcEngine();
	int getParameters(std::string paramsFilepath);
	void runCalculation();
	void exportShapefile();
};

