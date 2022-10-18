def rule_based_symbology(layer, renderer, label, expression, symbol, color):
    root_rule = renderer.rootRule()
    rule = root_rule.children()[0].clone()
    rule.setLabel(label)
    rule.setFilterExpression(expression)
    rule.symbol().setColor(QColor(color))
    root_rule.appendChild(rule)

def getColored(i):
    id = 79055 + i
    rule = ['selected', "ogc_fid LIKE '" + str(id) + "'", QColor("green")]
    rule_other = ['other', "ogc_fid NOT LIKE '" + str(id) + "'", QColor("red")]
    rule_based_symbology(layer, renderer, rule[0], rule[1], symbol.setSize(6), rule[2])
    rule_based_symbology(layer, renderer, rule_other[0], rule_other[1], symbol.setSize(2), rule_other[2])
    layer.setRenderer(renderer)
    layer.triggerRepaint()

def action():
    global i,max,timer
    if i > 0:
        renderer.rootRule().removeChildAt(i)
        layer.setRenderer(renderer)
        layer.triggerRepaint()
    i = i + 1
    getColored(i)
    if i >= 51:
        iface.layerTreeView().refreshLayerSymbology(layer.id())
        timer.stop()

layer = iface.activeLayer()
symbol = QgsSymbol.defaultSymbol(layer.geometryType())
renderer = QgsRuleBasedRenderer(symbol)
canvas = iface.mapCanvas()

i = 0
timer = QTimer()
timer.timeout.connect(action)
timer.start(1000)
