from qgis.core import *
from qgis.gui import *
from qgis.utils import *

# Este script muestra cómo utilizar QGIS fuera de la aplicación
# Es importante que todas las variables de entorno de QGIS se establezcan
# en el archivo .env del workspace de VSCode (o su equivalente)
# C:\Program Files\QGIS 3.22.1\bin\qgis-bin.env -> .env

qgs = QgsApplication([], True)
qgs.initQgis()

#opcional
if False:
    project = QgsProject.instance()
    project.setCrs(QgsCoordinateReferenceSystem.fromEpsgId(25830))

path='D:/onedrive/proyectos/acc/ignore/'
layer1 = QgsRasterLayer(path+'demzonamalaga.gpkg','Mapa de Málaga')


#uri="file:///"+path+"time_series_covid19_confirmed_global.csv?crs=epsg:4326&delimiter=,&xField=Long&yField=Lat&encoding=UTF-8"
#capa1=QgsVectorLayer(uri,'confirmados', "delimitedtext")
#QgsVectorFileWriter.writeAsVectorFormat(capa1, path+'covid.gpkg', "UTF-8", capa1.crs(), "GPKG")
layer2=QgsVectorLayer(path+"covid.gpkg|layername=covid",'confirmados', "ogr")


#opcional
if False:
    project.addMapLayer(layer1)
    project.addMapLayer(layer2)

canvas=QgsMapCanvas()
canvas.setLayers([layer1,layer2])
canvas.setExtent(layer1.extent())
canvas.show()
a=input("Pulsa una tecla\n")
canvas.setExtent(layer2.extent())
#canvas.zoomToFullExtent()
canvas.refresh()
a=input("Pulsa una tecla\n")
qgs.exitQgis()