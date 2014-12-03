import renderer
import json

filename = os.environ['TEST_MODEL_PATH'] + '/simple/Cylinder.stl'

material = renderer.make_material('default_material', diffuse_color=renderer.rgb(132, 132, 132))

scene = renderer.SceneOptions(default_material=material, samples=4, use_shadow=True, transparent_background=True)

render = renderer.Render4ViewOptions()
render.camera_distance = 0.0
render.out_filename = 'render_iso.png'
render.camera_rotation = math.acos(2.0 / math.sqrt(5.0))
render.model_rotation = math.radians(315.0)
render.resolution_x = 40
render.resolution_y = 30

res = renderer.render_4view(filename, scene, render)

print(json.dumps(res))
