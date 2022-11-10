//Reclaman headers adicionales pg_config_ext y postgres_ext
const char* conninfo = "dbname = mlearn user=ml_aa00 password=vc0910$$ host=obd.ac.uma.es";
PGconn* conn;
PGresult* result;
PGresult* result2;
//Connect to DB
bool finished = true;
PQprintOpt options;

gdalparser* parser = new gdalparser();

tripline** tracks;
tripline** tracks256;
tripline** trim_tracks;

sample* samples;
int nsamples;
int ntracks;

const char* getFullTracks = "select idtrip,st_astext(tripline),numsamples,length,distance from pract3_full order by idtrip asc";
const char* getTrimTracks = "select idtrip,st_astext(tripline) from pract3_trim order by idtrip asc";
const char* getSamples = "select * from pract3_samples order by trip_idtrip asc,time asc";


int mylat(double x) {
	uint64_t llat;
	memcpy(&llat, &x, sizeof llat);
	return (int)(1048576 + ((llat & 0x000FFFFF00000000L) >> 32));
	//1048576 es el bit implícito
	//Equivale a latitud multiplicada * 32768 y truncada a entero (~1203000)
	//Un grado son 111000m -> Error ~3.387m
}


int mylon(double x) {
	uint64_t llng;
	memcpy(&llng, &x, sizeof llng);
	return (int)(131072 + ((llng & 0x000FFFF800000000L) >> 35));
	//131072 es el bit implícito
	//Longitud multiplicada * 32768 y truncada a entero (~146800)
	//Un grado son 89350m -> Error ~2.726m}
}