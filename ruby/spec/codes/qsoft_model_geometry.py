from geomagic_api_v3 import *
import os
import sys
from subprocess import Popen, PIPE
import bpyutils
import meshrepair
import volumecalc

def preCADconvert_if_needed(filename, newExt='.stl'):
            preconvert_extensions = [ '.sat', '.sab', '.ipt', '.iam', '.model', '.session', '.dlv', '.exp', '.catpart', '.catproduct', '.catshape', '.cgr', '.3dxml', '.asm', '.neu', '.prt', '.xas', '.xpr', '.mf1', '.arc', '.unv', '.pkg', '.ifc', '.ifczip', '.iges', '.igs', 'jt', '.x_t', '.x_b', '.xmt', '.xmt_txt', '.prc', '.3dm', '.par', '.pwd', '.psm', '.sldasm', '.sldprt', '.stp', '.step', '.stp.z', '.u3d', '.vda' ]
            if (os.path.splitext(filename)[-1].lower() in preconvert_extensions):
                        arguments = [os.environ['CMR_PATH'] + '/cadconv', filename, os.path.dirname(filename) + '/', os.environ['CMR_PATH'] + '/', '-exportonly', 'stl']
                        p = Popen(arguments, stdout=PIPE, stderr=PIPE, stdin=PIPE)
                        output, err = p.communicate()
                        try:
                                    sys.stderr.write(err.decode(encoding='UTF-8'))
                        except:
                                    sys.stderr.write(err)
                        return filename + newExt
            else:
                        return filename

def preconvert_if_needed(filename, newExt='.stl'):
            # preconvert below extensions to .stl using blender python API
            preconvert_extensions = [ '.dae', '.fbx', '.x3d', '.wrl' ]

            if (os.path.splitext(filename)[-1].lower() in preconvert_extensions):
                        bpyutils.convert_mesh(filename, filename + newExt)
                        return filename + newExt
            else:
                        return filename

def extract_properties(input_filename):
	filename = preCADconvert_if_needed(input_filename)
	filename = preconvert_if_needed(filename)

	# Set up standard options for opening a mesh file
	openOptions = FileOpenOptions()
	# A small epsilon merge radius will fix a lot of small cracks
	# NOTE: This is a potentially significant modification of the source mesh
	openOptions.mergeRadius = 0.000001
	# set to hash technique for all mergeRadius values
	openOptions.mergeVertices = 3
	# similar to printing - load as is and use it to decompose, then reorient afterwards
	openOptions.keepOrientation = True #hasPerFaceNormal
	openOptions.applyManifoldDecomposition = False
	openOptions.removeDuplicatedTriangles = False

	filereader = ReadFile()
	filereader.options = openOptions
	filereader.filename = filename
	filereader.run()

	if filereader.mesh:
		# try to repair if it is necessary.
		repair_info = {}
		mesh = meshrepair.repair_mesh(filereader.mesh, True, repair_info)
		properties = repair_info['analysis_after' if 'analysis_after' in repair_info else 'analysis_before']
		if properties['volume'] <= 0.0: # for non-closed model which wasn't able to repare.
			properties['volume'] = volumecalc.compute_mesh_volume(mesh, '', True, False, True)
		bounds = properties['boundingBox']
		properties['axisAlignedBox'] = {}
		properties['axisAlignedBox']['sizes'] = [ bounds[1][i] - bounds[0][i] for i in range(3) ]
		properties['axisAlignedBox']['matrix'] = [
			1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0,
			(bounds[0][0] + bounds[1][0]) * 0.5, (bounds[0][1] + bounds[1][1]) * 0.5,
			(bounds[0][2] + bounds[1][2]) * 0.5, 1.0 ]
		if os.path.isfile(input_filename+'.meta.json'):
			metafile=open(input_filename+'.meta.json')
			metadata = json.load(metafile)
			metafile.close()
			properties['unitFactor'] = metadata['unit_factor'] if 'unit_factor' in metadata else 1.0
		else:
			properties['unitFactor'] = 1.0
		return properties
		
if __name__ == '__main__':
            try:
                        filename = os.environ['TEST_MODEL_PATH'] + '/' + 'simple/Cylinder.stl'
                        properties = extract_properties(filename)
                        print(json.dumps({ 'result': True, 'filename': filename, 'properties': properties }, sort_keys=True))
            except Exception as e:
                        print(json.dumps({ 'result': False, 'exception': str(e) }, sort_keys=True))


