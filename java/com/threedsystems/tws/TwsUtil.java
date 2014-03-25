package com.threedsystems.tws;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.sql.Timestamp;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;

import org.apache.commons.codec.binary.Base64;

import com.google.gson.Gson;

public class TwsUtil extends Tws{

	public TwsUtil() throws Exception {
		super();
	}

	protected static String GetSignature(String strMethod, String strEndpoint, int iExpire) {
		
		if(iExpire == 0)
			iExpire = GetExpire();
		
		String strBaseString = strMethod + "\n" + "\n" + iExpire + "\n" + "/api/v" + Tws.api_version + strEndpoint;
		SecretKeySpec key = null;
		Mac mac = null;
		byte[] bytes = null;
		
		try {
			key = new SecretKeySpec((Tws.api_secret).getBytes("UTF-8"), "HmacSHA1");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		
		try {
			mac = Mac.getInstance("HmacSHA1");
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		}
		
		try {
			mac.init(key);
		} catch (InvalidKeyException e) {
			e.printStackTrace();
		}
		
		try {
			bytes = mac.doFinal(strBaseString.getBytes("UTF-8"));
		} catch (IllegalStateException e) {
			e.printStackTrace();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		
		return URLEncoder.encode(Base64.encodeBase64String(bytes));
	}
	
	protected static String GetAuthHeader(String strSignature) {
		return "3WS " + Tws.api_key + ":" + strSignature;
	}
	
	protected static int GetExpire() {
		Date date = new Date();
		Timestamp timestamp = new Timestamp(date.getTime());
		
		return (int) (timestamp.getTime()/1000) + Tws.default_expire;
	}
	
	protected static String GetHttpResponse(HttpMethod Method, ContentType Type, String strUrl, HashMap<String, Object>options) {

		URL url = null;
		HttpURLConnection conn = null;
		String strResult = "";
		String strPostFields = "";
		final String strBoundary = "--tws_boundary--";
		String strHypen = "--";
		String EOL = "\r\n";
		String strFilepath = "";
				
		try {
			url = new URL(strUrl);
		} catch(Exception ee) {
			System.out.println(ee.toString());
		}
		
		try {
			conn = (HttpURLConnection)url.openConnection();
			conn.setDoInput(true);
			conn.setDoOutput(true);
			conn.setUseCaches(false);
			conn.setConnectTimeout(Tws.default_timeout);
			
			if(options.keySet().contains("Authorization")) {
				String strAuthorization = (String)options.get("Authorization");
				conn.setRequestProperty("Authorization", strAuthorization);
			}	
			
			if(Method == HttpMethod.PUT){
				 ((HttpURLConnection)conn).setRequestMethod("PUT");
			} else if(Method == HttpMethod.DELETE) {
				 ((HttpURLConnection)conn).setRequestMethod("DELETE");				
			} else if(Method == HttpMethod.POST) {
				((HttpURLConnection)conn).setRequestMethod("POST");
			} else if(Method == HttpMethod.GET) {
				((HttpURLConnection)conn).setRequestMethod("GET");
			}			
			
			switch(Type) {
			case FORM:
				conn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded; charset=utf8");
				strPostFields = mapToQueryString(options);

				break;
			case JSON:
				conn.setRequestProperty("Content-Type", "application/json; charset=utf8");
				Gson json = new Gson();
				strPostFields = json.toJson(options);
				break;

			case UPLOAD:
				conn.setRequestProperty("Connection", "Keep-Alive");
				conn.setRequestProperty("Content-Type", "multipart/form-data; boundary=" + strBoundary);
				strFilepath = (String) options.get("path");

				break;
			default:
				break;
			}
			
			if(Method != HttpMethod.GET ){
				DataOutputStream dos = new DataOutputStream(new BufferedOutputStream(conn.getOutputStream()));
				
				switch(Type) {
					case FORM:				
					case JSON:
						dos.writeBytes(strPostFields);
						dos.flush();
						break;
						
					case UPLOAD:
						
						File file = new File(strFilepath);
						String strFilename = file.getName();
						StringBuffer postData = new StringBuffer();
						
						postData.append(strHypen + strBoundary).append(EOL);
						postData.append("Content-Disposition: form-data; name=\"AWSAccessKeyId\"");
						postData.append(EOL + EOL);
						postData.append((String)options.get("AWSAccessKeyId")).append(EOL);
						
						postData.append(strHypen + strBoundary).append(EOL);
						postData.append("Content-Disposition: form-data; name=\"key\"");
						postData.append(EOL + EOL);
						postData.append((String)options.get("key")).append(EOL);
						
						postData.append(strHypen + strBoundary).append(EOL);
						postData.append("Content-Disposition: form-data; name=\"policy\"");
						postData.append(EOL + EOL);
						postData.append((String)options.get("policy")).append(EOL);
						
						postData.append(strHypen + strBoundary).append(EOL);
						postData.append("Content-Disposition: form-data; name=\"signature\"");
						postData.append(EOL + EOL);
						postData.append((String)options.get("signature")).append(EOL);
												
						postData.append(strHypen + strBoundary).append(EOL);
						postData.append("Content-Disposition: form-data; name=\"file\"; ");					
						postData.append("filename=\"" + strFilename + "\"").append(EOL);
						
						postData.append(EOL);
						dos.writeBytes(postData.toString());

						// create a buffer of  maximum size
						FileInputStream fileInputStream = new FileInputStream(file);
		                int bytesAvailable = fileInputStream.available(); 
		                int maxBufferSize = 1 * 1024 * 1024; 
		                int bytesRead;
		                byte[] buffer;
		                int bufferSize = Math.min(bytesAvailable, maxBufferSize);
		                buffer = new byte[bufferSize];
		          
	                   // read file and write it into form...
		                bytesRead = fileInputStream.read(buffer, 0, bufferSize);  
	                      
		                while (bytesRead > 0) {
							dos.write(buffer, 0, bufferSize);
							bytesAvailable = fileInputStream.available();
							bufferSize = Math.min(bytesAvailable, maxBufferSize);
							bytesRead = fileInputStream.read(buffer, 0, bufferSize);
	                    }
		                
		                fileInputStream.close();						
						postData = new StringBuffer();
						postData.append(EOL + strHypen + strBoundary + strHypen).append(EOL);
						
						dos.writeBytes(postData.toString());
		                dos.flush();
		                dos.close();					
					default:
						break;
				}
			}

			InputStream is = conn.getInputStream();
			InputStreamReader isr = new InputStreamReader(is,"UTF-8");
			BufferedReader br = new BufferedReader(isr);
			String line;

			while((line = br.readLine()) != null) {
				strResult += line;
			}

			br.close();
			
		} catch(Exception ee) {
			ee.printStackTrace();
		}

		return strResult;
	}

	protected static String mapToQueryString(Map<String, Object> map) {
	    StringBuilder string = new StringBuilder();
	    int iMapSize = map.size();
	    int iCount = 1;

	    for(Entry<String, Object> entry : map.entrySet()) {
	        string.append(entry.getKey());
	        string.append("=");
	        string.append(entry.getValue());
	        
	        if(iCount < iMapSize)
	        	string.append("&");
	        iCount++;
	    }

	    return string.toString();
	}
}