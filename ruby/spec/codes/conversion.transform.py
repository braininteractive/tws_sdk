import conversion
import json

filename = 'simple/Cylinder.stl'
infile = os.environ['TEST_MODEL_PATH'] + '/' + filename
outfile = infile + '.ply'

res = conversion.transform(infile, outfile)
print(json.dumps(res))
