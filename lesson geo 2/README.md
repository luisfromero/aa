# Ejercicios de la Lección 2 (y primera parte de la Lección 3)

Se muestran a continuación algunos Scripts de ejemplos de código pyQGIS (Python + QGIS)

* ## Elementary

Este [código](elementary.py) contiene las líneas básicas para obtener las referencias al proyecto, a las capas, y al canvas de QGIS, a partir de los cuales podremos manipular la información programáticamente

* ## Covid (Ejemplo vectorial del año 2020)

[Código](covid.py) que anima la evolución temporal de la pandemia utilizando los datos diarios por paises, procedentes de una capa vectorial existente.

<video width="320" height="240" controls>
  <source src="img\\covid_000.mp4" type="video/mp4">
</video>

La animación utiliza un temporizador:
```
timer = QTimer()
timer.timeout.connect(refresh)
timer.start(2000)
...
timer.stop()
```
En el código, dentro de la función ```refresh```, se actualiza la columna en la que están los datos que se renderizan en cada frame, con la columna del dato diario que se elige en bucle.

* ## Inundación de Málaga (Ejemplo Raster, años 2020/21)

En el archivo [inundamalaga.md](inundamalaga.md) puede encontrar los detalles. 

* ## Museos de Málaga (Ejemplo vectorial del año 2021)

En este caso, se genera desde cero una capa vectorial,  a partir de los datos de visitas en museos. Puede ver el [código](museos.py), en el que se destaca la creación desde cero de la capa vectorial, agregando, para cada _feature_ un par de campos (nombre y nº de visitas, además de las propiedades geográficas)

* ## Densidad de población de Málaga (Vectorial, 2022)

En este proyecto, combinamos datos no geolocalizados con una capa vectorial ya existente. Este tipo de operaciones resulta tremendamente útil. Por ejemplo, podríamos disponer de tablas de datos que indican la renta per cápita por municipio y por año. Si lo combinamos con una capa vectorial de los municipios de España, podríamos visualizar la evolución de la renta per cápita en España con ubna simple animación. 

<img src=img/composicion.png>

La descripción detallada se encuentra en un [markdown específico](densidadmalaga.md).

* ## Ejecutando scripts de PyQGIS desde fuera de la aplicación QGIS

Desde fuera de QGIS es posible acceder a la prática totalidad de las funcionalidades de la aplicación utilizando exclusivamente scripts Python. En [este ejercicio](standaloneProject.py) se presenta la forma de hacerlo. 

En general, los principales problemas que nos encontraremos están relacionados con las rutas de los intérpretes, lass librerías, etc. Por ello, es necesario que las variables de entorno se establezcan con los mismos valores de la última instalación de QGIS, que, en el caso de Windows, se almacena en C:\Program Files\QGIS 3.22.1\bin\qgis-bin.env

