import cgi
import base64
import hmac
import hashlib
import urllib
import time
import requests
import json
import os

class Tws:
  def __init__( self, api_key = None, api_secret = None,
                stom_host = "https://stom.herokuapp.com",
                stor_host = "https://stor.herokuapp.com",
                stid_host = "https://stid.herokuapp.com",
                api_version = 1 ):

    if not api_key:
      try:
        api_key = os.environ['TWS_API_KEY']
      except:
        pass

    if not api_secret:
      try:
        api_secret = os.environ['TWS_API_SECRET']
      except:
        pass

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
      data=json.dumps({"meta": meta, "upload_id": upload_id}),
      headers={'Authorization': self.authorization('POST', '/models'), 'Content-Type': 'application/json'}
    )
    if response.status_code == 201:
      return response.json()
    else:
      return response.status_code

  def get_model(self, mid):
    self.set_expire()
    response = requests.get(
      self.endpoint('stor', '/models/%s' % mid),
      headers={'Authorization': self.authorization('GET', '/models/%s' % mid)}
    )
    if response.status_code == 200:
      return response.json()
    else:
      return response.status_code

  def get_models(self, params={}):
    self.set_expire()
    response = requests.get(
      self.endpoint('stor', '/models', extra_params=params),
      headers={'Authorization': self.authorization('GET', '/models')}
    )
    if response.status_code == 200:
      return response.json()
    else:
      return response.status_code
  
  def update_model(self, mid, meta={}):
    self.set_expire()
    response = requests.put(
      self.endpoint('stor', '/models/%s' % mid),
      data=json.dumps({"meta": meta}),
      headers={'Authorization': self.authorization('PUT', '/models/%s' % mid), 'Content-Type': 'application/json'}
    )
    if response.status_code == 200:
      return response.json()
    else:
      return response.status_code
  
  def delete_model(self, mid):
    self.set_expire()
    response = requests.delete(
      self.endpoint('stor', '/models/%s' % mid),
      headers={'Authorization': self.authorization('DELETE', '/models/%s' % mid)}
    )
    return response.status_code
    
  def presigned_upload_form(self):
    self.set_expire()
    response = requests.get(
      self.endpoint('stor', '/models/presign'),
      headers = { 'Authorization': self.authorization('GET', '/models/presign') }
    )
    if response.status_code == 200:
      return response.json()
    else:
      return None

  def upload_model(self, path, meta={}):
    self.set_expire()
    presign = self.presigned_upload_form()
    files = {'file': open(path, 'rb')}
    upload_response = requests.post(
      presign["form_action"],
      data=presign["form_fields"],
      files=files
    )
    return self.create_model(meta, presign['upload_id'])
    
  def get_link(self, mid, filename=''):
    self.set_expire()
    sig = self.signature(self.string_to_sign('GET', '/models/%s/download' % mid))
    escaped_filename = urllib.parse.quote_plus(filename)
    return "%s/api/v%s/models/%s/download?expire=%s&key=%s&signature=%s&filename=%s" % (self.stor_host, self.api_version, mid, self.expire, self.api_key, sig, escaped_filename)
    
  def get_sessions(self):
    self.set_expire()
    response = requests.get(
      self.endpoint('stom', '/sessions'),
      headers = { 'Authorization': self.authorization('GET', '/sessions') }
    )
    if response.status_code == 200:
      return response.json()
    else:
      return response.status_code

  def get_session(self, sid):
    self.set_expire()
    response = requests.get(
      self.endpoint('stom', '/sessions/%s' % sid),
      headers = { 'Authorization': self.authorization('GET', '/sessions/%s' % sid) }
    )
    if response.status_code == 200:
      return response.json()
    else:
      return response.status_code

  def create_session(self, timeout=60):
    self.set_expire()
    response = requests.post(
      self.endpoint('stom', '/sessions'),
      data = json.dumps({"timeout": timeout}),
      headers = { 'Authorization': self.authorization('POST', '/sessions'), 'Content-Type': 'application/json' }
    )
    if response.status_code == 201:
      return response.json()
    else:
      return response.status_code

  def close_session(self, sid):
    self.set_expire()
    response = requests.delete(
      self.endpoint('stom', '/sessions/%s' % sid),
      headers = { 'Authorization': self.authorization('DELETE', '/sessions/%s' % sid) }
    )
    if response.status_code == 200:
      return response.json()
    else:
      return response.status_code

  def create_run(self, sid, code):
    self.set_expire()
    response = requests.post(
      self.endpoint('stom', '/sessions/%s/runs' % sid),
      data = json.dumps({'code':code}),
      headers = { 'Authorization': self.authorization('POST', '/sessions/%s/runs' % sid), 'Content-Type': 'application/json' }
    )
    if response.status_code == 201:
      return response.json()
    else:
      return response.status_code

  def get_runs(self, sid):
    self.set_expire()
    response = requests.get(
      self.endpoint('stom', '/sessions/%s/runs' % sid),
      headers = { 'Authorization': self.authorization('GET', '/sessions/%s/runs' % sid)}
    )
    if response.status_code == 200:
      return response.json()
    else:
      return response.status_code