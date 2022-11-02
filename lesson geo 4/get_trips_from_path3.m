%Set preferences with setdbprefs.
setdbprefs('DataReturnFormat', 'dataset');
setdbprefs('NullNumberRead', 'NaN');
setdbprefs('NullStringRead', 'null');

%Make connection to database.  Note that the password has been omitted.
%Using ODBC driver.
%conn = database('umaobd-analysis-matlab64', '', '');
conn = database('umaobd-input-matlab64', '', '');
conn2 = database('readpriv', '', '');

%Read data from database.
comm = ['SELECT 	idtripsample as idsample'...
    ' ,	vehspeed as speedkph'...
    ' ,	gpsbearing as heading'...
    ' ,	rpm as rpm'...
    ' ,	trip_idtrip as idtrip'...
    ' ,	altitude as altitude'...
    ' ,	latitude as lat'...
    ' ,	longitude as long'...
    ' ,	maf as maf, dfuel as usedfuel_litre, l100km as fuel_lp100km'...
    ' ,	date_part(''epoch'', time) as timestamp'...
    ' , date_part(''dow'',(time)) + date_part(''hour'',(time)) / 24.0 + date_part(''minute'',(time)) / (60.0*24.0) + date_part(''second'',(time)) / 86400.0  as weekoffset_days '...
    ' FROM 	"umaobd"."public".tripsamples_path'...
    ' WHERE time > TIMESTAMP ''2018-07-01'' '];

curs = exec(conn, comm);
curs = fetch(curs);
samples = curs.Data;
%close(curs);

comm = ['SELECT min(trip_idtrip) as idtrip,'...
	'max(nsamples) as sample_cnt,'...
	'min(starttime) as start_time,'...
	'min(vehspeed) as minspeed,'...
	'max(vehspeed) as maxspeed,'...
	'max(toffset) as tot_time,'...
	'sum(dspace) as tot_space,'...
	'sum(dfuel) as tot_fuel_l,'...
	'sum(dfuel)/sum(dspace) as fuel_lpermeter,'...
    'date_part(''dow'',MIN(starttime)) + date_part(''hour'',MIN(starttime)) / 24.0 as weekoffset_days, '...
	'date_part(''dow'',MIN(starttime)) as day_of_week,'...
	'date_part(''hour'',MIN(starttime)) as hour_of_day '...
    'FROM tripsamples_path '...
    'WHERE time > TIMESTAMP ''2018-07-01'' '...
    'GROUP BY trip_idtrip'
];

%Read data from database.
curs = exec(conn, comm);
curs = fetch(curs);
%tripsummary = curs.Data;
%close(curs);

tripsummary = curs.Data;
close(curs);


%Close database connection.
close(conn);
%Clear variables
curs2 = exec(conn2, ['select * from candidatos;']);
curs2 = fetch(curs2);
trips = curs2.Data;
close(curs2);


clear curs conn comm conn2 curs2;

% 3d speed map
figure;
colormap(autumn);
h=scatter3(samples.long,samples.lat,samples.speedkph,2,samples.speedkph);
set(gca,'CLim',[0 50]);
zlim([0 50]);
colorbar;
view([-32 32]);
grid('on');
box('on');
set (gca,'Color',[0.5 0.5 0.5]);
set (gca,'XColor',[0 0 0]);
set (gca,'YColor',[0 0 0]);
set (gca,'ZColor',[0 0 0]);
hold on;
scatter(samples.long,samples.lat,1);

% 3d fuel map
figure;
colormap(autumn);
h=scatter3(samples.long,samples.lat,samples.fuel_lp100km,2,samples.fuel_lp100km);
set(gca,'CLim',[0 100]);
zlim([0 100]);
colorbar;
view([-32 32]);
grid('on');
box('on');
set (gca,'Color',[0.5 0.5 0.5]);
set (gca,'XColor',[0 0 0]);
set (gca,'YColor',[0 0 0]);
set (gca,'ZColor',[0 0 0]);
hold on;
scatter(samples.long,samples.lat,1);

% Histogram total fuel usage
figure;
histogram(tripsummary.tot_fuel_l, 0:0.025:1, 'Facecolor',[0.4 0.4 0.4]);
title('Histograma de consumo total de carburante facultad->autovía (L)');
xlabel('Consumo total (L)');
ylabel('Frecuencia');
hold on;
histogram(tripsummary.tot_fuel_l, 0:0.025:1, 'Edgecolor','red','Normalization','cdf','DisplayStyle','stairs');
legend('Histograma','Densidad Acumulada');

% Histogram total time
figure;
histogram(tripsummary.tot_time, 0:15:330, 'Facecolor',[0.0 0.7 0.0]);
title('Tiempo total facultad->autovía (L)');
xlabel('Tiempo total (s)');
ylabel('Frecuencia');
hold on;
histogram(tripsummary.tot_time, 0:15:330, 'Edgecolor','red','Normalization','cdf','DisplayStyle','stairs');
legend('Histograma','Densidad Acumulada');
