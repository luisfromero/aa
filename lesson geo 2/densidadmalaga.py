query="SELECT BARRIO, COUNT(BARRIO) as 'habitantes' from padronbarrios Group by BARRIO"
from qgis.core import QgsVectorLayer, QgsProject
habit = QgsVectorLayer("?query={}".format(query), "habitantes", "virtual" )
QgsProject.instance().addMapLayer(habit)