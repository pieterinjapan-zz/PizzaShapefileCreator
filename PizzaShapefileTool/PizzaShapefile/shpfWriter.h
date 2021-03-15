#include <vector>
#include "shapelib/shapefil.h"

class ShapefileWriter
{
public:
	ShapefileWriter(const char* filename);
	~ShapefileWriter();
	int addDoubleField(const char* fieldname, int nWidth, int nDecimal);
	void addPolygon(std::vector<double> x, std::vector<double> y, int numParts, int* panPartStart, int iField, double value);

private:
	const char* m_filename;
	SHPHandle m_hSHP;
	DBFHandle m_hDBF;
	unsigned long long m_numVertices;
	unsigned long long m_numParts;
};