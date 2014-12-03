import renderer
import json
import math

filename = os.environ['TEST_MODEL_PATH'] + '/simple/Cylinder.stl'

scene = renderer.SceneOptions()
scene.samples = 4
scene.use_shadow = False
scene.transparent_background = True

render1 = renderer.RenderOptions(out_filename='render_front.png', camera_distance=2.8, resolution_x=40, resolution_y=30)

render2 = renderer.RenderOptions()
render2.out_filename = 'render_iso.png'
render2.camera_rotation = math.acos(2.0 / math.sqrt(5.0))
render2.model_rotation = math.radians(315.0)
render2.resolution_x = 40
render2.resolution_y = 30

res = renderer.render_custom(filename, renderer.SceneOptions(), [ render1, render2 ])

print(json.dumps(res))
