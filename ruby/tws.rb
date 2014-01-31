require 'rubygems'
require 'restclient'
require 'open-uri'
require 'active_support/all'

class TWS
  
  def initialize opts={}
    @api_key = opts[:api_key] || 'bddc716254630f33f6e97e2636907c8e'
    @api_secret = opts[:api_secret] || '2de641661658231a4a7fe58de891e428'
    @stor_host = opts[:stor_host] || "https://stor.herokuapp.com"
    @auth_host = opts[:auth_host] || "https://threews.herokuapp.com"
    @api_version = opts[:api_version] || 1
    @default_expire = 1.hour
  end
  
  def signature string_to_sign
    raise "api_secret is not set" if @api_secret.blank?
    CGI.escape(
      Base64.encode64(
        OpenSSL::HMAC.digest(
          OpenSSL::Digest::Digest.new('sha1'),
          @api_secret,
          string_to_sign
        )
      ).strip
    )
  end
  
  def expire
    (Time.now + @default_expire).to_i
  end
  
  def authenticate
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{@api_version}/authenticate|
    response = RestClient.post "#{@auth_host}/api/v#{@api_version}/authenticate", {
      :key => @api_key,
      :signature => sig,
      :method => 'POST',
      :url => '/api/v1/authenticate',
      :options => nil,
      :expire => t
    }
    JSON.parse(response.body)
  end
  
  def create_model meta={}, upload_id=nil
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{@api_version}/models|
    auth_header = "3WS #{@api_key}:#{sig}"
    response = RestClient.post  "#{@stor_host}/api/v#{@api_version}/models?expire=#{t}",
                                {:meta => meta, :upload_id => upload_id},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_model id
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{@api_version}/models/#{id}|
    auth_header = "3WS #{@api_key}:#{sig}"
    response = RestClient.get  "#{@stor_host}/api/v#{@api_version}/models/#{id}?expire=#{t}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_models query={}
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{@api_version}/models|
    auth_header = "3WS #{@api_key}:#{sig}"
    f = URI.escape(query[:f]) rescue nil
    q = URI.escape(query[:q]) rescue nil
    m = URI.escape(query[:m]) rescue nil
    response = RestClient.get  "#{@stor_host}/api/v#{@api_version}/models?expire=#{t}&f=#{f}&q=#{q}&m=#{m}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def update_model id, meta={}
    t = expire
    sig = signature %|PUT\n\n#{t}\n/api/v#{@api_version}/models/#{id}|
    auth_header = "3WS #{@api_key}:#{sig}"
    response = RestClient.put  "#{@stor_host}/api/v#{@api_version}/models/#{id}?expire=#{t}",
                                {:meta => meta},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def delete_model id
    t = expire
    sig = signature %|DELETE\n\n#{t}\n/api/v#{@api_version}/models/#{id}|
    auth_header = "3WS #{@api_key}:#{sig}"
    response = RestClient.delete  "#{@stor_host}/api/v#{@api_version}/models/#{id}?expire=#{t}",
                                :Authorization => auth_header
    response.code
  end
  
  def presigned_upload_form
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{@api_version}/models/presign|
    auth_header = "3WS #{@api_key}:#{sig}"
    response = RestClient.get  "#{@stor_host}/api/v#{@api_version}/models/presign?expire=#{t}",
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def upload_model path, meta={}
    presign = presigned_upload_form
    begin
      upload_response = RestClient.post presign["form_action"],
                                        presign["form_fields"].merge({ :file => File.new(path), :multipart => true })
    rescue => e
      raise e.response
    end
    
    t = expire
    sig = signature %|POST\n\n#{t}\n/api/v#{@api_version}/models|
    auth_header = "3WS #{@api_key}:#{sig}"
    response = RestClient.post  "#{@stor_host}/api/v#{@api_version}/models?expire=#{t}",
                                {:meta => meta, :upload_id => presign["upload_id"]},
                                :Authorization => auth_header
    JSON.parse(response.body)
  end
  
  def get_link id
    t = expire
    sig = signature %|GET\n\n#{t}\n/api/v#{@api_version}/models/#{id}/download|
    "#{@stor_host}/api/v#{@api_version}/models/#{id}/download?expire=#{t}&key=#{@api_key}&sig=#{sig}"
  end
  
end