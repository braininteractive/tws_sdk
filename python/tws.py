import cgi
import base64
import hmac
import hashlib
import urllib
import time
import requests
import json

class Tws:
  
  def __init__( self, api_key = None, api_secret = None,
                stom_host = "https://stom.herokuapp.com",
                stor_host = "https://stor.herokuapp.com",
                stid_host = "https://stid.herokuapp.com",
                api_version = 1 ):
    self.api_key = api_key
    self.api_secret = api_secret
    self.stom_host = stom_host
    self.stor_host = stor_host
    self.stid_host = stid_host
    self.api_version = api_version
    self.default_expire = 3600
  
  def signature(self, string_to_sign):
    return urllib.parse.quote_plus(base64.b64encode(hmac.new(self.api_secret.encode(),string_to_sign.encode(),hashlib.sha1).digest()).decode())
            
  def expire(self):
    return int(time.time()) + self.default_expire
  
  def authenticate(self):
    t = self.expire()
    sig = self.signature("POST\n\n%d\n/api/v%d/authenticate" % (t, self.api_version))
    data = {
      'key': self.api_key,
      'signature': sig,
      'method': 'POST',
      'url': '/api/v1/authenticate',
      'options': '',
      'expire': t
    }
    return requests.post("%s/api/v%d/authenticate" % (self.stid_host, self.api_version), data=data).json()