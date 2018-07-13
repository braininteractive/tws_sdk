- [Getting Started](#getting_started)
  - [STOR Tutorial](#stor_tutorial)
  - [STOM Tutorial](#stom_tutorial)
- API Reference
  - [Authentication](#authentication)
  - [STOR](#stor)
  - [STOM](#stom)
  - [STOPP](#stopp)
- Example
  - [Printability Validation](#printability)
  - [Cloud storage for apps](#cloud_storage)
  - [Boolean using blender](#boolean)
  - [Off-screen Rendering using blender](#rendering)

<a name="getting_started"></a>
# Getting Started
### What 3WS is for?

3WS(3D Systems Web Service) is a cloud platform that can be used for any cloud-based 3D applications in 3D Systems.

3D Systems are developing many different apps and systems from multiple different divisions, and often times, development teams are unnecessarily repeating similar implementations. Even if one team finds a existing implementations or service, it's not easy to re-use it, because most of the time, it has dependency on specific environments, works only for certain businesses or API sets are not available in general.. etc.

3WS implements the common core of the repeating requirements as a single web based API service.
 
### What services are available?

 1. **STOR** (simple 3d object repository)
STOR service is a storage service that can store files with meta data. It's scalable and stored meta data is searchable.
 2. **STOM** (simple 3d object modeler)
Using "Geomagic Platform" and blender, STOM service provides a processing engine for 3d data. All Geomagic Platform's existing API sets and blender API are open in STOM service and can be used as a geometric processing engine for 3D applications.
 3. **STOV** (simple 3d object viewer)
3WS implements a 3d viewer in web browser using webGL technology. STOV implements an unified, customizable 3D viewing experience in the browser with 3D Systems's proprietary CMR(Cloud Model Representation) format.
 4. **STOPP** (simple 3d object print process)
STOPP is a service that provides a central database for storing printer and material properties for printing processes. STOPP is currently managing 80 properties for printability parameters for 40 printer models linked with 60 materials. STOPP service also provides material cost estimation for a few print processes.
 5. **PaaS** (Print As A Service)
Cloud based on-demand printing service API. Based on QuickParts API.

### Get started with 3WS console
3WS has a simple console interface to test out the service's functionality without using provided SDK. 3WS console has interfaces to three services: STOR, STOM and STOPP. STOPP service is a simple readonly database that provides printer/material properties, let's walk through STOR and STOM services in this tutorial.

**Console Tutorial #1. Upload a model with meta data to STOR**

 1. Click "STOR" link in the top navigation menu.
 2. Click "Upload" button and select a file. STOR console populates default meta data for filename when uploading finishes.
 3. Edit meta data (optional)
```
{"filename":"Selected file.stl", "my name":"3DS developer"}
``` 
The meta data should be a valid JSON format.

 4. Click "Create" to create a STOR object with uploaded file and meta data. Console will update the list.
 5. Try search uploaded file with values from meta data. Type  q=3DS in the search box at upper right corner to search the uploaded file.

**Console Tutorial #2. Hello world with STOM**
1. Click "STOM" link in the top navigation menu.
2. Click "New Session" button. This will create a sand-boxed environment for your hello world program.
3. Type following python code in the code editor.
```
print("Hello World")
```
4. Click "Run code" button to execute your hello world python program. "Hello World" should appear on the Result box.

**Console Tutorial #3. Count number of triangles and points in a STL mesh**
1. Upload a STL file to STOR using STOR console. After creating a model, STOR will assign a unique 10 digit alphanumeric model ID for the uploaded mesh.
2. In STOM console, create new session and paste following python code in the code editor. Replace red values with your 3WS account credentials and STOR model ID in the code. Your API key and secret can be found in "Account" menu in top navigation.
```
# Importing required python libraries including Geomagic Platform
import urllib  
from tws import Tws  
from geomagic_api_v3 import *

# Downloading the file stored in STOR to local disk in STOM session using 3WS SDK for python
t = Tws(api_key='Your API Key', api_secret='Your API secret')  
filename =t.get_model('Your STOR Model ID')['meta']['filename']  
url = t.get_link('Your STOR Model ID', filename)  
urllib.request.urlretrieve(url, filename)

# Setting up options for Geomagic Platform's fileio module
openOptions = FileOpenOptions()  
openOptions.mergeRadius = 0.000001  
openOptions.mergeVertices = 3  
openOptions.keepOrientation = True  
openOptions.applyManifoldDecomposition = False  
openOptions.removeDuplicatedTriangles = False

# Read STL file using fileio module  
filereader = ReadFile()  
filereader.options= openOptions  
filereader.filename = filename  
filereader.run()  
mesh = filereader.mesh  
  
# Print out geometric properties  
print("triangles: %d, " % (mesh.numTriangles))  
print("points: %d" % (mesh.numPoints))
```
3. Clicking "Run code" should return the number of triangles and points in the mesh data. In this example, we're using ADP's fileio module to import STL file and returns mesh properties. More detailed documentation for ADP can be found at  [this TeamPlatform workspace](https://3dsystems.teamplatform.com/workspaces/35715#tab-files-387258-s)

**Console Tutorial #4. Simple off-screen rendering**
1. Upload a STL to STOR
2. In STOM console, type in following code.
```
import renderer  
import urllib  
import json  
from tws import Tws

API_KEY = 'Your API Key'  
API_SECRET = 'Your API Secret'  
FILE_ID = 'Your STOR Model ID'

t = Tws(api_key=API_KEY, api_secret=API_SECRET)  
filename = t.get_model(FILE_ID)['meta']['filename']  
url = t.get_link(FILE_ID, filename)  
urllib.request.urlretrieve(url, filename)

# render 4-view image with specified options  
res = renderer.render_tp(filename, outdir='./', outprefix='pink_', samples=4, width=640, height=480)

# Upload rendered images to STOR  
for out in res["outfiles"]:

m = t.upload_model(out, meta={"filename":out})

print('<a href="', t.get_link(m['id']), '">', out, '</a>&nbsp;')
```
3. STOM console will return download links to rendered images.

### Get started with SDK - Python
3WS API's are simple http REST APIs. Client applications can call 3WS API using any REST client of its choice. Another option is to use 3WS SDK which is being maintained by 3WS team. We're providing SDKs for multiple different languages:

 - Ruby: [https://github.com/workon3d/tws_sdk](https://github.com/workon3d/tws_sdk)
 - Python: [https://github.com/workon3d/tws_sdk](https://github.com/workon3d/tws_sdk)
 - C#: [https://github.com/workon3d/tws_sdk_windows](https://github.com/workon3d/tws_sdk_windows)
 - Php: [https://github.com/workon3d/tws_sdk](https://github.com/workon3d/tws_sdk)
 - Java: [https://github.com/workon3d/tws_sdk](https://github.com/workon3d/tws_sdk)

To use 3WS service through API calls, API user should sign up at  [https://www.dddws.com](https://www.dddws.com%2C/) to get account credentials. 3WS API credentials are composed of key and secret which are required whenever an application calls the API. Go to [https://www.dddws.com/account](https://www.dddws.com/account) to get your own api key and secret.

1. Download python SDK to local folder and change directory to tws_sdk/python
```
$ cd tws_sdk/python
```
2. Run python interactive console:
```
$ python
```
3. Import tws SDK
```
>>> import tws
```
4. Initialize with your api key and secret
```
>>> t = tws.Tws(api_key="your api key", api_secret="your api secret")
```
5. Check if your api key and secret is authenticated
```
>>> t.authenticate()

{'authenticated': True, 'id': 123, 'key': 'your api key', 'email': '[you@example.com](mailto:you@example.com)'}
```

<a name="stor_tutorial"></a>
## STOR Tutorial
### Upload a file
STOR service provides file storage with meta data store and search.
When you store a file into STOR, you have additional options to store JSON meta data for the file.

* Example with ruby SDK, 
```
    irb> load 'tws.rb'
    irb> t = TWS.new :api_key => 'your api key', :api_secret => 'your api secret'
    irb> t.upload_model "/path/to/a/file.txt", {:filename => 'my custom filename', :filesize => 1234}
```
This example uploads the file.txt file with filename as 'my custom filename'.

### Search files
STOR provides two types of search query. One is universal full text search on meta data fields using "q=" parameter. The other is exact match query for meta keys using "meta=key:value" query.

* Search for files with extension .txt :
```
    irb> t.get_models :q => "*.txt"
```
* Search files with filename as 'myfilename'
```
    irb> t.get_models :meta => "filename:myfilename"
```

### Download a file
STOR SDK's get_link method will give you a download link to a file with authentication query strings.
```
    irb> t.get_link "a1b43m4b4"
    => "https://stor.dddws.com/api/v1/models/772ebeaabf/download?expire=1395168763&key=bddc..&signature=f44.."
```
This download link works without further authentication and will expire in given amount of time.

<a name="stom_tutorial"></a>
## STOM Tutorial
### How STOM works
The STOM is a scalable geometric processors on the cloud.

The workflow is;
- API user creates a stom session
- Each stom session can run python scripts posted by API user.

Example (python SDK)
```
    $ python
    >>> from tws import Tws
    >>> t = Tws(api_key="your api key", api_secret="your api secret")
    >>> t.create_session(300)
       * Session has a timeout and state. After given timeout, the session state will change to 'closed'.
    >>> t.create_run("ca7e8b4c", "", "print('hello')")
       * create_run actually runs the posted python code script - this case: print('hello') - on running stom processes.
       * second argument for create_run is deprecated and should be blank.
       * After the actual python code completes to run on the server side, the state of the previous run will change to 'complete'
    >>> t.get_runs("ca7e8b4c")
```

### Do I need to post a python code every time?
Yes. STOM session allows you to inject any python code into server and run it. STOM is completely flexible platform on which you can run your own codebase using provided blender and adp platforms. One can think of STOM as an application platform rather than geometric API services. 3WS team will develop/maintain python code samples for various operations like boolean or geometry validations for printing, off screen rendering... etc.


### Session timeout and platform
Default STOM session timeout given with the SDK's is 60 seconds. User can specify other values for the timeout up-to 1800 seconds. When the timeout reaches, STOM will close the session killing running processes. API user can run long-running processes on the STOM as long as the timeout allows.

[Deprecated] STOM has two types of built-in geometric engines which are blender 2.69 and adp(geomagic platform) v2. Both engines have python API and that's why we choose python as a scripting language. Each stom will run individual processes for blender and adp.

### Engine version
Each STOM session can be run from different engine images which contains different versions and features of ADP geometric engine. To manage ever-changing STOM capabilities, user can specify session engine version for each session. This "engine_version" parameter will be set to **"latest"** by default if not specified.

"latest" engine is a latest nightly build and will be updated without announcement. Other than "latest" engine, each and every engine versions will be locked down and will not change. Our dev team will consistently update the "latest" but other versions will remain same as before. List of available engine version can be found at STOM API reference page.

### Security
Each STOM session will be sandboxed and will be completely independent from each other. The sandboxed environments actually gives you full root privileges on ubuntu 12.04 environments with installed blender and adp platform. Each code runs will be stored on our database, but actually running session processes will be terminated after you closed the running STOM session. Closing STOM session kills all the processes running in the sandboxed env and wipes out the disk also.

# API Reference
<a name="authentication"></a>
## Authentication
Every 3WS API call should be accompanied with either a Authorization header or authentication string.
These authentication procedures are readily implemented in the SDK, but when you need to implement your own SDK or need to do raw request in http level, probably you need to understand the authentication mechanism.

For example, to get list of models you have, the request would be;
```
GET /api/v1/models?expire=1395178298 HTTP/1.1
Host: stor.dddws.com
Authorization: 3WS feb92d8b15d2dac6e003c41e29697bd3:frJIUN8DYpKDtOLCwo%2F%2FyllqDzg%3D
```

The authorization header is made of two parts, your api key and signed signature.
``` 
Authorization: 3WS your_api_key:signature
``` 

``signature`` is a base64 encoding of HMAC-SHA1 encryption of your api key and "string_to_sign".
"string_to_sign" is canonicalized resource description for the request.
For the above example, The "string_to_sign" would be "GET\n\n\1395178298\n/api/v1/models"
The "string_to_sign" comes in the following format.
```
{METHOD}\n{options}\n{expire}\n{url}
``` 
* ``{METHOD}`` can be one of GET, PUT, POST, DELETE
* ``{options}`` is blank for now.
* ``{expire}`` is absolute integer form of request expiration time.
* ``{url}`` is actual API endpoint url

Once the string_to_sign has been determined, you can encrypt it with your api key and send it as signature.
``` 
Signature = escape( Base64( HMAC-SHA1( your_api_key, UTF-8-Encoding-Of( StringToSign ) ) ) )
```

<a name="stor"></a>
## STOR
### Get models

To get models from STOR,
```
GET  https://stor.dddws.com/api/v1/models?expire=1395187476
```
Returns 30 models per page. To get more than 30 models, add page= parameter to the request,
```
GET https://stor.dddws.com/api/v1/models?expire=1395187476&page=2
```
To show a lot more models per each page,  per_page=  parameter can be added to the request and it can be extended to 1000.

Response would be 200 Ok with JSON array response body.
Returned json object contains object properties which includes id, created/updated timestamps and meta.
* Optional parameters
```
q=keyword
meta=key:value
facet=facet1,facet2...
```
 
### Get model

To retrive a model object from STOR,
```
GET https://stor.dddws.com/api/v1/models/772ebeaabf?expire=1395187476
```
Returns JSON object for model "772ebeaabf"  

### Create a model
To create a new model on STOR,
```
POST  https://stor.dddws.com/api/v1/models
JSON body:
{ "meta": {"filename":"my file name"} }
```
This will create a new model on STOR without having an actual file content.
To upload a file to STOR, you need to upload the data first, and then create a model for it.

### Upload a file
Uploading a file to STOR is three step process;
1. Get a url to upload a file and upload_id
2. Upload(POST) local file to the url
3. Create a model with upload_id from step 1

To get an authenticated url to upload a file to STOR,
```
POST  https://stor.dddws.com/api/v1/models/presign
JSON body:
{ "ip": "123.456.1.2", "starts_with: {"file":""} }
```
Response includes upload_id along with pre-authenticated POST parameters.
```
{"upload_id":"16c8bf43f3", ...}
```
The "form_action" from the response would be closest upload location if you specified "ip" when requesting presigned form.
Upload local file to the url as multipart/form-data.

To create a model with the "upload_id"
```
POST https://stor.dddws.com/api/v1/models
JSON body:
{ "upload_id":"16c8bf43f3", "meta": {"name":"my name"} }
```
 
### Download a file
To download file from existing model,
```
GET https://stor.dddws.com/api/v1/models/772ebeaabf/download?expire=1395187476
```
If you create a model with meta item ``filename``, the downloaded file will have the same name. Otherwise, the name will be same with ``stor_id`` such as "772ebeaabf".

### To download the file with different name,
```
GET https://stor.dddws.com/api/v1/models/772ebeaabf/desired_downloaded_filename
```  

### Update Meta
To update meta data for a model,
```
PUT  https://stor.dddws.com/api/v1/models/772ebeaabf
JSON body:
{ "meta": {"new name":"awesome new key", "existing key":"overwrite value for existing key"} }
```

### Delete a file
To delete the file from STOR,
```
DELETE https://stor.dddws.com/api/v1/models/772ebeaabf?expire=1395187476
```
The above DELETE method HTTP request will delete the item having stor_id - 772ebeaabf, if you are the owner of it.

<a name="stom"></a>
## STOM
### Create session
To create a STOM session,
```
POST https://stom.dddws.com/api/v1/sessions
JSON body:
{ "timeout": 300, "engine_version": "latest" }
```
Above request will create a 5 minutes session. Created session will close after 5 minutes. "engine_version" will be set to "latest" when not specified.
To keep the persistent result from using STOM, it would be better to set the specific engine version than leave it as default.

### Check session status
To get STOM session status,
```
GET https://stom.dddws.com/api/v1/sessions/87a78011
Response: {"id":"87a78011", "state":"active", "timeout":300, ...}
```
Active session status indicates that you can run codes with that session.
A new code is not going to run on closed sessions.

### Create a run
Once a session has been created, you can start running python scripts on it.
To run a code,
```
POST https://stom.dddws.com/api/v1/sessions/87a78011/runs
JSON body:
{ "platform":"blender", "code":"print('Hello Stom')" }
```

### Check run status
Once a code has been posted, you can poll the server the specific code run finished successfully.
```
GET https://stom.dddws.com/api/v1/sessions/87a78011/runs/1
Response: {"id":1, "state":"complete", "platform":"blender", "code":"print('Hello Stom')", "result":"Hello Stom", ...}
```
 
### Close a session
To close a session,
```
DELETE https://stom/dddws.com/api/v1/sessions/87a78011
```

<a name="stopp"></a>
## STOPP
### Usage
To get printers,
```
https://stopp.dddws.com/api/v1/printers
``` 

To get materials,
```
https://stopp.dddws.com/api/v1/materials
```
 
To filter by state,
```
https://stopp.dddws.com/api/v1/printers?state=active
https://stopp.dddws.com/api/v1/materials?state=active
``` 
Available states: ``create``, ``active``, ``archived``

To calculate estimated cost,
```
https://stopp.dddws.com/api/v1/costs?printer_id=cf2a3a&material_id=42964c&input[part_volume]=10&input[support_volume]=5&input[part_cost]=0.1
Parameters:
  printer_id
  material_id
  input[*] : required input parameters can be found from /printers endpoint in "cost_input" property. If a given printer doesn't have "cost_input" field, it means that the cost calculation module for the printer is not available yet. Even if a printer has a cost calculation module(cost_input field is present), cost calculation can return null output when the given material doesn't define required properties yet.
```
 
Example response:
```
{
    "printer":{
        "id":3,
        "name":"ProJet 6000 HD",
        "modes":["HD","UHD","XHD"]
    },
    "material":{
        "id":1,
        "name":"VisiJet SL Flex"
    },
    "input":{
        "part_volume":"10",
        "support_volume":"5",
        "part_cost":"0.1"
    },
    "costs":{
        "HD":1.5657620041753655,
        "UHD":1.5657620041753655,
        "XHD":1.5657620041753655
    }
}
```

# Examples
<a name="printability"></a>
## Printability Validation
Geomagic Solutions developed original validator in 2013 and it's ready for use in STOM service.

1. Store a file in STOR
```
import tws
tws.upload_model("path/to/local file.stl", meta={"filename":"model.stl"})
``` 
2. Validate the model
```
session = tws.create_session()
run = tws.create_run(session['id'], 'adp', python_code_for_stom)
``` 
python_code_for_stom:
```
import urllib
from tws import Tws
import validator
 
t = Tws(api_key='your api key', api_secret='your api secret')
filename =t.get_model('your model id on STOR')['meta']['filename']
url = t.get_link('your model id on STOR', filename)
 
try: # for python2
  urllib.urlretrieve(url, filename)
except: # for python3
  urllib.request.urlretrieve(url, filename)
 
# check possible units in validator
all_units = validator.get_length_units()
print('units: ' + str(all_units))
 
# check possible print processes in validator
all_processes = validator.get_print_processes()
print('processes: ' + str(all_processes))
 
# validate for all units and print processes combination
validator.validate(filename, 'all', 'all')
# or
validator.validate(filename, all_units)
 
# validate for specific unit and chosen processes
validator.validate(filename, 'mm', ['cube2', 'cube3'])
``` 
 
3. Getting the validation result
```
run = tws.get_run(session['id'], run['id'])
run['result'] # All standard output from the python_code_for_stom
```

### ADP validator documentation
**validator** is a package supports printability validation including basic mesh analyzing and repairing APIs

**Function list on module validator**

* ``validator.get_length_units()``
  Retrieves possible length units which can be used in validator.validate. At the time of writing this, it returns ['mm', 'ft', 'm', 'cm', 'in'] as JSON.
```
['mm', 'ft', 'm', 'cm', 'in']
```
* ``validator.get_print_processes()``
  Retrieves possible print processes which can be used as ‘print_processes’ in validator.validate. Actually, the resulting processes are defined by configuration file - ‘geomaticutil/config/print_processes.ini’.
```
['acutough', 'alumix', 'ceramix', 'clear', 'colorstone', 'everlast', 'frost', 'cube2', 'cube3', 'tpc', 'cubex', 'sls', 'mjp', 'cer', 'cjp', 'sla']
```

* ``validator.validate(mesh_file, units='mm', print_processes='all', disable_tests=[])``
  Analyzes the specified mesh, evaluates whether the model is valid for 3d printing for specific unit and process and returns the validation result as JSON. 
  
  **mesh_file**: Specifies the path to the input mesh file, including extension.
  **units**: The length units used in the mesh file. Below combinations are possible.  
```
units=’mm’
units=[‘mm’]
units=[‘mm’, ‘in’]
units=’all’
```

  **print_procces**: Print processes to evaluate printability validation.
```
print_process=’acutough’
print_process=[‘acutough’]
print_process=[‘acutough’, ‘frost’]
print_process=’all’
```

  **disable_tests**: Specifies validation tests to skip. (Possible options: min_wall_thickness, max_wall_thickness, gap_thickness, bottleneck, interlocking). (This option is available from the engine version 20140430)
```
disable_tests=’max_wall_thickness’
disable_tests=[‘interlocking']
disable_tests=[‘min_wall_thickness’, 'gap_thickness']
```
 
  **return value**: return the validation summary in the form of JSON with stdout.
```
[{

  "version": 1,
  "unit": "in",
  "analysis": {
    "fillRatio": 0.41818181818181605,
    "numMeshes": 2,
    "numTriangles": 96,
    "volume": 551.9999999999972,
    "boundingBox": [ [ 1.0, 0.0, 0.0 ], [ 23.0, 10.0, 6.0 ] ],
    "surfaceArea": 519.9999999999991,
    "numVertices": 52
  },
  "validation": [{
    "process": "acutough",
    "errorcode": 201,
    "message": "Too large",
    "valid": false,
    "context": "model size in X direction is too large."
  }]
}]
```
 
Example:
```
import urllib
from tws import Tws
import validator
 
t = Tws(api_key='your api key', api_secret='your api secret')
filename =t.get_model('your model id on STOR')['meta']['filename']
url = t.get_link('your model id on STOR', filename)
 
try: # for python2
  urllib.urlretrieve(url, filename)
except: # for python3
  urllib.request.urlretrieve(url, filename)
 
# check possible units in validator
all_units = validator.get_length_units()
print('units: ' + str(all_units))
 
# check possible print processes in validator
all_processes = validator.get_print_processes()
print('processes: ' + str(all_processes))
 
# validate for all units and print processes combination
validator.validate(filename, 'all', 'all')
 
# validate for specific unit and chosen processes
validator.validate(filename, 'mm', ['acutough', 'frost'], 'max_wall_thickness')
``` 

Error codes:

| Type | Code | Valid | Message | Context |
|--|--|--|--|--|
| | 0 | TRUE | [None] | [None] | 
| | 1 | FALSE | [unspecified] | [unspecified] | 
| | 2 | FALSE | Invalid input file | Input file is corrupted or does not contain a valid mesh | 
|Analyze | 101 | TRUE | Not a manifold | | 
| | 102 | TRUE | Not a closed manifold | | 
| | 103 | TRUE | Not a closed orientable manifold | | 
| | 104 | TRUE | Too many intersecting triangles | problem ratio = %1 | 
| | 105 | TRUE | Contains internal cavity | |
| | 106 | TRUE | Bad geometric orientation | problem ratio = %1 | 
|BoundingBox | 201 | FALSE | Too large | model size in %1 direction is too large. | 
| | 202 | FALSE | Too small | model size in %1 direction is too small. | 
| | 203 | FALSE | Too long bounding box sum | ['ceramix' only] | 
| | 204 | FALSE | Too short bounding box sum | ['ceramix' only] | 
| | 205 | FALSE | Too low print density | ['ceramix' only] | 
|WallThickness | 301 | TRUE | Too thin | Wall Thickness test failed: %1 of the data exceeding limit of %2 | 
| | 302 | TRUE | Warning: Too thin | Wall Thickness test warning: %1 of the data exceeding limit of %2 | 
|MaxWallThickness | 401 | TRUE | Too thick | Max Wall Thickness test failed: %1 of the data exceeding limit of %2 | 
| | 402 | TRUE | Warning: Too thick | Wall Thickness test warning: %1 of the data exceeding limit of %2 | 
|GapThickness | 501 | TRUE | Too narrow | Gap Thickness test failed: %1 of the data exceeding limit of %2 | 
| | 502 | TRUE | Warning: Too narrow | Gap Thickness test warning: %1 of the data exceeding limit of %2 | 
|InterlockingGapChecking | 601 | TRUE | Interlocking | Interlocking gap checking test failed: %1 of the data exceeding limit of %2 | 
| | 602 | TRUE | Warning: Interlocking | Interlocking gap checking test warning: %1 of the data exceeding limit of %2 | 
|Bottleneck | 701 | TRUE | Bottleneck | Bottleneck checking test failed: %1 of the data exceeding limit of %2 | 
| | 702 | TRUE | Warning: Bottleneck | Bottleneck checking test warning: %1 of the data exceeding limit of %2 | 

<a name="cloud_storage"></a>
## Cloud storage for apps
Let's assume we have this imaginary desktop application "DesignMe" which needs to implement a menu called "Save to cloud".
By having this cloud functionality, users of your app wouldn't need to moving around their files.
User will save their work directly to the cloud and open it from the cloud. 

In this example tutorial, we're going to implement "save to cloud" and "open from cloud" menus for "DesignMe".
3WS provides SDK's for a few different languages, our choice here would be python SDK for simplicity's sake.
 
1. **Save to cloud**
When user finishes working on the data on your application and clicks "Save to cloud" button.
Your app probably would want to save that data in temporary location before uploading it to STOR.
Let's say the temp location is "/tmp" and your app saved the data as "/tmp/_201401010000.tmp"

To upload the saved tmp data, using python SDK,
```
from tws import Tws
tws = Tws()
meta = {
  "filename": "Untitled 01.designme",
  "local path": "/Users/Tom/Documents/DesignMe/Untitled 01.designme",
  "version": 1,
  "app_id": "d7a1b15e"
}

model = tws.upload_model("/tmp/_201401010000.tmp", meta=meta)
=> {'id': '444183f9ce', 'meta':{'local path':'/Users/..', ...}, 'created_at':'...'}
```

STOR system will upload the data and store the meta. In this example, we stored meta data like "local path", "version" and "app_id".
Storing meta data is optional, but depending on your app design, you probably want to store end users's local save location for the file, version number if you want to track it, and someway to identify who owns this file in STOR. Your API account owns the file, but when you're using STOR for multiple app instances, you need to identity your user or application instance to look up files. When you upload and create a model in STOR, STOR returns model id on STOR. This id is unique on STOR and can be used for identification of the file. In this example, the model is is "444183f9ce"
 
2. **Open from cloud**
"DesignMe" probably wants to show an interface for users to browse models on the cloud and click open a model.
STOR has API to search stored models by querying meta data. In this example, the app will query models stored on STOR using "app_id" and end user's keyword on local file name.
```
models = tws.get_models({ "meta": "app_id:d7a1b15e", "q": "" })
```
Above query will return all models stored with "app_id" of "d7a1b15e". If you want to query with a keyword,
```
models = tws.get_models({ "meta": "app_id:d7a1b15e", "q": "*.designme" })
```
Query will return models having ".designme" somewhere in meta data.

"get_models " returns 30 items per request, app should query next page using "page" parameter.
```
models = tws.get_models({"meta":"app_id:d7a1b15e", "q":"", "page":2})
``` 
Once user selects a file to open, your app might want to download the file from STOR, saves it somewhere in temp or working directory. Let's assume the local path would be "/tmp/downloaded/_201401010001.tmp"
```
link = tws.get_link('444183f9ce')
```
"get_link" will return downloadable https link to the file. You app needs to download the file with this link using something like urlretrive,
```
urllib.urlretrieve(link, "/tmp/downloaded/_201401010001.tmp")
```
Now, your app would display the downloaded model by opening _201401010001.tmp file.

* **Note**
STOR service doesn't have predefined fields other than id and timestamps.
Meta data should be set and stored by applications as needed basis.

<a name="boolean"></a>
## Server side Boolean operation using STOM
When a browser or mobile app needs to run a serious geometric operations without enough processing power, server side geometric operation can be a solution.Let's assume we have a app named "DiffMe" which needs to implement boolean diff - subtract - operations on two STL files.

"DiffMe" would store two input STL files on STOR and will use STOM service to run boolean operations and gets the results stored back on STOR.

The workflow would be;
1. Store two STLs on STOR
2. Create a STOM session
3. Run python script to do boolean operation on Two STL files using 'blender' platform.
> 3.1 Download two files from STOR<br/>
> 3.2 Boolean using blender API<br/>
> 3.3 Save resulting STL<br/>
> 3.4 Upload resulting STL to STOR
4. Download output STL from STOR
 
1. Store two STL files on STOR
```
m1 = tws.upload_model("path/to/local/a.stl")
m2 = tws.upload_model("path/to/local/b.stl")
```
2. Create a STOM session
```
session = tws.create_session(600)
session_id = session['id']
```
3. Run python script on STOM
boolean.py:
```
import urllib
from tws import Tws

# Get model from STOR
t = Tws(api_key='your api key', api_secret='your api secret')

url = t.get_link('id of m1', 'a.stl')
urllib.request.urlretrieve(url, "a.stl")

url = t.get_link('id of m2', 'b.stl')
urllib.request.urlretrieve(url, "b.stl")

# Boolean
import bpy
    
s = bpy.context.scene
for o in s.objects:
  s.objects.unlink(o)

bpy.ops.import_mesh.stl(filepath='a.stl')
bpy.ops.import_mesh.stl(filepath='b.stl')

s = bpy.context.scene
a = s.objects[0]
b = s.objects[1]

diff = a.modifiers.new('Diff', 'BOOLEAN')
diff.object = b
diff.operation = 'DIFFERENCE'

bpy.ops.object.modifier_apply(apply_as='DATA', modifier='Diff')

s.objects.unlink(b)

bpy.ops.object.select_all()
bpy.ops.export_mesh.stl(filepath="diff.stl", check_existing=False)

m = t.upload_model("diff.stl", meta={"filename":"diff.stl"})
print(m)
``` 
boolean.py is the actual code sniffet to run on STOM session. To post the code to STOM service, you need to have the python code as a string with all the new lines and tabs if necessary. Create a run using tws SDK,

``` 
run = tws.create_run(session_id, 'blender', code)
run_id = run['id']
```
 
When STOM gets the code to run, it actually execute the code line by line storing standard output as a result.
In this example, you need the boolean diff result stored in STOR as STL format.
To get the result you need to know the STOR object id from last print(m).

``` 
run = tws.get_run(session_id, run_id)
```

Depending on the complexity, the boolean operation can take some time to finish. In the case of long running process, you need to poll the service to check the previous run is complete or not. get_run returns current state of given run. When run['state'] is 'complete', it means that the process completed and the standard output of the run will be present as run['result']

<a name="rendering"></a>
## Off-screen Rendering using blender
### module renderer
‘renderer’ is a package which supports off-screen rendering functions of a 3-d model file.

### Function list on module renderer
``make_material(name, diffuse_color=(1, 1, 1), diffuse_intensity=1.0, specular_color=(1, 1, 1), specular_intensity=0.5, specular_hardness=20, ambient=1.0, use_vertex_color=False, use_raytrace=True)``
Create a material with specified settings. The resulting material of this method can be passed as a 'default_material' argument of renderer.reder_xx().

``renderer.render_360(meshfiles, outdir='./', outprefix='render_', nsteps=4, samples=10, use_shadow=True, background_color=(1, 1, 1), transparent_background=False, default_material=None, camera_type='PERSP', camera_fov=math.radians(35.0), camera_distance=3.2, camera_orthoscale=1.0, camera_rotation=0.0, width=640, height=480)``
Generate 360 degree views of a specified model and returns the render image file paths as JSON format.

``renderer.render_tp(meshfiles, outdir='./', outprefix='render_', samples=10, use_shadow=True, background_color=(1, 1, 1), transparent_background=False, default_material=None, camera_type='PERSP', camera_fov=math.radians(35.0), camera_distance=3.2, camera_orthoscale=1.0, width=640, height=480)``
Generate front, right, top, and iso views of a specified model and returns the render image file paths as JSON format.

``renderer.render_custom(render_custom(meshfiles, outdir='./', samples=10, use_shadow=True, background_color=(1, 1, 1), transparent_background=False, renders = [ Render() ], default_material=None)``
Generate single or multiple view images with respect to each Render object of 'renders' and returns the rendered image file paths as JSON format.

``renderer.Render(filename="render.png", camera_type='PERSP', camera_rotation=0, camera_fov=math.radians(35.0), camera_distance=3.2, camera_orthoscale=1.0, model_rotation=0, width=640, height=480)``
Create an object of the class Render which describes local settings for a rendered image.

### Arguments
* **meshfiles**: Specifies the relative path or an array of the path to the input mesh file(s), including extension. Below combinations are possible.
```
meshfiles = ’sample.stl’
meshfiles = [ ’sample.stl’ ]
meshfiles = [ ’sample1.stl’, 'subdir/sample2.obj' ]
```
* **outdir**: Specifies the relative path of the output directory to store render image files. The default output directory is defined as './' (current directory).
```
outdir = './'  # default
outdir = 'images/subdir/'
```
* **outprefix**: Specifies the prefix which makes a name of render image file.
* **nsteps**: Specifies the number of views. For instance, if nstep is set to 4 (by default), four render images will be generated with 90 degree intervals.
* **use_shadow**:Specifies whether to render shadow or not. (set to True by default)
* **background_color**: Background color. (R, G, B) format.
* **transparent_background**: Specifies whether to make the background color transparent. The 'background_color' is ignored when this option is set to True.
* **camera_type**: enum in ['PERSP', 'ORTHO']. Sepcifies whether the projection mode is perspective or orthographic.
* **camera_fov**: Field of view. This will be effective when only the value of 'camera_type' is set to 'PERSP'.
* **camera_distance**: distance from the center of model bounding box. (The imported model is scaled to fit the bounding box [(-0.5, -0.5, -0.5), (0.5, 0.5, 0.5)]).
* **camera_orthoscale**: Specifies the amount of scaling when the 'camera_type' is set to 'ORTHO'. 
* **camera_rotation**: Specifies the angle between +Y-axis and look at direction in the YZ-plane, measured CCW from Y-axis, in radians. (The 'camera_angle' which is the same with 'camera_rotation' is also possible, but it will be deprecated).
* **model_rotation**: Specifies the angle between in the XY-plane, measured CCW from Z-axis, in radians.
* **samples**: Specifies the number of samples for generating shadow. Higher number of samples will produce better image quality but it will take more rendering time.
[http://www.blender.org/documentation/blender_python_api_2_70_5/bpy.types.AreaLamp.html?highlight=bpy.types.arealamp#bpy.types.AreaLamp.shadow_ray_samples_x]

* **width**: Specifies the number of pixels of a render image in x direction.
* **height**: Specifies the number of pixels of a render image in y direction.
* **camtype**: Specifies whether camera projection model is perspective or orthogonal.
```
camtype = 'PERSP' # perspective projection
camtype = 'ORTHO' # orthogonal projection
```
* **default_material**: Specifies default material which is used when a 3d model is without having texture or vertex color. The material can be created from renderer.make_material() method
* **renders**: Specifies an array of 'Render' instances.
```
renders = [ Render() ] # specify a default(front) view
renders = [ Render(filename='right.png', camtype='PERSP', camrot=0, modelrot=math.radians(270.0), width=640, height=480) ] # specify a right view
```
 
### Return value

Above three functions return the validation summary in the form of JSON with stdout.
```
{
    "result": true,
    "outfiles": [
        "render_00.png",
        "render_01.png",
        "render_02.png",
        "render_03.png"
    ]
}
```

### Simple Render
```
import renderer
import urllib
import sys
import math
import json
from tws import Tws

API_KEY = 'YOUR API KEY' # type your API key here
API_SECRET = 'YOUR API SECRET' # type your API secret here
FILE_ID = 'MODEL FILE ID' # STOR model id
DEPENDENCY_IDS = [] #[ 'TEXTURE SOURCE', 'MATERIAL FILE' ] - specifies store model ids if needed

t = Tws(api_key=API_KEY, api_secret=API_SECRET)

filename = t.get_model(FILE_ID)['meta']['filename']
url = t.get_link(FILE_ID, filename)
urllib.request.urlretrieve(url, filename)

for dep in DEPENDENCY_IDS:
  depname = t.get_model(dep)['meta']['filename']
  url = t.get_link(dep, depname)
  urllib.request.urlretrieve(url, depname)

# render 4-view image with specified options
res = renderer.render_tp(filename, outdir='./', outprefix='pink_', samples=4, width=640, height=480)
```

### Rendering with customizable options
```
import renderer
import urllib
import sys
import math
import json
from tws import Tws

API_KEY = 'YOUR API KEY' # type your API key here
API_SECRET = 'YOUR API SECRET' # type your API secret here
FILE_ID = 'MODEL FILE ID' # STOR model id
DEPENDENCY_IDS = [] #[ 'TEXTURE SOURCE', 'MATERIAL FILE' ] - specifies store model ids if needed

t = Tws(api_key=API_KEY, api_secret=API_SECRET)

filename = t.get_model(FILE_ID)['meta']['filename']
url = t.get_link(FILE_ID, filename)
urllib.request.urlretrieve(url, filename)

for dep in DEPENDENCY_IDS:
  depname = t.get_model(dep)['meta']['filename']
  url = t.get_link(dep, depname)
  urllib.request.urlretrieve(url, depname)

# render 360 degree views
res = renderer.render_360(filename, outdir='./', outprefix='default_', nsteps=4, camera_rotation=math.radians(30), samples=4, width=640, height=480)
for outfile in res['outfiles']:
  m = t.upload_model(outfile, {"filename": outfile})
  sys.stdout.write('<img src="' + t.get_link(m['id']) + '" width="200" height="150">\n')
sys.stderr.write(json.dumps(res))

# render 4-view images with pink material
pink_material = renderer.make_material(name='default', diffuse_color=(0.94, 0.32, 0.72))
res = renderer.render_tp(filename, outdir='./', outprefix='pink_', samples=4, width=640, height=480, default_material=pink_material)
for outfile in res['outfiles']:
  m = t.upload_model(outfile, {"filename": outfile})
  sys.stdout.write('<img src="' + t.get_link(m['id']) + '" width="200" height="150">\n')
sys.stderr.write(json.dumps(res))

# render a single iso-view image with green material
green_material = renderer.make_material(name='default', diffuse_color=(0.24, 0.82, 0.32))
render = renderer.Render(filename='green_iso.png', camera_rotation=math.acos(2.0 / math.sqrt(5.0)), model_rotation=math.radians(315.0))
res = renderer.render_custom(filename, outdir='./', samples=4, renders=[ render ], default_material=green_material)
for outfile in res['outfiles']:
  m = t.upload_model(outfile, {"filename": outfile})
  sys.stdout.write('<img src="' + t.get_link(m['id']) + '" width="200" height="150">\n')
sys.stderr.write(json.dumps(res))
```

