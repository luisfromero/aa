def refreshAll():
    iface.mapCanvas().refreshAllLayers()

def exportMap(i): 
    global canvas
    canvas.saveAsImage( "d:/covid_" + str(i).zfill(3) + ".png" )	
    
###################################
#  Cambia la última columna por cada una de la serie temporal
###################################
def cambia(i):
    global capa
    print("cambiando capa {}".format(i))
    with edit(capa):
        for feat in capa.getFeatures():
            capa.changeAttributeValue(feat.id(), nfields-1, feat[i]) #asigna el campo i-esimo al nuevo campo



    
def refresh():
    global i,max,timer
    cambia(i)
    refreshAll()
    exportMap(i-6)
    if i == max-1:
        timer.stop()
    i=(i+1)
    
def fin():
        global timer
        timer.stop()

def restart():
    global i,max,timer,capa,project,nfields
    i=6 #primera columna con datos
    max=nfields-1 #ultima columna con datos
    timer.start(2000)
    print(i)
######################################
def refreshAll():
    iface.mapCanvas().refreshAllLayers()

def exportMap(i): 
    global canvas
    canvas.saveAsImage( "d:/covid_" + str(i).zfill(3) + ".png" )	
    
###################################
#  Cambia la última columna por cada una de la serie temporal
###################################
def cambia(i):
    global capa
    with edit(capa):
        for feat in capa.getFeatures():
            capa.changeAttributeValue(feat.id(), nfields-1, feat[i]) #asigna el campo i-esimo al nuevo campo



    
def refresh():
    global i,max,timer
    cambia(i)
    refreshAll()
    exportMap(i-6)
    if i == max-1:
        timer.stop()
    i=(i+1)
    
def fin():
        global timer
        timer.stop()

def restart():
    global i,max,timer,capa,project,nfields
    i=6 #primera columna con datos
    max=nfields-1 #ultima columna con datos
    timer.start(2000)

######################################
# Add Layer with covd data
# Download from
# https://raw.githubusercontent.com/CSSEGISandData/COVID-19/master/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_confirmed_global.csv
######################################
path="d:/proyectos/acc/carto/"
uri="file:///"+path+"time_series_covid19_confirmed_global.csv?crs=epsg:4326&delimiter=,&xField=Long&yField=Lat&encoding=UTF-8"
capa1=QgsVectorLayer(uri,'confirmados', "delimitedtext")
QgsVectorFileWriter.writeAsVectorFormat(capa1, path+'covid.gpkg', "UTF-8", capa1.crs(), "GPKG")
capa=QgsVectorLayer(path+"covid.gpkg|layername=covid",'confirmados', "ogr")
# Calcula la ultima columna, y si no existe, agrega la columna -nueva-
# La inicializa con la ultima columna
nfields=len(capa.attributeList())
capa.startEditing()
if capa.fields()[nfields-1].name()!='nueva':
    capa.addAttribute(QgsField("nueva",QVariant.Int))
    nfields+=1
for feat in capa.getFeatures():
   capa.changeAttributeValue(feat.id(), nfields-1, feat[nfields-2]) #asigna el campo i-esimo al campo 4
capa.commitChanges()
project.addMapLayer(capa)

#nfea=capa.featureCount() # Nº de dias
#capa.addFeature(QgsFeature())
######################################
# Set marker
######################################

timer = QTimer()
timer.timeout.connect(refresh)
