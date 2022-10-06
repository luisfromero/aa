#include "umaobd.h"
#include "ogrsf_frmts.h"
#include "libpq-fe.h"
#include "gdalparser.h"



gdalparser::gdalparser()
{
}


gdalparser::~gdalparser()
{
}

int totcnt = 0;
int cnt = 0;


char * gdalparser::exportGeometry(tripline *trip)
{
	OGRLineString *nuevols = new OGRLineString();
	for (int i = 0; i < trip->npoints; i++) {
		OGRPoint *nuevo = new OGRPoint(trip->points[i]->longitude, trip->points[i]->latitude);
		nuevols->addPoint(nuevo);

	}
	return nuevols->exportToJson();

}

void gdalparser::parseGeometry(unsigned char *wkt, tripline *trip)
{
	OGRSpatialReference osr;
	OGRGeometry *geom = NULL;
	// Parse WKB
	OGRErr err = OGRGeometryFactory::createFromWkt((char **)&wkt, &osr, &geom);
	if (err != OGRERR_NONE) {
		// process error, like emit signal
		return;
	}
	
	//return;

	// Analyse geometry by type and process them as you wish
	OGRwkbGeometryType type = wkbFlatten(geom->getGeometryType());




	switch (type) {
	case wkbLineString:
	{
		cnt++;
		OGRLineString *poRing = (OGRLineString*)geom;

		// Access line string nodes for example :
		int numNode = poRing->getNumPoints();
		trip->fill(numNode);
		totcnt += numNode;
		OGRPoint p;
		//printf("%03d: Se han encontrado %d puntos en el track. Total: %d\n", cnt, numNode,totcnt);
		for (int i = 0; i < numNode; i++) {
			poRing->getPoint(i, &p);
			trip->points[i] = new trippoint(p.getX(),p.getY());
			trip->points[i]->numorder = i;
			//qDebug() << p.getX() << p.getY();
		}
		break;
	}
	case wkbPoint:
	{
		OGRPoint *point = (OGRPoint *)geom;
		break;
	}
	case wkbMultiLineString:
	{
		OGRGeometryCollection  *poCol = (OGRGeometryCollection*)geom;
		int numCol = poCol->getNumGeometries();
		for (int i = 0; i < numCol; i++) {
			// Access line length for example :
			//qDebug() << poCol->getGeometryRef(i)->get_Length();
		}
		break;
	}
	default:
		// process error, like emit signal
		break;
	}
/*
	// Clean-up
	OGRGeometryFactory::destroyGeometry(geom);
	*/
}
