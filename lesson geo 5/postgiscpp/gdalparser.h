#pragma once
class gdalparser
{
public:
	gdalparser();
	~gdalparser();
	void gdalparser::parseGeometry(unsigned char* data, tripline *trip);
	char *gdalparser::exportGeometry(tripline *trip);
};

