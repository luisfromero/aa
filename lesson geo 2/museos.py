# Un script que crea una nueva capa vectorial 
# con algunos museos de Málaga (a partir de su nombre y coordenadas) y asocia el tamaño del marcador al número de visitas



def duerme(seconds):
    """Pause for set amount of seconds, replaces time.sleep so program doesn't stall"""
    #https://stackoverflow.com/questions/55301747/how-to-make-python-wait-while-qgis-renders-a-shapefile 
    import time
    time_end = time.time() + seconds
    while time.time() < time_end:
        QApplication.processEvents()

def asociaVisitas(capa):
    symbol = QgsMarkerSymbol.createSimple({})
    symbol.setDataDefinedSize(QgsProperty.fromExpression("\"visitas\"/100000"))
    capa.startEditing()
    capa.renderer().setSymbol(symbol)
    capa.commitChanges()


def creaCampos(capa):
    pr = capa.dataProvider()
    capa.startEditing()
    # add fields
    pr.addAttributes( [ QgsField("nombre", QVariant.String),
                   QgsField("visitas",  QVariant.Int)] )
    capa.commitChanges()

def agregaPuntos(capa):
    pr = capa.dataProvider()
    capa.startEditing()
    
    fet = QgsFeature()
    fet.setGeometry( QgsGeometry.fromPointXY(QgsPointXY(373438,4064747)) )
    fet.setAttributes(["Museo de Málaga","150000"])
    pr.addFeatures( [ fet ] )
    
    fet = QgsFeature()
    fet.setGeometry( QgsGeometry.fromPointXY(QgsPointXY(373331,4064929)) )
    fet.setAttributes(["Museo Picasso","703000"])
    pr.addFeatures( [ fet ] )

    fet = QgsFeature()
    fet.setGeometry( QgsGeometry.fromPointXY(QgsPointXY(372922,4064920)) )
    fet.setAttributes(["Museo Carmen Thyssen","170000"])
    pr.addFeatures( [ fet ] )
    
    capa.commitChanges()

def creaCapa():   
    project=QgsProject.instance()
    canvas=canvas = iface.mapCanvas()
    museos = QgsVectorLayer("Point", "museos", "memory")
    crs = museos.crs()
    crs.createFromId(25830)
    museos.setCrs(crs)
    creaCampos(museos)
    return museos
    
museos=creaCapa()

project=QgsProject.instance()
canvas = iface.mapCanvas()
project.addMapLayer(museos)
agregaPuntos(museos)
canvas.refresh()
duerme(10)
asociaVisitas(museos)
canvas.refresh()

