import renderer
import json

filename = 'simple/Cylinder.stl'

res = renderer.render_tp(os.environ['TEST_MODEL_PATH'] + '/' + filename, samples=4, width=40, height=30)
print(json.dumps(res))
