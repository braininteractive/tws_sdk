import renderer
import json

filename = 'simple/Cylinder.stl'

res = renderer.render_360(os.environ['TEST_MODEL_PATH'] + '/' + filename, nsteps=3, samples=4, width=40, height=30)
print(json.dumps(res))
