# TO Install pytest: 
# $ sudo easy_install -U pytest

import os, sys
import json
sys.path.append(os.path.abspath('../'))
from tws import Tws

t = Tws(api_key=os.environ["TWS_STAGING_KEY"], api_secret=os.environ["TWS_STAGING_SECRET"], 
	stom_host = "https://stom-staging.herokuapp.com",
  stor_host = "https://stor-staging.herokuapp.com",
  stid_host = "https://stid-staging.herokuapp.com")
session = None

class TestClass:
	def test_createSession(self):
		session = t.create_session()
		assert session["engine_version"] == 'latest'
		assert session["id"] != None
		
