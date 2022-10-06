#include "umaobd.h"

	trippoint::trippoint(double x, double y)
	{
		latitude = y;
		longitude = x;
	};
	trippoint::trippoint(trippoint *s, trippoint *d, double w)
	{
		latitude = s->newlat + w*(d->newlat - s->newlat);
		longitude = s->newlon + w*(d->newlon - s->newlon);
		//ToDo Interpolate more parameters ?
	};
	trippoint::trippoint(trippoint *s)
	{
		latitude = s->newlat;
		longitude = s->newlon;
	};
	trippoint::~trippoint()
	{

	}
