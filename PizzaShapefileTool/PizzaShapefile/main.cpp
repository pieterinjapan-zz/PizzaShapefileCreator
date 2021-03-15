#include <iostream>
#include "shpfWriter.h"
#include "CalcEngine.h"

int main()
{
	// Initialize CalcEngine class
	std::cout << "\nReading Input Parameters" << std::endl;
	CalcEngine calcEng;
	if (calcEng.getParameters("CalcCond.txt") != 0)
	{
		return -1;
	}

	// Run calculation to generate shapefile
	std::cout << "Generating Shapefile\n" << std::endl;
	calcEng.runCalculation();

	// Export shapefile
	std::cout << "Exporting Shapefile\n" << std::endl;
	calcEng.exportShapefile();

	std::cout << "All is well" << std::endl;
	return 0;
}