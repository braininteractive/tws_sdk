package com.threedsystems.tws;

import com.threedsystems.tws.Bucket.*;
import java.net.URLEncoder;
import java.util.HashMap;
import com.google.gson.Gson;

public class StorClient extends Tws{

	public StorClient() throws Exception {
		super();
	}
	
	public String CreateModel(HashMap<String, Object> mapMeta, String strUploadId) {
		
		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models";
		String strSignature = TwsUtil.GetSignature("POST", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("meta", mapMeta);
			options.put("upload_id", strUploadId);
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.JSON, strRequestUrl, options);
	}
	
	public String CreateModel(HashMap<String, Object> mapMeta) {
		
		return CreateModel(mapMeta, null);
	}
	
	public String GetModels(HashMap<String, Object> params) {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models";
		String strSignature = TwsUtil.GetSignature("GET", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.GET, ContentType.JSON, strRequestUrl, options);
	}
	
	public String GetModels() {
		HashMap<String, Object> map = new HashMap<String, Object>();
		return GetModels(map);
	}
	
	public String GetModel(String strModelId) {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models/" + strModelId;
		String strSignature = TwsUtil.GetSignature("GET", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.GET, ContentType.JSON, strRequestUrl, options);
	}
	
	public String UpdateModel(String strModelId, HashMap<String,Object> mapMeta) {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models/" + strModelId;
		String strSignature = TwsUtil.GetSignature("PUT", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
			options.put("meta", mapMeta);
		}

		return TwsUtil.GetHttpResponse(HttpMethod.PUT, ContentType.JSON, strRequestUrl, options);
	}	
	
	public String DeleteModel(String strModelId) {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models/" + strModelId;
		String strSignature = TwsUtil.GetSignature("DELETE", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.DELETE, ContentType.JSON, strRequestUrl, options);
	}
	
	private static String PresignedUploadForm(HashMap<String, Object> params, String strIp) {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models/presign";
		String strSignature = TwsUtil.GetSignature("POST", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
			options.put("ip", strIp);
			options.put("starts_with", options.get("starts_with"));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.JSON, strRequestUrl, options);
	}
	
	public String UploadModel(String strFilePath, HashMap<String, Object> params) {
		
		Gson json = new Gson();
		String strUploadResponse = null;
		String strUploadUrl = null;
		HashMap<String, Object> mapFormFields = null;
		PresignedUploadForm form = null;
		
		try {
			String strJsonPresign = PresignedUploadForm((HashMap<String, Object>)params.get("starts_with"), (String)params.get("ip"));			
			form = json.fromJson(strJsonPresign, PresignedUploadForm.class);
			
			mapFormFields = form.getForm_fields();
			mapFormFields.put("path", strFilePath);
			
			form.setForm_fields(mapFormFields);
			strUploadUrl = form.getForm_action();			
		} catch(Exception ee) {
			return ee.toString();
		}
		
		try {
			strUploadResponse = TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.UPLOAD, strUploadUrl, mapFormFields);
		} catch(Exception ee) {
			return ee.toString();
		}

		int iExpire = TwsUtil.GetExpire();				
		String strEndpoint = "/models";
		String strSignature = TwsUtil.GetSignature("POST", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;

		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
			options.put("meta", (HashMap<String, Object>)params);
			options.put("upload_id", form.getUpload_id());
		}

		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.JSON, strRequestUrl, options);
	}
	
	public String GetLink(String strModelId, String strFileName, int expire_sec) {
		
		int iExpire = TwsUtil.GetExpire(expire_sec);
		String strEndpoint = "/models/" + strModelId;
		String strSignature = null;
		String strUri = null;
		
		if(strFileName.isEmpty()) {
			strSignature = TwsUtil.GetSignature("GET", strEndpoint + "download", iExpire);
			strUri = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "/download?expire=" + iExpire + "&key=" + Tws.api_key + "&signature=" + strSignature; 
		} else {
			strSignature = TwsUtil.GetSignature("GET", strEndpoint + URLEncoder.encode(strFileName), iExpire);
			strUri = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "/" + URLEncoder.encode(strFileName) + "?expire=" + iExpire + "&key=" + Tws.api_key + "&signature=" + strSignature;			
		}

		return strUri;		
	}

	public String GetLink(String strModelId, String strFileName) {
		return GetLink(strModelId, "", 0);
	}
	
	public String GetLink(String strModelId) {
		return GetLink(strModelId, "");
	}
}
