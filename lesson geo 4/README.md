# Lesson 4. PostGIS in depth

En la lección 3 aprendimos los conceptos básicos de una base de datos geográfica, y en particular, de la extensión GIS de postgresql. El objetivo de esta lección es profundizar en el conocimiento de la base de datos. Pero antes de empezar, repasamos algunos aspectos de la base de datos.



## Una base de datos para practicar:

Para acceder a la base de datos, necesitará los siguientes datos de conexión:

Servidor: obd.ac.uma.es
Usuarios: ml_aa00, ml_aa01
<<<<<<< HEAD
Password: ******** (preguntar)
=======
Password: vc0910**
>>>>>>> 6322a9e3c5698c24018411baeb2325436b3e9ff9
Database: mlearn

Puede evitar la solicitud de contraseña mediante la creación de un archivo /users/username/.pgpass con tantas líneas como conexiones desee, siendo cada linea de la forma:

<<<<<<< HEAD
```obd.ac.uma.es:5432:mlearn:ml_aa02:********```
=======
```obd.ac.uma.es:5432:mlearn:ml_aa02:vc0910**```
>>>>>>> 6322a9e3c5698c24018411baeb2325436b3e9ff9

(en gitignore hay un ejemplo)

El archivo debe tener permisos 0600:

```sh
chmod 0600 .pgpass
```


Establezca una conexión con la base de datos mediente el front-end interactivo para base de datos PostgreSQL

```psql -h obd.ac.uma.es -U ml_aa00 –d mlearn```



## Comandos psql

Una vez dentro de la ventana de comandos de psql, podemos ejecutar consultas a la base de datos, o bien comandos de la propia aplicación:


- `\?`: Show help (list of available commands with an explanation)
- `\q`: Quit/Exit
- `\c __database__`: Connect to a database
- `\l`: List databases
- `\d __table__`: Show table definition (columns, etc.) including triggers
- `\d+ __table__`: More detailed table definition including description and physical disk size

Most `\d` commands support additional param of `__schema__.name__` and accept wildcards like `*.*`


- `\dy`: List events
- `\df`: List functions
- `\di`: List indexes
- `\dn`: List schemas
- `\dt *.*`: List tables from all schemas (if `*.*` is omitted will only show SEARCH_PATH ones)
- `\dT+`: List all data types
- `\dv`: List views
- `\dx`: List all extensions installed
- `\df+ __function__` : Show function SQL code. 
- `\x`: Pretty-format query results instead of the not-so-useful ASCII tables
- `\copy (SELECT * FROM __table_name__) TO 'file_path_and_name.csv' WITH CSV`: Export a table as CSV
- `\des+`: List all foreign servers
- `\dE[S+]`: List all foreign tables
- `\! __bash_command__`: execute `__bash_command__` (e.g. `\! ls`)

User Related:
- `\du`: List users
- `\du __username__`: List a username if present.
- `create role __test1__`: Create a role with an existing username.
- `create role __test2__ noinherit login password __passsword__;`: Create a role with username and password.
- `set role __test__;`: Change role for current session to `__test__`.
- `grant __test2__ to __test1__;`: Allow `__test1__` to set its role as `__test2__`.
- `\deu+`: List all user mapping on server




## Las tablas de las bases de datos de las prácticas

En las prácticas, utilizaremos fundamentalmente tres tablas:

* Calles de Málaga (streetlines)
* Viajes de vehículos (trip)
* Muestras durante un viaje (tripsample)

Antes de continuar con la práctica, consultemos su estructura (\d trip) y visualizaremos, mediante cuatro técnicas diferentes, los datos de las tablas:



### Modo 1: Visualización elemental, en modo texto (psql)

select idtripsample,samplelocation,time from tripsample where time > timestamp '2019-12-11';

select idtripsample,st_astext(samplelocation),time from tripsample where time > timestamp '2019-12-11';

<img src='img/psql.jpg' width=400 style="display: block; margin: 0 auto">

Como se observa, hemos introducido un operador nuevo (st_astext). Este tipo de operadores espaciales (st procede de Spatial-Type) son funciones disponibles al incorporar la extensión GIS. Veremos algunos de ellos en los siguientes ejemplos.


### Modo 2: Visualización mediante pgadmin

Visualizamos los datos en las tablas mediante la selección de la tabla y la ejecución de una consulta. Como primera opción usaremos la opción view/edit data de una tabla como inputdata.tripsample. En la tabla, localizamos la columna geométrica, y veremos un icono a la izquierda del nombre que permite visualizar la geometría.

<img src='img/pgadmin1.jpg' width=400 style="display: block; margin: 0 auto">

Después, ejecutaremos la siguiente consulta mediate la opción _query tool_ de la tabla:

```sql
select idtripsample,st_setsrid(samplelocation,4326),time from tripsample where time > timestamp '2019-12-11' and st_distance(st_setsrid(samplelocation,4326),st_GeogFromText('SRID=4326;POINT(-4.476 36.715)')) < 300;
```

Al visualizar la geometría, en este caso, aparece una capa OpenStreetMap de fondo, que es una consecuencia de que la segunda columna tiene establecido un SRC, permitiendo geoproyectarla correctamente. Comprobamos asimismo que los datos visualizados se corresponde con un trayecto a menos de 300 metros del punto _SRID=4326;POINT(-4.476 36.715)_ (situado en el centro del aparcamiento del Complejo Tecnológico)

<img src='img/pgadmin2.jpg' width=400 style="display: block; margin: 0 auto">




### Modo 3: Visualización con QGIS

En este caso, nos bastaría con agregar la conexión a la base de datos en el Panel Browser, introduciendo los [parámetros](#una-base-de-datos-para-practicar), y arrastrar la tabla geográfica que nos interese al lienzo, o al panel de capas. La única tabla correctamente georeferenciada (con un SCR de referencias preestablecido es _streetlines_), por lo que será la que usemos en este caso como primer ejemplo.


<img src='img/qgispostgis.jpg' width=400 style="display: block; margin: 0 auto">




### Modo 3: Visualización con DB Manager de QGIS

Finalmente, instalaremos el plugin de QGIS denominado DB Manager, y veremos que aparece una opción en el menú principal para realizar operaciones sobre una base de datos.

<img src='img/dbmanager.jpg' width=400 style="display: block; margin: 0 auto">




# Consultas básicas con POSTGIS

Antes de adentrarnos en algunas interioridades propias de la componente QGIS de PostGIS, recordaremos algunos conceptos generales de las bases de datos:

Y no está de más hacer un backup previo...

pg_dump -a  -h obd.ac.uma.es -U ml_admin  -d mlearn > archivo.sql

Empecemos por lo más básico (create, drop, select)

```sql
create USER pepe WITH password 'juan';
drop USER pepe;
create database midb;
drop database midb;
create table tabla as select idtrip as id from trip;
select * from tabla;
drop table tabla;
```
Ahora, unos operadores elementales (promedio, suma):

```sql
select avg(distance) from trip;
select idtrip+distance from trip;
select 'id= ' || idtrip::varchar from trip;
```

## Extensión GIS a PostgreSQL

PostGIS gives you a choice of two different ways to store geospatial data: 

* Geometry, where it assumes all of your data lives on a Cartesian plane (like a map projection); 
* Geography, where it assumes that your data is made up of points on the earth's surface, as specified by latitudes and longitudes

En realidad, las geometrías ya existían en PostgreSQL sin la extensión GIS.



Veamos algunos operadores básicos de PostGIS, y comparamos las diferencias entre los dos comandos:


```sql
SELECT idtrip,ST_Length(tripline),distance from trip where starttime > timestamp '2019-09-11' ;
SELECT idtrip,ST_Length(st_setsrid(tripline,4326)::geography),distance from trip where starttime > timestamp '2019-09-11' ;
```


If the point coordinates are not in a geodetic coordinate system (such as WGS84), then they must be reprojected before casting to a geography.

```sql
SELECT ST_Transform(ST_SetSRID( ST_Point( 3637510, 3014852 ), 2273), 4326)::geography;
```

En PostGIS, el sistema de referencias es obligatorio tanto en datos geométricos como geográficos. Si no se establece, se le asigna SRID=0 (desconocido) con resultados impredecibles, especialmente en el caso de datos geográficos.

### Operadores espaciales básicos

El manual de referencia de PostGIS incorpora una [lista extendida](https://postgis.net/docs/reference.html) de operadores espaciales. Algunos de ellos, ya los hemos visto "de refilón". Sin ambargo, existe una infinidad de ellos. 

Es importante saber que los operadores tienen diferentes comportamientos, según el tipo de datos del operando. Así, por ejemplo:

* st_length(geometry line), cuando las coordenadas de la geometría están en grados/metros/... , devuelve la longitud de la curva en grados/metros/...
* st_length(geography line), utiliza datos con un SRC, y devuelve los resultados en metros, considerando el esferoide de referencia.

### Operadores espaciales más complejos:

El siguiente comando, st_makeenvelope, genera un objeto geográfico (un polígono rectangular) a partir de dos esquinas opuestas y un SCR. Las coordenadas elegidas, si el sistema es WGS84, coinciden con los extremos de un rectángulo que encierra al Campus de Teatinos. 

```sql
CREATE TABLE candidatos AS SELECT idtrip,distance,ST_Intersection(tripline, ST_MakeEnvelope(-4.51236,36.70386 , -4.45526,36.73679, 4326)::geography) FROM trip WHERE  ST_Intersects(tripline, ST_MakeEnvelope(-4.51236,36.70386 , -4.45526,36.73679, 4326)::geography);
```
Descifra el resultado, observando especialmente la sutil diferencia entre un comando que pregunta cierto/falso por una intersección, y otro comando que devuelve la intersección propiamente dicha.