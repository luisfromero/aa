#pragma once
#include <stdio.h>
#include <math.h>
#include <exception>
#include <fstream>


#define TESTTRACK 844


struct sample
{
	int idtrip;
	double time;
	double co2perkm;
	double intakeairtemp;
	double litres100km;
	double vehspeed;
	double hdop;  //horizontal dilution of precision 
	double gpsbearing;
	double throttle;
};

class trippoint
{
public:
	//Basic information
	sample *sample; //From database
	int numorder;
	double latitude;
	double longitude;
	bool hasdata = false;   //Sample data found

	//Advanced information (after processig)
	bool gps_ok = true;      //Valid GPS measure
	double gpsspeed;		 //Computed GPS speed, considering GPS data
	double time_weight = 0;  //proximity to prior (1: very close to i-1)
	double space_weight = 0; // (0: very close to i+1)
	double newlat;           //Corrected GPS data (if no ok)
	double newlon;

	//Constructors
	trippoint(double lat, double lng);
	trippoint(trippoint *s, trippoint *d, double w); //From interpolation
	trippoint(trippoint *s);   //Copy
	~trippoint();
};


class tripline
{

public:
	int id;
	trippoint **points;
	double distance1;
	double distance2;
	int npoints;

	bool outgoing = false;
	bool ingoing = false;
	bool invalid = false;
	int first, last; //First and last ok points
	double newdistance;
	bool verbose = true;

	tripline();
	~tripline();
	void fill(int npoints);
	bool insideCampus(int pointindex);
	void classify();
	void interpolation(int source, int destination);
	void computeGPSspeed();
	void filterStep1();
	void filterStep2();
	void maketrack256(tripline * newtrack);
};

double distance(tripline *a, tripline *b);
double distance(trippoint b, trippoint a);
double distance(double latb, double lata, double lonb, double lona);
