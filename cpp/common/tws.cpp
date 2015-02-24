//
//  tws.cpp
//  TWS_SDK_CPP
//
//  Created by Jinkyou Son on 2/11/15.
//  Copyright (c) 2015 Jinkyou Son. All rights reserved.
//

#include "tws.h"
#include <ctime>
#include <sstream>

#include <Poco/URI.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/AcceptCertificateHandler.h>
//#include <Poco/Net/ConsoleCertificateHandler.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SecureStreamSocket.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/FilePartSource.h>
#include <Poco/StreamCopier.h>
#include <Poco/HMACEngine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/Exception.h>

#include <fstream>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/SSLManager.h>

#include "stringconverter.h"

#define TWS_EXCEPTION_BLOCK_START try
#define TWS_EXCEPTION_BLOCK_END catch (Poco::Exception e) { std::cout << e.displayText() << "\n"; } catch (std::exception e) { std::cout << e.what() << std::endl; }

#define TWS_TEST
#ifdef TWS_TEST
#define TWS_TEST_COUT(x) std::cout << x << std::endl;
#else
#define TWS_TEST_COUT(x)
#endif

namespace TWS_SDK
{
    const std::string URI_ILLEGAL_CHAR = "!?#/'\",;:$&()[]*+=@";
    
    std::string TWS::m_api_key = "";
    std::string TWS::m_api_secret = "";
    std::string TWS::m_stom_host = "https://stom.dddws.com";
    std::string TWS::m_stor_host = "https://stor.dddws.com";
    std::string TWS::m_stid_host = "https://stid.dddws.com";
    std::string TWS::m_cloud3d_host = "https://cloud3d.herokuapp.com";
    std::string TWS::m_api_version = "1";
    long TWS::m_expire_seconds = 3600;
    std::string TWS::m_auth_schema = "3WS";
    
    TWS::TWS(std::string key, std::string secret, std::string api_version /* = "" */, std::string stom_host /* = "" */, std::string stor_host /* = "" */, std::string stid_host /* = "" */)
    {
        TWS_EXCEPTION_BLOCK_START
        {
            m_api_key = key;
            m_api_secret = secret;
            if (!api_version.empty()) m_api_version = api_version;
            if (!stom_host.empty()) m_stom_host = stom_host;
            if (!stor_host.empty()) m_stor_host = stor_host;
            if (!stid_host.empty()) m_stid_host = stid_host;
            
            checkParams();
        }
        TWS_EXCEPTION_BLOCK_END
    }
    
    Poco::DynamicStruct TWS::authenticate()
    {
		TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            Poco::Net::initializeSSL();
            
            HTTP_METHOD method = HTTP_METHOD::POST;
            std::string expire_t = std::to_string(expire());
            std::string path = "/authenticate";
            std::string signature = getSignature(method, expire_t, path);
            
            Poco::URI uri(m_stid_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("key", m_api_key);
            uri.addQueryParameter("signature", signature);
            uri.addQueryParameter("method", convertHTTPMethodToString(method));
            uri.addQueryParameter("url", "/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method);
            getResponse(*session, result);
            
            Poco::Net::uninitializeSSL();
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::createModel(Poco::DynamicStruct meta /* = Poco::DynamicStruct() */, std::string upload_id /* = "" */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::POST;
            std::string expire_t = std::to_string(expire());
            std::string path = "/models";
            std::string signature = getSignature(method, expire_t, path);
            
            Poco::URI uri(m_stor_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::DynamicStruct requestBody;
            if (!meta.empty())
                requestBody["meta"] = meta;
            if (!upload_id.empty())
                requestBody["upload_id"] = upload_id;

            TWS_TEST_COUT(requestBody.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature, requestBody);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::getModel(std::string id /* = "" */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::GET;
            std::string expire_t = std::to_string(expire());
            std::string path = "/models/" + id;
            std::string signature = getSignature(method, expire_t, path);
            
            Poco::URI uri(m_stor_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);

            TWS_TEST_COUT(uri.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::Dynamic::Array TWS::getModels(std::unordered_map<std::string, std::string> query_params /* = std::unordered_map<std::string, std::string>() */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::Dynamic::Array result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::GET;
            std::string expire_t = std::to_string(expire());
            std::string path = "/models";
            std::string signature = getSignature(method, expire_t, path);
            
            // request
            Poco::URI uri(m_stor_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            for (auto it = query_params.begin(); it != query_params.end(); ++it)
            {
                uri.addQueryParameter(it->first, it->second);
            }
            
            TWS_TEST_COUT(uri.toString());

            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::updateModel(std::string id, Poco::DynamicStruct meta /* = Poco::DynamicStruct() */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::PUT;
            std::string expire_t = std::to_string(expire());
            std::string path = "/models/" + id;
            std::string signature = getSignature(method, expire_t, path);
            
            Poco::URI uri(m_stor_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::DynamicStruct requestBody;
            if (!meta.empty())
                requestBody["meta"] = meta;
            
            TWS_TEST_COUT(requestBody.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature, requestBody);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::deleteModel(std::string id)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::DELETE;
            std::string expire_t = std::to_string(expire());
            std::string path = "/models/" + id;
            std::string signature = getSignature(method, expire_t, path);
            
            // request
            Poco::URI uri(m_stor_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::presignedUploadForm(std::unordered_map<std::string, std::string> starts_with_hash /* = std::unordered_map<std::string, std::string>() */, std::string ip_str /* = "" */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::POST;
            std::string expire_t = std::to_string(expire());
            std::string path = "/models/presign";
            std::string signature = getSignature(method, expire_t, path);

            // request
            Poco::URI uri(m_stor_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::DynamicStruct jsonHash;
            for (auto it = starts_with_hash.begin(); it != starts_with_hash.end(); ++it)
            {
                jsonHash[it->first] = it->second;
            }
            
            Poco::DynamicStruct requestBody;
            if (!ip_str.empty())
                requestBody["ip"] = ip_str;
            if (!jsonHash.empty())
                requestBody["starts_with"] = jsonHash;
            
            TWS_TEST_COUT(requestBody.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature, requestBody);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::uploadModel(std::string path, Poco::DynamicStruct opts /* = Poco::DynamicStruct() */, std::string ip_str /* = "" */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            // Upload to pre-signed s3 url
            std::unordered_map<std::string, std::string> starts_with;
            try
            {
                auto var = opts["starts_with"];
                if (var.isStruct())
                {
                    auto starts_with_struct = var.extract<Poco::DynamicStruct>();
                    for (auto it = starts_with_struct.begin(); it != starts_with_struct.end(); ++it)
                    {
                        if (!it->second.isString())
                        {
                            throw;
                        }
                        starts_with[it->first] = it->second.convert<std::string>();
                    }
                }
            }
            catch (std::exception e)
            {
                starts_with.clear();
            }
            
            
            Poco::DynamicStruct presign = presignedUploadForm(starts_with, ip_str);
            Poco::URI upload_uri(presign["form_action"].convert<std::string>());
            Poco::DynamicStruct form_fields = presign["form_fields"].extract<Poco::DynamicStruct>();
            
            TWS_TEST_COUT(upload_uri.toString());

            Poco::SharedPtr<Poco::Net::HTTPClientSession> upload_session = getHTTPClientSession(upload_uri);
            Poco::Net::HTTPRequest upload_req(Poco::Net::HTTPRequest::HTTP_POST, upload_uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
            upload_session->setKeepAlive(true);

            Poco::Net::HTMLForm requestForm;
            requestForm.setEncoding(Poco::Net::HTMLForm::ENCODING_MULTIPART);
            for (auto it = form_fields.begin(); it != form_fields.end(); ++it)
            {
                requestForm.set(it->first, it->second);
            }
            requestForm.addPart("file", new Poco::Net::FilePartSource(path));
            requestForm.prepareSubmit(upload_req);
            upload_req.setContentLength(requestForm.calculateContentLength());
            requestForm.write(upload_session->sendRequest(upload_req));

            Poco::Net::HTTPResponse upload_res;
            upload_session->receiveResponse(upload_res);
            
            TWS_TEST_COUT(std::to_string(upload_res.getStatus()) + " " + upload_res.getReason())
            
            if (upload_res.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_NO_CONTENT)
            {
                throw new std::runtime_error("file " + presign["upload_id"].convert<std::string>() + "upload failed. Response Code: " + std::to_string(upload_res.getStatus()));
            }
            
            upload_session = nullptr;
            
            // Create model in STOR
            HTTP_METHOD method = HTTP_METHOD::POST;
            std::string expire_t = std::to_string(expire());
            std::string create_path = "/models";
            std::string signature = getSignature(method, expire_t, create_path);
            
            // request
            Poco::URI create_uri(m_stor_host);
            create_uri.setPath("/api/v" + m_api_version + create_path);
            create_uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(create_uri.toString());
            
            Poco::DynamicStruct meta;
            if (!opts.empty() && !opts["meta"].isEmpty())
            {
                Poco::Dynamic::Var var = opts["meta"];
                if (var.isStruct())
                {
                    meta = var.extract<Poco::DynamicStruct>();
                }
            }
            
            Poco::DynamicStruct create_requestBody;
            if (!meta.empty())
                create_requestBody["meta"] = meta;
            if (!presign["upload_id"].convert<std::string>().empty())
                create_requestBody["upload_id"] = presign["upload_id"];
            
            TWS_TEST_COUT(create_requestBody.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> create_session = getHTTPClientSession(create_uri);
            sendRequest(*create_session, create_uri, method, signature, create_requestBody);
            getResponse(*create_session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    std::string TWS::getLink(std::string id, std::string filename /* = "" */, long expire_seconds /* = 0 */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        std::string result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            std::string expire_t = std::to_string(expire());
            std::string endPoint;
            if (filename.empty())
            {
                endPoint = "download";
            }
            else
            {
                Poco::URI::encode(filename, URI_ILLEGAL_CHAR, endPoint);
            }
            
            std::string sig = signature("POST\n\n" + expire_t + "\n/api/v" + m_api_version + "/models/" + id + "/" + endPoint);
            result = m_stor_host + "/api/v" + m_api_version + "/models/" + id + "/"+ endPoint + "?expire=" + expire_t + "&key=" + m_api_key + "&signature=" + sig;
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::requetTPTX(std::unordered_map<std::string, std::string> opts /* = std::unordered_map<std::string, std::string>() */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::POST;
            std::string path = "/jobs";
            
            // request
            Poco::URI uri(m_cloud3d_host);
            uri.setPath("/api/v" + m_api_version + path);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::DynamicStruct requestBody;
            requestBody["auth_token"] = "HRJ7PsKqAVzwYbV5ENaK";
            if (!opts.empty())
            {
                requestBody["input_url"] = opts["input_url"];
                requestBody["input_type"] = opts["input_type"];
                requestBody["include_thumbnail"] = opts["include_thumbnail"];
            }
            
            TWS_TEST_COUT(requestBody.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, "", requestBody);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::poleTPTX(std::string job_id)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::GET;
            std::string path = "/jobs/" + job_id;

            // request
            Poco::URI uri(m_cloud3d_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("auth_token", "HRJ7PsKqAVzwYbV5ENaK");
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::createSession(std::string timeout /* = "60" */, std::string engine_ver /* = "latest" */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::POST;
            std::string expire_t = std::to_string(expire());
            std::string path = "/sessions";
            std::string signature = getSignature(method, expire_t, path);
            
            // request
            Poco::URI uri(m_stom_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::DynamicStruct requestBody;
            if (!timeout.empty())
                requestBody["timeout"] = timeout;
            if (!engine_ver.empty())
                requestBody["engine_version"] = engine_ver;
            
            TWS_TEST_COUT(requestBody.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature, requestBody);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }

    Poco::Dynamic::Array TWS::getSessions(std::unordered_map<std::string, std::string> query_params /* = std::unordered_map<std::string, std::string>()*/)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::Dynamic::Array result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            long expire_t = expire();
            std::string t = std::to_string(expire_t);
            std::string sig = signature("GET\n\n" + t + "\n/api/v" + m_api_version + "/sessions");
            std::string auth_schema = "3WS";
            std::string auth_info = m_api_key + ":" + sig;
            
            // request
            Poco::URI uri(m_stom_host);
            uri.setPath("/api/v" + m_api_version + "/sessions");
            uri.addQueryParameter("expire", t);
            for (auto it = query_params.begin(); it != query_params.end(); ++it)
            {
                uri.addQueryParameter(it->first, it->second);
            }
            
            TWS_TEST_COUT(uri.toString());
            TWS_TEST_COUT(auth_schema + " " + auth_info);
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_0);
            req.setCredentials(auth_schema, auth_info);
            session->sendRequest(req);
            
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::getSession(std::string id)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::GET;
            std::string expire_t = std::to_string(expire());
            std::string path = "/sessions/" + id;
            std::string signature = getSignature(method, expire_t, path);
            
            // request
            Poco::URI uri(m_stom_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    
    Poco::DynamicStruct TWS::closeSession(std::string id)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::DELETE;
            std::string expire_t = std::to_string(expire());
            std::string path = "/sessions/" + id;
            std::string signature = getSignature(method, expire_t, path);

            // request
            Poco::URI uri(m_stom_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::createRun(std::string id, std::string platform, std::string code)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::POST;
            std::string expire_t = std::to_string(expire());
            std::string path = "/sessions/" + id + "/runs";
            std::string signature = getSignature(method, expire_t, path);
            
            // request
            Poco::URI uri(m_stom_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::DynamicStruct requestBody;
            if (!platform.empty())
                requestBody["platform"] = platform;
            if (!code.empty())
                requestBody["code"] = code;
            
            TWS_TEST_COUT(requestBody.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature, requestBody);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::Dynamic::Array TWS::getRuns(std::string id, std::string platform /* = "" */)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::Dynamic::Array result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::GET;
            std::string expire_t = std::to_string(expire());
            std::string path = "/sessions/" + id + "/runs";
            std::string signature = getSignature(method, expire_t, path);

            // request
            Poco::URI uri(m_stom_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            uri.addQueryParameter("platform", platform);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    Poco::DynamicStruct TWS::getRun(std::string session_id, std::string run_id)
    {
        TWS_TEST_COUT(__FUNCTION__);
        
        Poco::DynamicStruct result;
        
        TWS_EXCEPTION_BLOCK_START
        {
            HTTP_METHOD method = HTTP_METHOD::GET;
            std::string expire_t = std::to_string(expire());
            std::string path = "/sessions/" + session_id + "/runs/" + run_id;
            std::string signature = getSignature(method, expire_t, path);
            
            // request
            Poco::URI uri(m_stom_host);
            uri.setPath("/api/v" + m_api_version + path);
            uri.addQueryParameter("expire", expire_t);
            
            TWS_TEST_COUT(uri.toString());
            
            Poco::SharedPtr<Poco::Net::HTTPClientSession> session = getHTTPClientSession(uri);
            sendRequest(*session, uri, method, signature);
            getResponse(*session, result);
        }
        TWS_EXCEPTION_BLOCK_END
        
        return result;
    }
    
    // private
    void TWS::checkParams()
    {
        if (m_api_key.empty()
            || m_api_secret.empty()
            || m_api_version.empty()
            || m_stom_host.empty()
            || m_stor_host.empty()
            || m_stid_host.empty())
        {
            throw new std::invalid_argument("Parameter cannot be null");
        }
    }
    
    std::string TWS::signature(std::string string_to_sign)
    {
        try
        {
            Poco::HMACEngine<Poco::SHA1Engine> hmac(m_api_secret);
            hmac.update(string_to_sign);
            std::vector<unsigned char> digest = hmac.digest();
            //std::string digest_str = urlEncode(base64Encode(&digest[0], digest.size()));
            std::string signature;
            Poco::URI::encode(base64Encode(&digest[0], digest.size()), "!?#/'\",;:$&()[]*+=@", signature);
            return signature;
        }
        catch (Poco::Exception e)
        {
            throw e;
        }
    }
    
    long TWS::expire(long expire_seconds /* = 0 */)
    {
        time_t now = time(NULL);
        if (expire_seconds == 0)
            expire_seconds = m_expire_seconds;
        return now + expire_seconds;
    }
    
    std::string TWS::getSignature(HTTP_METHOD method, std::string expire_t, std::string path)
    {
        std::string sig = signature(convertHTTPMethodToString(method) + "\n\n" + expire_t + "\n/api/v" + m_api_version + path);
        TWS_TEST_COUT("signature= " + sig);
        
        return sig;
    }
    
    std::string TWS::getAuthorizationInfo(std::string signature)
    {
        return m_api_key + ":" + signature;
    }
    
    Poco::Net::HTTPClientSession* TWS::getHTTPClientSession(Poco::URI& uri)
    {
        TWS_TEST_COUT("create session");
        
        if (uri.getPort() == 443)
        {
            Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrHandler = new Poco::Net::AcceptCertificateHandler(false);
            
            Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
            
            Poco::Net::SSLManager::instance().initializeClient(0, ptrHandler, ptrContext);
            
            return new Poco::Net::HTTPSClientSession(uri.getHost(), uri.getPort(), ptrContext);
        }
        else
        {
            return new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
        }
    }
    
    void TWS::sendRequest(Poco::Net::HTTPClientSession& session, Poco::URI uri, HTTP_METHOD method, std::string signature /* = "" */, Poco::DynamicStruct requestBody /* = Poco::DynamicStruct() */)
    {
        TWS_TEST_COUT("send request");
        
        Poco::Net::HTTPRequest req(convertHTTPMethodToPocoHTTPMethod(method), uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_0);
        
        TWS_TEST_COUT("auth= " + m_auth_schema + " " + getAuthorizationInfo(signature));
        if (!signature.empty())
        {
            req.setCredentials(m_auth_schema, getAuthorizationInfo(signature));
        }
        
        if (!requestBody.empty())
        {
            req.setContentType("application/json");
            req.setContentLength(requestBody.toString().length());
        }
        
        std::ostream& bodyWriter = session.sendRequest(req);
        
        if (!requestBody.empty())
        {
            TWS_TEST_COUT("send request body");
            bodyWriter << requestBody.toString();
        }
        
        TWS_TEST_COUT("sendRequest finished");
    }
    
    void TWS::getResponse(Poco::Net::HTTPClientSession& session, Poco::DynamicStruct& result)
    {
        TWS_TEST_COUT("receive response");
        
        Poco::Net::HTTPResponse res;
        std::istream& bodyReader = session.receiveResponse(res);
        
        TWS_TEST_COUT(std::to_string(res.getStatus()) + " " + res.getReason());
        
        TWS_TEST_COUT("convert response body");
        
        if (res.getStatus() >= 200 && res.getStatus() < 300)
        {
            if (res.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_NO_CONTENT && !bodyReader.eof())
            {
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var var = parser.parse(bodyReader);
                Poco::JSON::Object::Ptr object = var.extract<Poco::JSON::Object::Ptr>();
                result = *object;
            }
        }
        
        if (result.empty())
        {
            result["status"] = (int)res.getStatus();
            result["reason"] = res.getReason();
        }
        
        TWS_TEST_COUT(result.toString());
        TWS_TEST_COUT("getResponse finished");
    }
    
    void TWS::getResponse(Poco::Net::HTTPClientSession& session, Poco::Dynamic::Array& result)
    {
        TWS_TEST_COUT("receive response");
        
        Poco::Net::HTTPResponse res;
        std::istream& bodyReader = session.receiveResponse(res);

        TWS_TEST_COUT(std::to_string(res.getStatus()) + " " + res.getReason());
        
        TWS_TEST_COUT("convert response body");
        
        if (res.getStatus() >= 200 && res.getStatus() < 300)
        {
            if (res.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_NO_CONTENT && !bodyReader.eof())
            {
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var var = parser.parse(bodyReader);
                Poco::JSON::Array::Ptr object = var.extract<Poco::JSON::Array::Ptr>();
                result = *object;
            }
        }
        
        if (result.empty())
        {
            result.push_back("status: " + std::to_string((int)res.getStatus()));
            result.push_back("reason: " + res.getReason());
        }
        
        TWS_TEST_COUT("getResponse finished");
    }
    
    const std::string TWS::convertHTTPMethodToString(HTTP_METHOD method)
    {
        switch (method) {
            case HTTP_METHOD::GET:
                return "GET";
            case HTTP_METHOD::PUT:
                return "PUT";
            case HTTP_METHOD::POST:
                return "POST";
            case HTTP_METHOD::DELETE:
                return "DELETE";
            default:
                return "";
        }
    }
    
    const std::string TWS::convertHTTPMethodToPocoHTTPMethod(HTTP_METHOD method)
    {
        switch (method) {
            case HTTP_METHOD::GET:
                return Poco::Net::HTTPRequest::HTTP_GET;
            case HTTP_METHOD::PUT:
                return Poco::Net::HTTPRequest::HTTP_PUT;
            case HTTP_METHOD::POST:
                return Poco::Net::HTTPRequest::HTTP_POST;
            case HTTP_METHOD::DELETE:
                return Poco::Net::HTTPRequest::HTTP_DELETE;
            default:
                return "";
        }
    }
}
