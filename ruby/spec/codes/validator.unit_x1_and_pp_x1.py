import validator
import os

filename = 'simple/Cylinder.stl'
units = 'cm'
processes = 'colorstone'

validator.validate(os.environ['TEST_MODEL_PATH'] + '/' + filename, units, processes)
