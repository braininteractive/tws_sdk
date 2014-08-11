package com.threedsystems.tws;

import java.util.HashMap;

public class StomClient extends Tws{

	public StomClient() throws Exception {
		super();
	}

	public String GetSessions(HashMap<String, Object> params) {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/sessions";
		String strSignature = TwsUtil.GetSignature("GET", strEndpoint, iExpire);
		String strRequestUrl = Tws.stom_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire; // + "&" + TwsUtil.mapToQueryString(params);
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.GET, ContentType.JSON, strRequestUrl, options);
	}
/*	
	public HashMap<String, Object> GetSessions(HashMap<String, Object> params) {
		
		Gson json = new Gson();
		HashMap<String, Object> mapResult = null;
		mapResult = json.fromJson(GetSessionsJson(params), mapResult.getClass());
		
		return mapResult;
	}
*/	
	
	public String GetSession(String strSessionId) {

		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/sessions/" + strSessionId;
		String strSignature = TwsUtil.GetSignature("GET", strEndpoint, iExpire);
		String strRequestUrl = Tws.stom_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.GET, ContentType.JSON, strRequestUrl, options);
	}
	
	public String CreateSession(int timeout, String strEngineVersion) {
			
		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/sessions";
		String strSignature = TwsUtil.GetSignature("POST", strEndpoint, iExpire);
		String strRequestUrl = Tws.stom_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
			options.put("timeout", timeout);
			options.put("engine_version", strEngineVersion);
		}

		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.JSON, strRequestUrl, options);
	}

	public String CreateSession(int timeout) {
		return CreateSession(timeout, "latest");
	}
	
	public String CreateSession() {
		return CreateSession(60, "latest");
	}
	
	public String CloseSession(String strSessionId) {
		
		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/sessions/" + strSessionId;
		String strSignature = TwsUtil.GetSignature("DELETE", strEndpoint, iExpire);
		String strRequestUrl = Tws.stom_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.DELETE, ContentType.JSON, strRequestUrl, options);
	}
	
	public String CreateRun(String strSessionId, String strPlatform, String strCode) {
		
		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/sessions/" + strSessionId + "/runs";
		String strSignature = TwsUtil.GetSignature("POST", strEndpoint, iExpire);
		String strRequestUrl = Tws.stom_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
			options.put("platform", strPlatform);
			options.put("code", strCode);
		}

		return TwsUtil.GetHttpResponse(HttpMethod.POST, ContentType.JSON, strRequestUrl, options);
	}
	
	public String GetRuns(String strSessionId, String strPlatform) {
		
		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/sessions/" + strSessionId + "/runs";
		String strSignature = TwsUtil.GetSignature("GET", strEndpoint, iExpire);
		String strRequestUrl = Tws.stom_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
			options.put("platform", strPlatform);
		}

		return TwsUtil.GetHttpResponse(HttpMethod.GET, ContentType.JSON, strRequestUrl, options);
	}
	
	public String GetRun(String strSessionId, String strRunId) {
		
		int iExpire = TwsUtil.GetExpire();
		String strEndpoint = "/sessions/" + strSessionId + "/runs/" + strRunId;
		String strSignature = TwsUtil.GetSignature("GET", strEndpoint, iExpire);
		String strRequestUrl = Tws.stom_host + "/api/v" + Tws.api_version + strEndpoint + "?expire=" + iExpire;
				
		HashMap<String, Object> options = new HashMap<String, Object>();
		{
			options.put("Authorization", TwsUtil.GetAuthHeader(strSignature));
		}

		return TwsUtil.GetHttpResponse(HttpMethod.GET, ContentType.JSON, strRequestUrl, options);
	}		
}
