import renderer
import json

filename = os.environ['TEST_MODEL_PATH'] + '/simple/Cylinder.stl'

material = renderer.make_material('default_material', diffuse_color=renderer.rgb(132, 132, 132))

scene = renderer.SceneOptions(default_material=material)
scene.samples = 4
scene.use_shadow = True
scene.transparent_background = False

render = renderer.Render360Options()
render.nsteps = 3
render.camera_distance = 2.8
render.resolution_x = 40
render.resolution_y = 30

res = renderer.render_360(filename, scene, render)

print(json.dumps(res))
