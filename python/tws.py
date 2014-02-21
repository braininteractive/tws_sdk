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
    self.expire = None
  
  def signature(self, string_to_sign):
    return urllib.parse.quote_plus(base64.b64encode(hmac.new(self.api_secret.encode(),string_to_sign.encode(),hashlib.sha1).digest()).decode())
            
  def set_expire(self):
    self.expire = int(time.time()) + self.default_expire
  
  def string_to_sign(self, method, endpoint):
    return "%s\n\n%d\n/api/v%d%s" % (method, self.expire, self.api_version, endpoint)
    
  def authorization(self, method, endpoint):
    return "3WS %s:%s" % (self.api_key, self.signature(self.string_to_sign(method, endpoint)))
  
  def endpoint(self, host, path, extra_params={}):
    extra = urllib.parse.urlencode(extra_params)
    return "%s/api/v%s%s?expire=%s&%s" % (eval("self.%s_host" % host), self.api_version, path, self.expire, extra)
  
  # stid test authentication
  def authenticate(self):
    self.set_expire()
    sig = self.signature(self.string_to_sign('POST', '/authenticate'))
    data = {
      'key': self.api_key,
      'signature': sig,
      'method': 'POST',
      'url': '/api/v1/authenticate',
      'options': '',
      'expire': self.expire
    }
    return requests.post("%s/api/v%d/authenticate" % (self.stid_host, self.api_version), data=data).json()
  
  def create_model(self, meta={}, upload_id=None):
    self.set_expire()
    response = requests.post(
      self.endpoint('stor', '/models'),
      data={"meta": meta, "upload_id": upload_id},
      headers={'Authorization': self.authorization('POST', '/models')}
    )
    return response.json()

  def get_model(self, mid):
    self.set_expire()
    response = requests.get(
      self.endpoint('stor', '/models/%s' % mid),
      headers={'Authorization': self.authorization('GET', '/models/%s' % mid)}
    )
    return response.json()

  def get_models(self, params={}):
    self.set_expire()
    response = requests.get(
      self.endpoint('stor', '/models', extra_params=params),
      headers={'Authorization': self.authorization('GET', '/models')}
    )
    return response.json()
  
  #def update_model(self, meta={}):
  
  #def delete_model(self, mid):
    
  def presigned_upload_form(self):
    self.set_expire()
    response = requests.get(
      self.endpoint('stor', '/models/presign'),
      headers = { 'Authorization': self.authorization('GET', '/models/presign') }
    )
    return response.json()

  def upload_model(self, path, meta={}):
    presign = self.presigned_upload_form()
    upload_response = requests.post(
      presign["form_action"],
      presign["form_fields"]
    )

    self.set_expire()
    auth_header = self.authorization("POST\n\n%s\n/api/v%s/models" % (t, self.api_version))
    response = requests.post(
      self.endpoint('stor', '/models'),
      {"meta": meta, "upload_id": presign["upload_id"]},
      headers = {"Authorization": self.authorization('POST', '/models')}
    )
    return response.json()