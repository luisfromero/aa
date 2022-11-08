import os
import numpy, ephem, datetime
from PIL import Image

def datetime2azalt(datetime):
  # At datetime, what is the angular location
  # of the sun over the map in radians?
  # azimuthal angle is measured clockwise from north
  # altitude angle is the elevation from the horizon
  o = ephem.Observer()
  o.lat, o.long, o.date = '51:45', '-1:15', datetime
  sun = ephem.Sun(o)
  return float(sun.az), float(sun.alt)

def array2image(array):
  # Converts a numpy array to an image
  normArray = (255.0 / array.max() * (array - array.min())).astype(numpy.uint8)
  im = Image.fromarray(normArray)
  return im

def GIS2array():
  # Extract what we want from the GIS file
  filename = 'd:/onedrive/proyectos/acc/ignore/modeloLidar.asc'
  data = open(filename, 'r')
  #parameters = dict()
  heightmap = []
  for line in data:
    if not line[0].isdigit() and line[0]!='-':
      # parse the parameters; ncols, nrows etc.
      # parameters[line.split(' ')[0]] = line[14:]
      continue
    # For the space-delimited numeric data
    heightmap.append(line.split(' ')[:-1])
  buildingMap = numpy.array(heightmap).astype(float)
  buildingMap[buildingMap == -999] = 0
  arrayFile = open('d:/onedrive/proyectos/acc/ignore/numpyarray.npy', 'wb')
  numpy.save(arrayFile,buildingMap)
  return buildingMap

def generateBitmask(buildingMap, datetime):
  # A shadow map Bitmask represents a map defined for each point in the plane
  # that defines WHETHER an object standing at that point
  # will be shaded by the occulting structure(s) around it.
  ground = buildingMap.min()
  shadowMap = numpy.ones((7607,12050))
  az, alt = datetime2azalt(datetime)
  azDispl = numpy.array([1,-numpy.cos(az)/numpy.sin(az)])
  altDispl = numpy.tan(alt)
  # generate random numbers on a subset of the image,
  # attempting to avoid problems with out of range indicies
  # low and high define the internal bounding box
  # size is (number of desired samples, 2)
  NSamples = 500000
  randomSamplex = numpy.random.randint(0, 7607, size=NSamples)
  randomSampley = numpy.random.randint(0, 12050, size=NSamples)
  randomSampley = numpy.random.randint(3000, 4000, size=NSamples)
  randomSamplex = numpy.random.randint(4500, 5500, size=NSamples)
  randomSample = numpy.column_stack((randomSamplex, randomSampley))
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
      shadowH = shadowH - altDispl
      arrayPoint = arrayPoint + azDispl

      # buildingMap must be indexed by a tuple of integers
      tupleinterpolated = int(round(arrayPoint[0])),int(round(arrayPoint[1]))
      if(0<=tupleinterpolated[0]<7607 and 0<tupleinterpolated[1]<12050):
        if(shadowH > buildingMap[tupleinterpolated]):
          shadowMap[tupleinterpolated] = 0
  return shadowMap

# Sample date time
# to achieve 7 points in the day, sampled at the first of each month
# To form the averaged images, we will iterate over
# months = range(1,13)
# hours = range(6,20,2)
# to achieve 7 points in the day, sampled at the first of each month
# datetime.datetime(2014,month,1,hour,0)

sample_datetime = datetime.datetime(2022,3,8,10,0)
# GIS2array()
buildingMap = numpy.load('d:/onedrive/proyectos/acc/ignore/numpyarray.npy')
bitmask = generateBitmask(buildingMap,sample_datetime)
bitmaskImage = array2image(bitmask)
bitmaskImage.save('d:/onedrive/proyectos/acc/ignore/output.png')