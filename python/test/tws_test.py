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

m = {}

class TestClass:
	def test_authentication(self):
		result = t.authenticate()
		assert result["authenticated"] == True

	def test_create_a_model(self):
		m = t.create_model({'name': "from py.test"})
		assert m["meta"]["name"] == "from py.test"
		assert t.get_model(m["id"])["id"] == m["id"]
		
	def test_upload_a_model(self):
		m = t.upload_model("tws_test.py", {'name': "tws_test.py"})
		assert m["meta"]["name"] == "tws_test.py"
		assert t.get_model(m["id"])["id"] == m["id"]
		
	def test_get_models(self):	
		assert t.get_models() != None

	def test_createSession(self):
		session = t.create_session()
		assert session["engine_version"] == 'latest'
		assert session["id"] != None
		assert t.get_session(session["id"])["id"] == session["id"]
		assert t.get_sessions()[0]["id"] == session["id"]