#pragma once
class gdalparser
{
public:
	gdalparser();
	~gdalparser();
	void parseGeometry(unsigned char* data, tripline *trip);
	char *exportGeometry(tripline *trip);
};

