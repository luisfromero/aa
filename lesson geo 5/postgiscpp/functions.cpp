#include "umaobd.h"
#include <math.h>

double distance(trippoint b, trippoint a)
{
	double d1 = pow(a.latitude - b.latitude, 2);
	double d2 = pow(a.longitude - b.longitude, 2);
	//110972.38m por grado de lat a 36.78º
	//89336.96m por grado de lon a 36,78º
	d1 *= 110.97238*110.97238;
	d2 *= 89.33696*89.33696;
	return sqrt(d1 + d2);
}
double distance(double latb, double lata, double lonb, double lona)
{
	double d1 = pow(lata - latb, 2);
	double d2 = pow(lona - lonb, 2);
	d1 *= 110.97238*110.97238;
	d2 *= 89.33696*89.33696;
	return sqrt(d1 + d2);
}


