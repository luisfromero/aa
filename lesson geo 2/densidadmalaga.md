# Densidad de población de los barrios de Málaga en 2021

En este ejercicio crearemos un mapa que muestre gráficamente la densidad de población en el municipio de Málaga. Para ello, utilizaremos las siguientes fuentes de datos del Ayuntamiento:

* [Cartografía de barrios (shapefile)](https://datosabiertos.malaga.eu/recursos/urbanismoEInfraestructura/planimetria/callejero/da_cartografiaBarrio-25830.zip)
* [Datos censales por barrio, en 2021 (csv)](https://datosabiertos.malaga.eu/dataset/padron-de-habitantes-por-barrios-2021)

Establecemos el SRC de la capa en 25830, ya que vamos a trabajar a nivel municipal. 

Tras cargar las dos capas en QGIS, consultamos sus datos y observamos su estructura. 

En la primera capa, observamos cómo cada barrio de la primera capa tiene su identificador numérico. Con la calculadora de campos, agregamos un campo virtual denominado superficie, utilizando la expresión area($geometry), en lugar de $area, ya que no consideraremos la esfericidad de la tierra.

Los datos censales tienen una fila por cada habitante de la ciudad, siendo un ejemplo de una capa no geolocalizada (en realidad es una simple tabla de datos). Interesa especialmente el campo BARRIO, que usaremos para contar los habitantes como consulta SQL:

## Cálculo de los habitantes por barrio

Añadimos una nueva capa virtual a través del menú con la siguiente consulta:

```SELECT BARRIO, COUNT(BARRIO) as 'habitantes' from padronbarrios Group by BARRIO```


Asignamos el nombre _habitantes_ a la capa. Esta capa es en realidad una reducción de los datos de la capa anterior para proporcionar el número de habitantes por barrio.

<sub>Debido a un bug en la versión 3.22, la capa virtual se ha creado de una forma alternativa, mediante la consola python:

```
query="SELECT BARRIO, COUNT(BARRIO) as 'habitantes' from padronbarrios Group by BARRIO"
from qgis.core import QgsVectorLayer, QgsProject
habit = QgsVectorLayer("?query={}".format(query), "habitantes", "virtual" )
QgsProject.instance().addMapLayer(habit)
```

</sub>

## Unión de tablas

A continuación vamos a unir (JOIN) la tabla de habitantes por barrio a la tabla de datos de la cartografía de barrios.

Para ello, abrimos las propiedades de la capa cartográfica, y buscamos "Uniones" o "Join". Hacemos click en el símbolo + y asociamos la la capa habitantes (campo BARRIO) al campo objetivo NUMBARRIO. Esperamos unos segundos, y ya tenemos las dos tablas unidas, como comprobamos en la tabla de atributos.

## Visualización

Para visualizar el mapa de densidad de población, utilizaremos la simbología graduada en el mapa de la cartografía, según se muestra en la imagen.

<img src=img/rampa.png  width="400">

El mapa resultante, al que hemos agregado una leyenda, en la composición de impresiones de QGIS es:

<br>


<img src=img/composicion.png>

<br>


<img src=img/detalle.png>
