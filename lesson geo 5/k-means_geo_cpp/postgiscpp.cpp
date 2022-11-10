// postgiscpp.cpp: define el punto de entrada de la aplicación de consola.
//
// Para la parte postgresql, instalar en windows, con OsGeo4W setup, la 
// 
// Para la parte gdal, elegir gdal-dev-devel (instala como paquete adicional la ultima dll, que en 2022 es gdal-dev307)
// C:\OSGeo4W\apps\gdal-dev\bin es el path
// C:\OSGeo4W\bin tambien es necesario en el path
// Compilar con libpq.lib gdal_i.lib que los encontrará en los directorios adicionales de libreria
// 
// Agregar path de los dll (debugger environment PATH= [ver])

#include <iostream>
#include "umaobd.h"
#include "gdalparser.h"
#include <libpq-fe.h>
#include <ogrsf_frmts.h>
#include "postgiscpp.h"

tripline* kmeans(tripline** tracks);


void error(const char* mensaje)
{
	fprintf(stderr, mensaje, PQerrorMessage(conn));
	PQclear(result); //Nulo si no hay conexión, pero no da error
	PQfinish(conn);
	finished = true;
}

void query(char * comando, bool clear)
{
	if (finished)return;
	result = PQexec(conn, comando);

	ExecStatusType x = PQresultStatus(result);
	if((x!= PGRES_TUPLES_OK)&&(x != PGRES_COMMAND_OK)) {
		fprintf(stderr, comando);
		error(" failed: %s\n");
		return;
		}
	if(clear)PQclear(result);
}

void prepare()
{
	options = { 0 };
	options.header = 1;    /* Ask for column headers           */
	options.align = 1;    /* Pad short columns for alignment  */
//	options.fieldSep = "|";  /* Use a pipe as the field separator*/
	options.fieldSep = const_cast <char*>("|");  /* Use a pipe as the field separator*/
	//conninfo = "dbname = umaobd user=obd_readpriv password=vc0910$$ host=atomic3.dmz.ac.uma.es";
	conninfo = "dbname = mlearn user=ml_aa00 password=vc0910$$ host=obd.ac.uma.es";
	//conninfo = "dbname = umaobd user=obd_readpriv password=vc0910$$ host=obd.ac.uma.es";
}

void parseLS(int rec_count, tripline** trip, int mode)
{
	char* tmp;

	for (int i = 0; i < rec_count; i++) {
		tmp = PQgetvalue(result, i, 1);
		trip[i] = new tripline();
		parser->parseGeometry((unsigned char*)tmp, trip[i]);
		tmp = PQgetvalue(result, i, 0);
		sscanf_s(tmp, "%d", &trip[i]->id);
		if (mode == 1)continue;
		tmp = PQgetvalue(result, i, 2);
		sscanf_s(tmp, "%d", &trip[i]->npoints);
		tmp = PQgetvalue(result, i, 3);
		sscanf_s(tmp, "%lf", &trip[i]->distance1);
		tmp = PQgetvalue(result, i, 4);
		sscanf_s(tmp, "%lf", &trip[i]->distance2);
	}
}

void parseSamples(int rec_count)
{
	char *tmp;
	for (int i = 0; i < rec_count; i++) {
		tmp = PQgetvalue(result, i, 1);
		sscanf(tmp, "%d", &samples[i].idtrip);
		tmp = PQgetvalue(result, i, 2);
		sscanf(tmp, "%lf", &samples[i].time);
		//parser->parseGeometry((unsigned char *)point,&trayectos[0]);
		tmp = PQgetvalue(result, i, 3);
		sscanf(tmp, "%lf", &samples[i].co2perkm);
		tmp = PQgetvalue(result, i, 4);
		sscanf(tmp, "%lf", &samples[i].intakeairtemp);
		tmp = PQgetvalue(result, i, 5);
		sscanf(tmp, "%lf", &samples[i].litres100km);
		tmp = PQgetvalue(result, i, 6);
		sscanf(tmp, "%lf", &samples[i].vehspeed);
		tmp = PQgetvalue(result, i, 7);
		sscanf(tmp, "%lf", &samples[i].hdop);
		tmp = PQgetvalue(result, i, 8);
		sscanf(tmp, "%lf", &samples[i].gpsbearing);
		tmp = PQgetvalue(result, i, 9);
		sscanf(tmp, "%lf", &samples[i].throttle);

	}
}

int readdb()
{
	prepare();
	conn = PQconnectdb(conninfo);
	ConnStatusType pqs=PQstatus(conn);
	if (pqs != CONNECTION_OK) {
		error("Connection to database failed: %s"); return 0;
	}
	else
	{
		finished = false;
		printf("Connection Ok\n");


		query((char*)getFullTracks, false);
		//PQprint(stdout, result, &options);
		ntracks = PQntuples(result);
		if (!ntracks)return 0;
		tracks = (tripline**) new tripline * [ntracks];
		parseLS(ntracks, tracks, 0);
		PQclear(result);

		query((char*)getTrimTracks, false);
		//PQprint(stdout, result, &options);
		ntracks = PQntuples(result);
		if (!ntracks)return 0;
		trim_tracks = (tripline**) new tripline * [ntracks];
		parseLS(ntracks, trim_tracks, 1);
		PQclear(result);

		query((char*)getSamples, false);
		nsamples = PQntuples(result);
		samples = new sample[nsamples];
		parseSamples(nsamples);
		PQclear(result);
		return 1;
	}
}


void attachsampletotrackpoint()
{
	int cnt = 0;
	int pcnt = 0;
	int idtrip;
	for (int i = 0; i < nsamples;i++) {
		//Descartamos los tracks que no tengan muestras
		idtrip = samples[i].idtrip;
		while(tracks[cnt]->id!= idtrip)
			{
			if (pcnt != 0)
			{
				printf("Track %d: Procesados %d, pero deben ser %d \n", idtrip, pcnt, tracks[cnt]->npoints);
				fflush(stdout);
			}
			else tracks[cnt]->invalid = true;
			cnt++;
			pcnt = 0;
			}
		
		tracks[cnt]->points[pcnt]->sample = &samples[i];
		tracks[cnt]->points[pcnt]->hasdata = true;
		pcnt++;

		}
		
}

/// 
void aligntrimmedtracks()
{
	for (int i = 0; i < ntracks; i++)
	{
		int nt = trim_tracks[i]->npoints;
		int nf = tracks[i]->npoints;
		if(nt== nf)
			printf("El track %d es interior\n", trim_tracks[i]->id);
		else
		{
			double inilatf = tracks[i]->points[0]->latitude;
			double inilonf = tracks[i]->points[0]->longitude;
			double inilatt = trim_tracks[i]->points[0]->latitude;
			double inilont = trim_tracks[i]->points[0]->longitude;
			double endlatf = tracks[i]->points[nf-1]->latitude;
			double endlonf = tracks[i]->points[nf - 1]->longitude;
			double endlatt = trim_tracks[i]->points[nt - 1]->latitude;
			double endlont = trim_tracks[i]->points[nt - 1]->longitude;
			bool idemini = (inilatf== inilatt) && (inilonf == inilont);
			bool idemend = (endlatf == endlatt) && (endlonf == endlont);
			if(idemini&&!idemend)printf("El track %d es saliente\n", trim_tracks[i]->id);
			if (!idemini && idemend)printf("El track %d es entrante\n", trim_tracks[i]->id);
			if (!idemini && !idemend)
				printf("El track %d es de paso\n", trim_tracks[i]->id);
			if (idemini && idemend)
				printf("El track %d es extraño (nt=%d, nf=%d)\n", trim_tracks[i]->id, nt, nf);

		}

	}
}

int main()
{
	if(!readdb())return -1;
	attachsampletotrackpoint();
	tracks256 = (tripline **) new tripline *[ntracks];
	int a = mylat((double)35.78);
	int b = mylon((double)-4.23);
	int contadorValidos = 0;
	FILE *f = fopen("tracks.geojson", "w");
	for (int i = 0; i < ntracks; i++)
	{
			tracks[i]->classify();
			tracks[i]->computeGPSspeed();
			tracks[i]->filterStep1();
			tracks[i]->filterStep2();
			if (!tracks[i]->invalid) {
				tracks256[contadorValidos] = new tripline();// tripline[256];
				tracks[i]->maketrack256(tracks256[contadorValidos]);
				char* wkt = parser->exportGeometry(tracks256[contadorValidos]);
				fprintf(f, "%s\n", wkt);
				contadorValidos++;
			}
	}
	fclose(f);
	//aligntrimmedtracks();
	
	f = fopen("central.geojson", "w");
	tripline* centro = kmeans(tracks256);
	char* wkt = parser->exportGeometry(centro);
	fprintf(f, "%s\n", wkt);
	fclose(f);
	printf("Distancia entre 1 y 4 es %d metros\n", (int)distance(tracks256[2], tracks256[5]));
	printf("Distancia entre 1 y 2 es %d metros\n", (int)distance(tracks256[2], tracks256[3]));
	printf("Distancia entre 1 y C es %d metros\n", (int)distance(tracks256[2], centro));
	printf("Distancia entre 0 y 3 es %d metros\n", (int)distance(tracks256[0], tracks256[4]));
	printf("Distancia entre 0 y C es %d metros\n", (int)distance(tracks256[0],centro));
	delete tracks;
	delete tracks256;
	delete samples;
    return 0;
	//94037 muestras
	//select count(*) from tripsample where (idtrip )

}

