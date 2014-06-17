from geomagic_api_v3 import *
import json

mod = CreateMesh()
a = mod.addPoint (Vector3D (1, 0, 0))
b = mod.addPoint (Vector3D (0, 1, 0))
c = mod.addPoint (Vector3D (0, 0, 1))
mod.addTriangle (a, b, c)
mod.run()
m = mod.mesh

result = { 'numTriangles': m.numTriangles, 'numPoints': m.numPoints }
print(json.dumps(result))
