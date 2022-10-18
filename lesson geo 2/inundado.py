params={'INPUT': 'demcortado',
            'INTERVAL': 2000.0,
            'BAND':1,
            'EXTRA': '-p',
            'NODATA': 0,
            'OFFSET': 0,
            'OPTIONS':'',
            'OUTPUT': QgsProcessing.TEMPORARY_OUTPUT}

def exportMap(i): 
    iface.mapCanvas().saveAsImage( "d:/mapa_" + str(i).zfill(2) + ".png" )
    capa.triggerRepaint()


def nuevomapa(i):
    global params,capa,renderer,project
    params['OFFSET']=i
    salida=processing.run("gdal:contour",params )
    capa= QgsVectorLayer(salida['OUTPUT'],"salida_"+str(i).zfill(2),"ogr")
    if capa.featureCount() > 1:
        # feas=capa.getFeatures()
        capa.startEditing()
        capa.deleteFeature(2)
        capa.commitChanges() #and stop editing
        project.addMapLayer(capa)
        renderer=capa.renderer()
        props=renderer.symbol().symbolLayers()[0].properties()
        props['color']='100,100,255,255'
        renderer.setSymbol(QgsFillSymbol.createSimple(props))

def fin():
    global timer,capa,project,canvas
    timer.stop()
    if capa in project.mapLayers().values():
        project.removeMapLayer(capa)
    canvas.refresh()
        

def restart():
    # Inicia o reinicia el renderizado de capas
    global i,maxi,project,canvas
    i=230
    maxi=1000
    try:
        project.removeMapLayer(capa)
    except:
        pass
    canvas.refresh()
    timer.start(5000)

def refresh():
    # Funcion temporizada
    # Borra capa existente
    global i,timer,capa,maxi,project
    if i> 1:
        try:
            project.removeMapLayer(capa)
        except:
            pass
    nuevomapa(i)
    capa.triggerRepaint()
    exportMap(i)
    i=i+1
    if i == maxi:
        timer.stop()
        project.removeMapLayer(capa)

timer = QTimer()
timer.timeout.connect(refresh)
project=QgsProject.instance()
canvas=canvas = iface.mapCanvas()


