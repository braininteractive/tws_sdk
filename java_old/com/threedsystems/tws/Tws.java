package com.threedsystems.tws;

import java.net.URLEncoder;
import java.util.HashMap;


public class Tws {
	protected static String api_key = null;
	protected static String api_secret = null;
	protected static int api_version = 1;
	protected static int sdk_version = 3;
	protected static String stid_host = "https://stid.dddws.com";
	protected static String stom_host = "https://stom.dddws.com";
	protected static String stor_host = "https://stor.dddws.com";
	protected static int default_expire = 3600;
	protected static int default_timeout = 600;
	
	public enum ContentType {
		FORM,
		JSON,
		UPLOAD
	}
	
	public enum HttpMethod {
		POST,
		GET,
		PUT,
		DELETE
	}
	
	public Tws() throws Exception {
		Tws.api_key = "654a4195cab64a893357a3df3e3282b3";
		Tws.api_secret = "28a0f288a475cf0eddfa4f9939a122f5";
		
		if(Tws.api_key == null)
			throw new Exception("api_key cannot be omitted");
		
		if(Tws.api_secret == null)
			throw new Exception("api_secret cannot be omitted");
	}
	
	public String Authenticate() {
		
		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/authenticate";
		String strSignature = TwsUtil.GetSignature("POST", strEndpoint, iExpire);
		String strRequestUrl = Tws.stid_host + "/api/v" + Tws.api_version + strEndpoint;
		
		HashMap<String, Object> options = new HashMap<String, Object>();
		options.put("key", Tws.api_key);
		options.put("signature", URLEncoder.encode(strSignature));
		options.put("method", "POST");
		options.put("url", "/api/v" + Tws.api_version + strEndpoint);
		options.put("expire", String.valueOf(iExpire));
		
		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.FORM, strRequestUrl, options);
	}
	
	public int GetSDKVersion() {
		return Tws.sdk_version;
	}
}
