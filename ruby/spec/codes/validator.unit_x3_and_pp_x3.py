import validator
import os

filename = 'simple/Cylinder.stl'
units = [ 'mm', 'cm', 'in' ]
processes = [ 'colorstone', 'everlast', 'alumix' ]

validator.validate(os.environ['TEST_MODEL_PATH'] + '/' + filename, units, processes)
