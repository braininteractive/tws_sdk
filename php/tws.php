<?
class Tws{
  var $api_key = null;
  var $api_secret = null;
  var $api_version = 1;
  var $sdk_version = 2;
  var $stid_host = "https://stid.dddws.com";
  var $stom_host = "https://stom.dddws.com";
  var $stor_host = "https://stor.dddws.com";
  var $default_expire = 3600;
  var $default_timeout = 600; 

/******************************************************************************
 Initialize and Authenticate
******************************************************************************/

  function __construct($params = array()){
    if(!is_array($params)) throw new Exception("params should be an array");

    if(array_key_exists("api_key", $params))
      $this->api_key = $params["api_key"];
    else
      $this->api_key = $_ENV["TWS_API_KEY"];

    if(array_key_exists("api_secret", $params))
      $this->api_secret = $params["api_secret"];
    else
      $this->api_secret = $_ENV["TWS_API_SECRET"];

    if($this->api_key == null) throw new Exception("api_key cannot be omitted");
    if($this->api_secret == null) throw new Exception("api_secret cannot be omitted");
  }

  function authenticate(){
    $t = $this->get_expire();
    $endpoint = "/authenticate";
    $signature = $this->get_signature("POST", $endpoint, $t);
    $request_url = $this->stid_host . "/api/v" . $this->api_version . $endpoint;

    $options = array('key' => $this->api_key,
            'signature' => $signature,
            'method' => 'POST',
            'url' => "/api/v" . $this->api_version . $endpoint,
            'options' => null,
            'expire' => $t);
    return $this->get_response("POST",$request_url, $options);
  }

/******************************************************************************
 STOR
******************************************************************************/

  function create_model($meta = null, $upload_id = null) {
    if(!is_array($meta)) $meta = json_decode($meta);

    $t = $this->get_expire();
    $endpoint = "/models";
    $signature = $this->get_signature("POST", $endpoint, $t);
    $request_url = $this->stor_host . "/api/v" . $this->api_version . $endpoint . '?expire=' . $t;

    $options = array('meta' => $meta, 'upload_id' => $upload_id, 'Authorization' => $this->get_auth_header($signature));
    return $this->get_response("POST", $request_url, $options);
  }

  function get_models($params = array()) {
    $t = $this->get_expire();
    $endpoint = "/models";
    $signature = $this->get_signature("GET", $endpoint, $t);
    $request_url = $this->stor_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t . "&" . http_build_query($params, '', '&');

    $options = array('Authorization' => $this->get_auth_header($signature));
    return $this->get_response("GET", $request_url, $options);
  }

  function get_model($id = null) {
    if($id == null) throw new Exception("id is missing");

    $t = $this->get_expire();
    $endpoint = "/models/" . $id;
    $signature = $this->get_signature("GET", $endpoint, $t);
    $request_url = $this->stor_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array('Authorization' => $this->get_auth_header($signature));
    return $this->get_response("GET", $request_url, $options);
  }

  function update_model($id = null, $meta = null) {
    if($id == null) throw new Exception("id is missing");
    if(!is_array($meta)) $meta = json_decode($meta);

    $t = $this->get_expire();
    $endpoint = "/models/" . $id;
    $signature = $this->get_signature("PUT", $endpoint, $t);
    $request_url = $this->stor_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array('Authorization' => $this->get_auth_header($signature), 'meta' => $meta);
    return $this->get_response("PUT", $request_url, $options);
  }

  function delete_model($id = null) {
    if($id == null) throw new Exception("id is missing");

    $t = $this->get_expire();
    $endpoint = "/models/" . $id;
    $signature = $this->get_signature("DELETE", $endpoint, $t);
    $request_url = $this->stor_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array('Authorization' => $this->get_auth_header($signature));
    return $this->get_response("DELETE", $request_url, $options);
  }

  private function presigned_upload_form($params = array(), $ip = null) {
    $t = $this->get_expire();
    $endpoint = "/models/presign";
    $signature = $this->get_signature("POST", $endpoint, $t);
    $request_url = $this->stor_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array("Authorization" => $this->get_auth_header($signature), "ip" => $ip, "starts_with" => $params);
    return $this->get_response("POST", $request_url, $options);
  }

  function upload_model($data = null, $params = array()) {
    if(!array_key_exists('method', $params)){
      $params['method'] = 'file';
    }

    $presigned_upload_form = $this->presigned_upload_form($params['starts_with'], $params['ip']);

    if($presigned_upload_form["status_code"] != 200)
      return $presigned_upload_form;

    $presign = json_decode($presigned_upload_form["contents"],1);

    if($params['method'] == 'file'){
      if(!file_exists($data)){
        return "File doesn't exist";
      }
      $presign["form_fields"]["file"] = "@".$data;
    } else if($params['method'] == 'data') {
      $presign["form_fields"]["file"] = $data;
    } else {
      return "Unknown method";
    }

    try {
      $upload_response = $this->get_response("POST", $presign["form_action"], $presign["form_fields"]);

      if($upload_response["status_code"] != 204)
        return $upload_response;

    } catch (Exception $ee) {
      return $ee;
    }

    $t = $this->get_expire();
    $endpoint = "/models";
    $signature = $this->get_signature("POST", $endpoint, $t);
    $request_url = $this->stor_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;
    $options = array("Authorization" => $this->get_auth_header($signature), "meta" => $params["meta"], "upload_id" => $presign["upload_id"]);
    return $this->get_response("POST", $request_url, $options);   
  }

  function get_link($id = null, $filename = null, $expire_sec = null) {
    if($id == null) throw new Exception("id is missing");

    $t = $this->get_expire($expire_sec);
    $endpoint = "/models/" . $id;
    
    if($filename == null) {
      $signature = $this->get_signature("GET", $endpoint . "/download", $t);
      $uri = $this->stor_host . "/api/v" . $this->api_version . $endpoint . "/download?expire=" . $t . "&key=" . $this->api_key . "&signature=" . $signature;
    } else {
      $signature = $this->get_signature("GET", $endpoint . "/" . rawurldecode($filename), $t);
      $uri = $this->stor_host . "/api/v" . $this->api_version . $endpoint . "/" . rawurldecode($filename)  . "?expire=" . $t . "&key=" . $this->api_key . "&signature=" . $signature;
    }
    return $uri;
  }

/******************************************************************************
 STOM 
******************************************************************************/

  function get_sessions($params = array()) {
    if(!is_array($params)) throw new Exception("params should be an array");

    $t = $this->get_expire();
    $endpoint = "/sessions";
    $signature = $this->get_signature("GET", $endpoint, $t);
    $request_url = $this->stom_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t . "&" . http_build_query($params, '', '&');

    $options = array('Authorization' => $this->get_auth_header($signature));
    return $this->get_response("GET", $request_url, $options);
  }

  function get_session($id = null) {
    if($id == null) throw new Exception("id is missing");

    $t = $this->get_expire();
    $endpoint = "/sessions/" . $id;
    $signature = $this->get_signature("GET", $endpoint, $t);
    $request_url = $this->stom_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array('Authorization' => $this->get_auth_header($signature));
    return $this->get_response("GET", $request_url, $options);
  }

  function create_session($timeout = 60, $engine_version = "latest") {
    $t = $this->get_expire();
    $endpoint = "/sessions";
    $signature = $this->get_signature("POST", $endpoint, $t);
    $request_url = $this->stom_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array("Authorization" => $this->get_auth_header($signature), "timeout" => $timeout, "engine_version" => $engine_version);
    return $this->get_response("POST", $request_url, $options);
  }

  function close_session($id = null) {
    if($id == null) throw new Exception("id is missing");

    $t = $this->get_expire();
    $endpoint = "/sessions/" . $id;
    $signature = $this->get_signature("DELETE", $endpoint, $t);
    $request_url = $this->stom_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array("Authorization" => $this->get_auth_header($signature));
    return $this->get_response("DELETE", $request_url, $options);
  }

  function create_run($id = null, $platform = null, $code = null){
    if($id == null) throw new Exception("id is missing");
    if($platform == null) throw new Exception("platform is missing");
    if($code == null) throw new Exception("code is null");

    $t = $this->get_expire();
    $endpoint = "/sessions/" . $id . "/runs";
    $signature = $this->get_signature("POST", $endpoint, $t);
    $request_url = $this->stom_host ."/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array("Authorization" => $this->get_auth_header($signature), "code" => $code, "platform" => $platform);
    return $this->get_response("POST", $request_url, $options);
  }

  function get_runs($id = null, $platform = null) {
    if($id == null) throw new Exception("id is missing");
    if($platform == null) throw new Exception("platform is missing");

    $t = $this->get_expire();
    $endpoint = "/sessions/" . $id . "/runs";
    $signature = $this->get_signature("GET", $endpoint, $t);
    $request_url = $this->stom_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t . "&platform" . $platform;

    $options = array("Authorization" => $this->get_auth_header($signature));
    return $this->get_response("GET", $request_url, $options);
  }

  function get_run($session_id = null, $id = null) {
    if($session_id == null) throw new Exception("session id is missing");
    if($id == null) throw new Exception("run id is missing");
    
    $t = $this->get_expire();
    $endpoint = "/sessions/" . $session_id . "/runs/" . $id;
    $signature = $this->get_signature("GET", $endpoint, $t);
    $request_url = $this->stom_host . "/api/v" . $this->api_version . $endpoint . "?expire=" . $t;

    $options = array("Authorization" => $this->get_auth_header($signature));
    return $this->get_response("GET", $request_url, $options);
  } 

/**********************************************************
 Common
***********************************************************/

  private function get_signature($method = null, $endpoint = null, $expire = null) {
    if($method == null) throw new Exception("method is missing");
    if($endpoint == null) throw new Exception("endpoint is missing");
    if($expire == null) throw new Exception("expire is missing");

    $base_string = $method . PHP_EOL . PHP_EOL . $expire . PHP_EOL . "/api/v" . $this->api_version . $endpoint;
    return rawurlencode(base64_encode(hash_hmac('sha1', $base_string, $this->api_secret, true)));
  }

  private function get_expire($expire_sec = null) {
    if($expire_sec == null) $expire_sec = $this->default_expire;
    return time() + $expire_sec;
  }

  private function get_auth_header($signature) {
    return "3WS " . $this->api_key . ':' . $signature;
  } 

  private function get_response($method = null, $url = null, $options = array()) {
    if($url == null) throw new Exception("url is missing");
    if($method == null) throw new Exception("method is missing");

    try {
      $ch = curl_init($url);
    } catch (Exception $ee) {
      return $ee;
    }

    $ch = curl_init();
    $http_header = array();
    $post_fields = null;

    if(array_key_exists('Authorization', $options)) {
      array_push($http_header, 'Content-Type: application/json; charset=utf-8');
      array_push($http_header, 'Authorization: ' . $options['Authorization']);
      $post_fields = json_encode($options);
    } else if(array_key_exists('AWSAccessKeyId', $options)) {
      array_push($http_header, 'Content-Type: multipart/form-data; charset=utf-8');
      $post_fields = $options;
    } else {
      array_push($http_header, 'Content-Type: application/x-www-form-urlencoded; charset=utf-8');
      $post_fields = http_build_query($options, '', '&');
    }
    
    curl_setopt($ch, CURLOPT_HTTPHEADER, $http_header);
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $this->default_timeout);

    if(strtoupper($method) == "POST") {
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_POSTFIELDS, $post_fields);
    }

    if(strtoupper($method) == "DELETE") {
      curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    if(strtoupper($method) == "PUT") {
      curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "PUT");
      curl_setopt($ch, CURLOPT_POSTFIELDS, $post_fields);
    }   

    try { 
      $g = curl_exec($ch);
      $status_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
      curl_close($ch);
    } catch(Exception $ee) {
      return $ee;
    }

    return array("status_code" => $status_code, "contents" => $g);
  }
}