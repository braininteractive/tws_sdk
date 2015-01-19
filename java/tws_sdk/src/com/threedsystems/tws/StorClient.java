package com.threedsystems.tws;

import com.threedsystems.tws.Bucket.*;

import java.net.URLEncoder;
import java.util.HashMap;

import com.google.gson.Gson;

public class StorClient extends Tws{

	public StorClient() throws Exception {
		super();
	}
	
	public String CreateModel(HashMap<String, Object> mapMeta, String strUploadId) throws Exception {
		
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

		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.JSON, strRequestUrl, options).Body;
	}
	
	public String CreateModel(HashMap<String, Object> mapMeta) throws Exception {
		
		return CreateModel(mapMeta, null);
	}
	
	public String GetModels(HashMap<String, Object> params) throws Exception {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models";
		String strSignature = TwsUtil.GetSignature("GET", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.GET, ContentType.JSON, strRequestUrl, options).Body;
	}
	
	public String GetModels() throws Exception {
		HashMap<String, Object> map = new HashMap<String, Object>();
		return GetModels(map);
	}
	
	public String GetModel(String strModelId) throws Exception {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models/" + strModelId;
		String strSignature = TwsUtil.GetSignature("GET", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.GET, ContentType.JSON, strRequestUrl, options).Body;
	}
	
	public String UpdateModel(String strModelId, HashMap<String,Object> mapMeta) throws Exception {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models/" + strModelId;
		String strSignature = TwsUtil.GetSignature("PUT", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
			options.put("meta", mapMeta);
		}

		return TwsUtil.GetHttpResponse(HttpMethod.PUT, ContentType.JSON, strRequestUrl, options).Body;
	}	
	
	public String DeleteModel(String strModelId) throws Exception {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models/" + strModelId;
		String strSignature = TwsUtil.GetSignature("DELETE", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.DELETE, ContentType.JSON, strRequestUrl, options).Body;
	}
	
	private static String PresignedUploadForm(HashMap<String, Object> params) throws Exception {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/models/presign";
		String strSignature = TwsUtil.GetSignature("POST", strEndpoint, iExpire);
		String strRequestUrl = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
			options.put("ip", params.get("ip"));
			options.put("starts_with", params.get("starts_with"));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.JSON, strRequestUrl, options).Body;
	}
	
	public String UploadModel(String strFilePath) throws Exception {
		return UploadModel(strFilePath, null);
	}
	
	public String UploadModel(String strFilePath, HashMap<String, Object> params) throws Exception {
		
		Gson json = new Gson();
		String strUploadUrl = null;
		HashMap<String, Object> mapFormFields = null;
		PresignedUploadForm form = null;
		
		if(params == null){
			params = new HashMap<String, Object>();
			params.put("starts_with", null);
			params.put("ip", null);
		}
		
		try {
			String strJsonPresign = PresignedUploadForm(params);			
			form = json.fromJson(strJsonPresign, PresignedUploadForm.class);
			
			mapFormFields = form.getForm_fields();
			mapFormFields.put("path", strFilePath);
			
			form.setForm_fields(mapFormFields);
			strUploadUrl = form.getForm_action();			
		} catch(Exception ee) {
			return ee.toString();
		}
		
		try {
			HttpResponse response = TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.UPLOAD, strUploadUrl, mapFormFields);
			
			if(response.StatusCode != 204)
				throw new Exception(response.Body);
		} catch(Exception e) {
			throw e;
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

		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.JSON, strRequestUrl, options).Body;
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
			try
			{
				strSignature = TwsUtil.GetSignature("GET", strEndpoint + URLEncoder.encode(strFileName, "UTF-8"), iExpire);
				strUri = Tws.stor_host + "/api/v" + Tws.api_version + strEndpoint + "/" + URLEncoder.encode(strFileName, "UTF-8") + "?expire=" + iExpire + "&key=" + Tws.api_key + "&signature=" + strSignature;
			}
			catch(Exception e)
			{
				return e.toString();
			}
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
