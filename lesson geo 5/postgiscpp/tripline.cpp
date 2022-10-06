#include "umaobd.h"


tripline::tripline() {
};

tripline::~tripline()
{
	delete points;
}

void tripline::fill(int n)
{
	npoints = n;
	points = (trippoint **)new trippoint *[n];
}


bool tripline::insideCampus(int i)
{
	if (points[i]->longitude >= -4.455261)return false;
	if (points[i]->longitude <= -4.512359)return false;
	if (points[i]->latitude >= 36.736785)return false;
	if (points[i]->latitude <= 36.703857)return false;
	return true;
}


/// Calcula si el trayecto es de entrada, de salida, de paso
void tripline::classify() {
	if (npoints < 40)invalid = true;
	else {
		outgoing = insideCampus(0);
		ingoing = insideCampus(npoints - 1);
		if (verbose) {
			if (outgoing&&ingoing)printf("El track %3d con %5d puntos, es interior\n", id, npoints);
			if (!outgoing && !ingoing)printf("El track %3d con %5d puntos, es de paso\n", id, npoints);
			if (!outgoing && ingoing)printf("El track %3d con %5d puntos, entra\n", id, npoints);
			if (outgoing && !ingoing)printf("El track %3d con %5d puntos, sale\n", id, npoints);
		}
	}
}


/// Calcula la velocidad de GPS en base a desplazamiento y tiempo
void tripline::computeGPSspeed()
{
	if (invalid)return;
	double fptf, fptb, fpsf, fpsb; //weighting factor in space and time
	double df, db, vf, vb, tf, tb, v = 0;
	for (int i = 0; i < npoints; i++)
	{
		df = db = 0;
		//Velocidad entre este punto y el siguiente
		if (i != npoints - 1) {
			df = distance(*points[i + 1], *points[i]);
			tf = points[i + 1]->sample->time - points[i]->sample->time;
			v = vf = 3600 * df / tf;
		}
		//Velocidad entre este punto y el anterior
		if (i != 0) {
			db = distance(*points[i], *points[i - 1]);
			tb = points[i]->sample->time - points[i - 1]->sample->time;
			v = vb = 3600 * db / tb;
		}
		//Factor de ponderación
		if ((i != 0) && (i != npoints - 1)) {
			fptf = tb / (tf + tb);
			fpsf = db / (df + db);
			points[i]->time_weight = fptb = 1 - fptf;
			points[i]->space_weight = fpsb = 1 - fpsf;
			v = vf*fptf + vb*fptb; //¿Seguro? Si. Si no se mueve, spaceweight es indeterminado
		}


		points[i]->gpsspeed = v;


		double relative_diff_speed = 0;
		if ((points[i]->sample->vehspeed > 0) || (points[i]->gpsspeed > 0))
			relative_diff_speed = (fabs(points[i]->sample->vehspeed - points[i]->gpsspeed)
				/ points[i]->sample->vehspeed);

		if (points[i]->sample->hdop>10)points[i]->gps_ok = false;
		else
			if (points[i]->sample->vehspeed < 10)
				points[i]->gps_ok = (points[i]->gpsspeed < 20);
			else
				points[i]->gps_ok = relative_diff_speed < 0.4;
		if (points[i]->gps_ok) {
			points[i]->newlat = points[i]->latitude;
			points[i]->newlon = points[i]->longitude;
		}

		if (verbose)if (id == TESTTRACK)
			printf("%3dkmh \t%c%dm\t %3.0fkmh Diff: %3.f%% \tin %4dm\t%s\n",
			(int)points[i]->gpsspeed, 241,
				(int)points[i]->sample->hdop,
				points[i]->sample->vehspeed,
				relative_diff_speed * 100,
				(int)(df + db), (points[i]->gps_ok ? "" : "no"));
	}
}


/// Step 1, calculation of first and last valid points (inside campus)
void tripline::filterStep1()
{
	if (invalid)return;
	try {
		//Direct search
		int i = 0;
		for (i = 0; i < npoints; i++)
		{
			if (!points[i]->gps_ok)continue;
			if (!outgoing && !insideCampus(i))continue;
			first = i;
			break;
		}
		if (i == npoints) {
			invalid = true; return;
		}
		for (i = npoints - 1; i > first; i--)
		{
			if (!points[i]->gps_ok)continue;
			if (!ingoing && !insideCampus(i))continue;
			last = i;
			break;
		}
		if (i == first)invalid = true;
		if (last - first<10) invalid = true;
	}
	catch (const std::exception& e)
	{
		printf("%s\n", e.what());
		invalid = true;
	}
	if (!invalid)printf("First point: %4d, last point %4d\n", first, last);
	return;
}


// Used in filter step 2 (recovering bad GPS data)
void tripline::interpolation(int s, int d)
{
	//s: last preceding point with good data
	//d: first following point with good data
	double deltalat = (points[d]->latitude - points[s]->latitude) / (d - s);
	double deltalon = (points[d]->longitude - points[s]->longitude) / (d - s);
	for (int i = s + 1; i < d; i++)
	{
		points[i]->newlat = points[s]->latitude + (i - s)*deltalat;
		points[i]->newlon = points[s]->longitude + (i - s)*deltalon;
	}
	//TO DO
	/*
	The interpolation method here is based on equidistance in space, but it can be
	relatively easily corrected to proportional distance based on vehspeed and time
	*/

}


/// Step 2, Calculate and recovery of bad data sequences
void tripline::filterStep2()
{
	if (invalid)return;
	int j, i = first;
	for (int i = first; i <= last; i++)
	{
		j = i;
		while ((j <= last) && !points[j]->gps_ok)j++;
		if (i != j) {
			if (verbose)if (id == TESTTRACK)printf("Bad region from %d to %d \n", i, j - 1);
			interpolation(i - 1, j);
			i = j - 1;
		}
	}
	newdistance = 0;
	for (int i = first + 1; i <= last; i++)
		newdistance +=
		distance(points[i]->newlat, points[i - 1]->newlat, points[i]->newlon, points[i - 1]->newlon);
	if (verbose)printf("Distancia dentro del campus: %4.0f m\n", 1000 * newdistance);
}


/// Make new track from existing track
void tripline::maketrack256(tripline * newtrack)
{
	if (invalid)return;
	newtrack->npoints = 256;
	newtrack->points = (trippoint **)new trippoint *[256];
	double step = newdistance / 255;
	//Clonar primer y último punto
	newtrack->points[0] = new trippoint(points[first]);
	newtrack->points[255] = new trippoint(points[last]);
	int s = first;
	double d = 0;
	for (int i = 1; i < 255; i++) {
		double w = 0;
		//ToDo
		//Ir aumentando distancia hasta encontrar
		//Distancia del punto es i*step;
		//revisar
		while (d < i*step) {
			d += distance(points[s + 1]->newlat, points[s]->newlat, points[s + 1]->newlon, points[s]->newlon);
			s++;
		}

		newtrack->points[i] = new trippoint(points[s - 1], points[s], w);
		newtrack->points[i]->numorder = i;
	}
}
