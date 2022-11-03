# 2021 |-> No usado
# Ejemplo de uso mediate PyQGIS de una conexión

uri=QgsDataSourceUri()
uri.setConnection('127.0.0.1','5432','umaobd','obd_readpriv','vc0910$$')
uri.setDataSource('public', 'candidatos', 'trayectos', 'distance > 10')
layer=QgsVectorLayer(uri.uri(),'micapapg','postgres')
QgsProject.instance().addMapLayer(layer)
