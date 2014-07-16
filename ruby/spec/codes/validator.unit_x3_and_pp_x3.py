import validator
import os
import json

filename = 'simple/Cylinder.stl'
units = [ 'mm', 'cm', 'in' ]
processes = [ 'colorstone', 'everlast', 'alumix' ]

res = validator.validate(os.environ['TEST_MODEL_PATH'] + '/' + filename, units, processes)
print(json.dumps(res))
