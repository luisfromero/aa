# Script para crear una capa virtual (en QGIS 3.22 había un bug que impedía crearlo desde el menu)
query="SELECT BARRIO, COUNT(BARRIO) as 'habitantes' from padronbarrios Group by BARRIO"
from qgis.core import QgsProject, QgsVectorLayer

habit = QgsVectorLayer("?query={}".format(query), "habitantes", "virtual" )
QgsProject.instance().addMapLayer(habit)
