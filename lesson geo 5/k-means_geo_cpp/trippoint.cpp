#include "umaobd.h"

	trippoint::trippoint(double x, double y)
	{
		latitude = y;
		longitude = x;
	};

	//Punto obtenido por interpolación
	trippoint::trippoint(trippoint *s, trippoint *d, double w)
	{
		newlat=latitude = s->newlat + w*(d->newlat - s->newlat);
		newlon=longitude = s->newlon + w*(d->newlon - s->newlon);
		//ToDo Interpolate more parameters ?
	};

	trippoint::trippoint(trippoint *s)
	{
		newlat=latitude = s->newlat;
		newlon=longitude = s->newlon;
	};
	trippoint::~trippoint()
	{

	}
