// postgiscpp.cpp: define el punto de entrada de la aplicación de consola.
//
// Compilar con libpq.lib gdal_i.lib y los headers (disponibles en la carpeta include y lib de QGIS 3.8)
// Agregar path de los dll (debugger environment PATH=c:\PROGRA~1\QGIS3~1.8\bin;%PATH% )

#include "umaobd.h"
#include "gdalparser.h"
#include "libpq-fe.h"
#include "ogrsf_frmts.h"
#include "postgiscpp.h"



void error(char * mensaje)
{
	fprintf(stderr, mensaje , PQerrorMessage(conn));
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
	options.fieldSep = "|";  /* Use a pipe as the field separator*/
	conninfo = "dbname = umaobd user=obd_readpriv password=vc0910$$ host=atomic3.dmz.ac.uma.es";
	conninfo = "dbname = mlearn user=ml_aa00 password=vc0910$$ host=obd.ac.uma.es";
	//conninfo = "dbname = umaobd user=obd_readpriv password=vc0910$$ host=obd.ac.uma.es";
}

void parseLS(int rec_count, bool full)
{
	char *tmp;
	char *linestring;
	tripline **trip = tracks;
	if (!full)trip = trimmed;
	for (int i = 0; i < rec_count; i++) {
		linestring = PQgetvalue(result, i, 1);
		trip[i] = new tripline();
		tmp = PQgetvalue(result, i, 0);
		sscanf(tmp, "%d", &trip[i]->id);
		parser->parseGeometry((unsigned char *)linestring, trip[i]);
		if (full)
		{
			tmp = PQgetvalue(result, i, 2);
			sscanf(tmp, "%d", &trip[i]->npoints);
			tmp = PQgetvalue(result, i, 3);
			sscanf(tmp, "%lf", &trip[i]->distance1);
			tmp = PQgetvalue(result, i, 4);
			sscanf(tmp, "%lf", &trip[i]->distance2);
		}
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


		query((char *)getFullTracks, false);
		//PQprint(stdout, result, &options);
		ntracks = PQntuples(result);
		tracks = (tripline **) new tripline *[ntracks];
		parseLS(ntracks,true);
		PQclear(result);

		query((char *)getTrimTracks, false);
		//PQprint(stdout, result, &options);
		if(ntracks != PQntuples(result)) exit(1);
		trimmed = (tripline **) new tripline *[ntracks];
		parseLS(ntracks, false);
		PQclear(result);

		query((char *)getSamples, false);
		nsamples= PQntuples(result);
		samples = new sample[nsamples];
		parseSamples(nsamples);
		PQclear(result);
	}
}


void attachsampletotrackpoint()
{
	int cnt = 0;
	int pcnt = 0;
	int idtrip;
	for (int i = 0; i < nsamples; i++) {
		idtrip = samples[i].idtrip;
		while (tracks[cnt]->id != idtrip) {
			//printf("Track %d: Procesados %d, deberían ser %d \n", idtrip, pcnt, trayectos[cnt]->npuntos);
			cnt++; 
			pcnt = 0; }
		tracks[cnt]->points[pcnt++]->sample =& samples[i];
		}
	//printf("Track %d: Procesados %d, deberían ser %d \n", idtrip, pcnt, trayectos[cnt]->npuntos);
}

/// Not Used, because trimmed tracks  fails
void aligntrimmedtracks()
{
	for (int i = 0; i < ntracks; i++)
	{
		int nt = trimmed[i]->npoints;
		int nf = tracks[i]->npoints;
		if(nt== nf)
			printf("El track %d es interior\n", trimmed[i]->id);
		else
		{
			double inilatf = tracks[i]->points[0]->latitude;
			double inilonf = tracks[i]->points[0]->longitude;
			double inilatt = trimmed[i]->points[0]->latitude;
			double inilont = trimmed[i]->points[0]->longitude;
			double endlatf = tracks[i]->points[nf-1]->latitude;
			double endlonf = tracks[i]->points[nf - 1]->longitude;
			double endlatt = trimmed[i]->points[nt - 1]->latitude;
			double endlont = trimmed[i]->points[nt - 1]->longitude;
			bool idemini = (inilatf== inilatt) && (inilonf == inilont);
			bool idemend = (endlatf == endlatt) && (endlonf == endlont);
			if(idemini&&!idemend)printf("El track %d es saliente\n", trimmed[i]->id);
			if (!idemini && idemend)printf("El track %d es entrante\n", trimmed[i]->id);
			if (!idemini && !idemend)
				printf("El track %d es de paso\n", trimmed[i]->id);
			if (idemini && idemend)
				printf("El track %d es extraño (nt=%d, nf=%d)\n", trimmed[i]->id, nt, nf);

		}

	}
}

int main()
{
	readdb();
	attachsampletotrackpoint();
	tracks256 = (tripline **) new tripline *[ntracks];
	int a = mylat((double)35.78);
	int b = mylon((double)-4.23);
	FILE *f = fopen("tracks.geojson", "w");
	for (int i = 0; i < ntracks; i++)
	{
		tracks256[i] = new tripline[256];
		tracks[i]->classify();
		tracks[i]->computeGPSspeed();
		tracks[i]->filterStep1();
		tracks[i]->filterStep2();
		tracks[i]->maketrack256(tracks256[i]);
		char *wkt=parser->exportGeometry(tracks256[i]);
		fprintf(f,"%s\n",wkt);
	}
	fclose(f);
	//aligntrimmedtracks();
	delete tracks;
	delete tracks256;
	delete samples;
    return 0;
	//94037 muestras
	//select count(*) from tripsample where (idtrip )

}

