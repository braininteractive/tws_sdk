require 'rubygems'
require 'restclient'
require 'open-uri'
require 'active_support/all'

class TWS
  
  attr_accessor :api_key, :api_secret, :stid_host, :stor_host, :stom_host, :stopp_host, :api_version, :default_expire
  
  def initialize opts={}
    self.api_key = opts[:api_key] || ENV['TWS_API_KEY']
    self.api_secret = opts[:api_secret] || ENV['TWS_API_SECRET']
    self.stopp_host = opts[:stopp_host] || "https://stopp.dddws.com"
    self.stom_host = opts[:stom_host] || "https://stom.dddws.com"
    self.stor_host = opts[:stor_host] || "https://stor.dddws.com"
    self.stid_host = opts[:stid_host] || "https://stid.dddws.com"
    self.api_version = opts[:api_version] || 1
    self.default_expire = 1.hour
  end

  def signature string_to_sign
    raise "api_secret is not set" if api_secret.blank?
    CGI.escape(
      Base64.encode64(
        OpenSSL::HMAC.digest(
          OpenSSL::Digest::Digest.new('sha1'),
          api_secret,
          string_to_sign
        )
      ).strip
    )
  end
  
  def expire t = nil
    t = default_expire if t.nil?
    (Time.now + t).to_i
  end
  
  def authenticate
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{api_version}/authenticate|
    response = RestClient.post "#{stid_host}/api/v#{api_version}/authenticate", {
      :key => api_key,
      :signature => sig,
      :method => 'POST',
      :url => "/api/v#{api_version}/authenticate",
      :options => nil,
      :expire => t
    }
    JSON.parse(response.body)
  end
  
  def create_model meta={}, upload_id=nil
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{api_version}/models|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.post  "#{stor_host}/api/v#{api_version}/models?expire=#{t}",
                                {:meta => meta, :upload_id => upload_id},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_model id
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/models/#{id}|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get  "#{stor_host}/api/v#{api_version}/models/#{id}?expire=#{t}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_models params={}
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/models|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get  "#{stor_host}/api/v#{api_version}/models?expire=#{t}&#{params.to_query}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def update_model id, meta={}
    t = expire
    sig = signature %|PUT\n\n#{t}\n/api/v#{api_version}/models/#{id}|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.put  "#{stor_host}/api/v#{api_version}/models/#{id}?expire=#{t}",
                                {:meta => meta},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def delete_model id
    t = expire
    sig = signature %|DELETE\n\n#{t}\n/api/v#{api_version}/models/#{id}|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.delete  "#{stor_host}/api/v#{api_version}/models/#{id}?expire=#{t}",
                                :Authorization => auth_header
    response.code
  end
  
  def presigned_upload_form starts_with={}, ip=""
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{api_version}/models/presign|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.post  "#{stor_host}/api/v#{api_version}/models/presign?expire=#{t}",
                                {:ip => ip, :starts_with => starts_with},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def upload_model path, opts={}
    presign = presigned_upload_form(opts[:starts_with], opts[:ip])
    begin
      upload_response = RestClient.post presign["form_action"],
                                        presign["form_fields"].merge({ :file => File.new(path), :multipart => true })
    rescue => e
      raise e
    end
    
    raise upload_response.body unless upload_response.code == 204
    
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{api_version}/models|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.post  "#{stor_host}/api/v#{api_version}/models?expire=#{t}",
                                {:meta => opts[:meta], :upload_id => presign["upload_id"]},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_link id, filename=nil, expire_time=nil, content_type=nil
    t = expire expire_time
    if filename.nil?
      sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/models/#{id}/download|
      "#{stor_host}/api/v#{api_version}/models/#{id}/download?expire=#{t}&key=#{api_key}&signature=#{sig}&content_type=#{content_type}"
    else
      sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/models/#{id}/#{URI.escape(filename)}|
      "#{stor_host}/api/v#{api_version}/models/#{id}/#{URI.escape(filename)}?expire=#{t}&key=#{api_key}&signature=#{sig}&content_type=#{content_type}"
    end
  end
  
  def request_tptx opts={}
    re = RestClient.post "https://cloud3d.herokuapp.com/api/v1/jobs", {
      :auth_token => 'XyDJkYZ9iwJyeqqaBqWf',
      :input_url => opts[:input_url],
      :input_type => opts[:input_type],
      :include_thumbnail => opts[:include_thumbnail]
    }
    JSON.parse(re.body)
  end
  
  def poll_tptx job_id
    re = RestClient.get "https://cloud3d.herokuapp.com/api/v1/jobs/#{job_id}?auth_token=XyDJkYZ9iwJyeqqaBqWf"
    JSON.parse(re.body)
  end
  
  def get_sessions params={}
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/sessions|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get  "#{stom_host}/api/v#{api_version}/sessions?expire=#{t}&#{params.to_query}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_session id
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/sessions/#{id}|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get  "#{stom_host}/api/v#{api_version}/sessions/#{id}?expire=#{t}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def create_session timeout=60, engine_version='latest'
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{api_version}/sessions|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.post  "#{stom_host}/api/v#{api_version}/sessions?expire=#{t}",
                                {:timeout => timeout, :engine_version => engine_version},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def close_session id
    t = expire
    sig = signature %|DELETE\n\n#{t}\n/api/v#{api_version}/sessions/#{id}|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.delete  "#{stom_host}/api/v#{api_version}/sessions/#{id}?expire=#{t}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end

  def create_run id, platform, code
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{api_version}/sessions/#{id}/runs|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.post  "#{stom_host}/api/v#{api_version}/sessions/#{id}/runs?expire=#{t}",
                                {:platform => platform, :code => code},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_runs id, platform=""
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/sessions/#{id}/runs|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get  "#{stom_host}/api/v#{api_version}/sessions/#{id}/runs?expire=#{t}&platform=#{platform}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_run session_id, run_id
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/sessions/#{session_id}/runs/#{run_id}|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get  "#{stom_host}/api/v#{api_version}/sessions/#{session_id}/runs/#{run_id}?expire=#{t}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  # STOPP
  def get_printers params={}
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/printers|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get "#{stopp_host}/api/v#{api_version}/printers?expire=#{t}&#{params.to_query}", :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_materials params={}
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/materials|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get "#{stopp_host}/api/v#{api_version}/materials?expire=#{t}&#{params.to_query}", :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_properties params={}
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{api_version}/properties|
    auth_header = "3WS #{api_key}:#{sig}"
    response = RestClient.get "#{stopp_host}/api/v#{api_version}/properties?expire=#{t}&#{params.to_query}", :Authorization => auth_header
    JSON.parse(response.body)
  end
  
end