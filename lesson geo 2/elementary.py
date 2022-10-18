# Basic tasks

project=QgsProject.instance()
layerList = project.layerTreeRoot().findLayers()
for layer in layerList:
    print(layer.name())
canvas = iface.mapCanvas()
