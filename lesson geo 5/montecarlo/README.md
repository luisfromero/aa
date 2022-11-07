# Radiación solar en los tejados de Málaga mediante un método de MonteCarlo

Cálculo LIDAR GIS Monte Carlo de áreas sombreadas basado en el posicionamiento del sol python ephum

## Objetivo

Cálculo de la radiación solar en los superficies de los tejados de los edificios de la ciudad de Málaga.

En este [video](./radiacionMalaga.avi) puede ver cómo cambian las sombras de la zona del puerto de Málaga en función de las horas en tres días consecutivos (21 diciembre, 21 de marzo y 21 de junio).

Con esta información se puede obtener información concreta para instalar paneles solares bien orientados.

<br/>
<p style="text-align: center;">
[<img src='img/radiacion.jpg' width=600>]
</p>


## Metodología

Con los datos del mapa de altura LIDAR, podemos predecir cómo se bloqueará la luz del sol en un momento determinado del día. Este script elige muestras aleatorias del mapa y trata cada muestra como un reloj solar, rastreando la proyección de la muestra a lo largo de la región geográfica (según la dirección del sol en una fecha y hora específicas). Un punto está "sombreado" (de color negro en las imágenes a continuación) si esta proyección es más alta que la altura del edificio en ese punto. El algoritmo tiene aplicaciones potenciales en la colocación de paneles solares urbanos, calculando qué techos tienen una buena exposición al sol.

## Datos de origen

Archivo con el modelo de edificaciones del Centro Nacional de Información Geográfica a [escala 2.5 metros.](http://centrodedescargas.cnig.es/CentroDescargas/catalogo.do?Serie=MDT02#). El Modelo digital de superficies normalizado de la clase edificación correspondiente a la 1ª Cobertura con paso de malla de 2,5 metros se ha obtenido con un vuelo LiDAR en el año 2015. Se ha descargado el archivo en formato ASC, proyectado en ETRS89, UTM huso 30, para la extensión de la hoja 1053 del IGN.

## Código

El código completo puede consultarse [aquí](./radiacionMonteCarlo.py). Sin embargo, la función crítica, que calcula la sombra de cada muestra lanzada, se muestra y describe en esta función.

```py
def generateBitmask(buildingMap, datetime):
  # A shadow map Bitmask represents a map defined for each point in the plane
  # that defines WHETHER an object standing at that point
  # will be shaded by the occulting structure(s) around it.
  ground = buildingMap.min()
  shadowMap = numpy.ones((1000,1000))
  az, alt = datetime2azalt(datetime)
  azDispl = numpy.array([1,-numpy.cos(az)/numpy.sin(az)])
  altDispl = numpy.tan(alt)
  # generate random numbers on a subset of the image,
  # attempting to avoid problems with out of range indicies
  # low and high define the internal bounding box
  # size is (number of desired samples, 2)
  NSamples = 999999
  randomSample = numpy.random.random_integers(0, 999, (NSamples,2))
  for sample in range(0,NSamples-1):
    # Array points are used for mathematics
    # Tuple points are used for indexing arrays
    arrayPoint = randomSample[sample]
    tuplePoint = arrayPoint[0],arrayPoint[1]
    # Find the height at randomCoord
    shadowH = buildingMap[tuplePoint]
    while(shadowH > ground):
      # Track the shadow across the points it intersects
      # After each step, the shadow shortens
      shadowH -= altDispl
      arrayPoint += azDispl
      # buildingMap must be indexed by a tuple of integers
      tupleinterpolated = int(round(arrayPoint[0])),int(round(arrayPoint[1]))
      if(0<=tupleinterpolated[0]<1000 and 0<tupleinterpolated[1]<1000):
        if(shadowH > buildingMap[tupleinterpolated]):
          shadowMap[tupleinterpolated] = 0
  return shadowMap

```

## Resultados

A continuación se muestran los resultados para 10000, 100000 y 1000000 de lanzamientos:

<img src='https://camo.githubusercontent.com/0ee2adafe7480b53f551b0dc9bef73853ae73cd4/687474703a2f2f692e696d6775722e636f6d2f38713233714a552e706e67' width=300>
<img src='https://camo.githubusercontent.com/743d27135187210eb3e0eee4617c10761d3bc79c/687474703a2f2f692e696d6775722e636f6d2f67305275326b322e706e67' width=300>
<img src='https://camo.githubusercontent.com/3cd6ade83630b73b1a0aa4fd6c3d831c4cf686d3/687474703a2f2f692e696d6775722e636f6d2f79454b35796b622e706e67' width=300>


## Bibliografía

Pippin Hack. https://github.com/catenoid