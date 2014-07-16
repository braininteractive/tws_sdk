from cmr import Cmr
import json

c = Cmr(api_key='b1828e9271c2a018350ac852bd669f24', api_secret='ce835fcc7e3369a1ebdbce28accd1771')

result = c.convert("e3cdba7295")
print(json.dumps(result))