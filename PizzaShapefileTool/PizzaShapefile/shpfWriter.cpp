#include "shpfWriter.h"

ShapefileWriter::ShapefileWriter(const char* filename)
{
	m_filename = filename;
	const int nSHPType = SHPT_POLYGON;
	m_hSHP = SHPCreate(filename, nSHPType);
	m_hDBF = DBFCreate(filename);
}

ShapefileWriter::~ShapefileWriter()
{
	SHPClose(m_hSHP);
	DBFClose(m_hDBF);
}

// <fieldname> Name of the field to add
// <nWidth> Total number of characters to save
// <nDecimal> number of decimal places to save
// returns field number
int ShapefileWriter::addDoubleField(const char* fieldname, int nWidth, int nDecimal)
{
	int iField = DBFAddField(m_hDBF, fieldname, FTDouble, nWidth, nDecimal);
	return iField;
}

// <x> x coordinate of the vertices
// <y> y coordinate of the vertices
// <numParts> Number of parts of the object
// <panPartStart> List of zero based start vertices for the parts in the object
// <iField> Field number
// <value> Value for the field
void ShapefileWriter::addPolygon(std::vector<double> x, std::vector<double> y, int numParts, int* panPartStart, int iField, double value)
{
	SHPObject *psShape = SHPCreateObject(SHPT_POLYGON, -1, numParts, panPartStart, NULL, x.size(), x.data(), y.data(), NULL, NULL);
	int iShape = SHPWriteObject(m_hSHP, -1, psShape);
	DBFWriteDoubleAttribute(m_hDBF, iShape, iField, value);
	SHPDestroyObject(psShape);
}