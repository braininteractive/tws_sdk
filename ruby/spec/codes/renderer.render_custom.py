import renderer
import json
import math

filename = 'simple/Cylinder.stl'

render1 = renderer.Render(filename='render_front.png', width=40, height=30)
render2 = renderer.Render(filename='render_iso.png', camera_rotation=math.acos(2.0 / math.sqrt(5.0)), model_rotation=math.radians(315.0), width=40, height=30)

res = renderer.render_custom(os.environ['TEST_MODEL_PATH'] + '/' + filename, samples=4, renders=[ render1, render2 ])
print(json.dumps(res))
