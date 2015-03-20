//
//  tws.h
//  TWS_SDK_CPP
//
//  Created by Jinkyou Son on 2/11/15.
//  Copyright (c) 2015 Jinkyou Son. All rights reserved.
//

#ifndef _TWS_SDK_CPP__tws__
#define _TWS_SDK_CPP__tws__

#include <string>
#include <unordered_map>
#include <Poco/DynamicStruct.h>
#include <Poco/Array.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPResponse.h>

namespace TWS_SDK
{
    enum class TWS_HTTP_METHOD
    {
        TWS_HTTP_METHOD_GET, TWS_HTTP_METHOD_PUT, TWS_HTTP_METHOD_POST, TWS_HTTP_METHOD_DELETE
    };
    
    class TWS
    {
    public:
        TWS(std::string key, std::string secret, std::string stom_host = "", std::string stor_host = "", std::string stid_host = "", std::string api_version = "");

        std::string authenticate();
        std::string createModel(const std::string& meta = "", const std::string& upload_id = "");
        std::string getModel(const std::string& id = "");
        std::string getModels(const std::unordered_map<std::string, std::string>& query_params = std::unordered_map<std::string, std::string>());
        std::string updateModel(const std::string& id, const std::string& meta = "");
        std::string deleteModel(const std::string& id);
        std::string presignedUploadForm(const std::unordered_map<std::string, std::string>& starts_with_hash = std::unordered_map<std::string, std::string>(), const std::string& ip_str = "");
        std::string uploadModel(const std::string& path, const std::string& opts = "", const std::string& ip_str = "");
        std::string getLink(const std::string& id, const std::string& filename = "", long long expire_seconds = 0);
        
        std::string requetTPTX(const std::unordered_map<std::string, std::string>& opts = std::unordered_map<std::string, std::string>());
        std::string poleTPTX(const std::string& job_id);
        
        std::string createSession(const std::string& timeout = "60", const std::string& engine_ver = "latest");
        std::string getSessions(const std::unordered_map<std::string, std::string>& query_params = std::unordered_map<std::string, std::string>());
        std::string getSession(const std::string& id);
        std::string closeSession(const std::string& id);
        std::string createRun(const std::string& id, const std::string& platform, const std::string& code);
        std::string getRuns(const std::string& id, const std::string& platform = "");
        std::string getRun(const std::string& session_id, const std::string& run_id);
        

    private:
        void checkParams();
        std::string signature(std::string string_to_sign);
        long long expire(long long expire_seconds = 0);
        
        std::string getSignature(TWS_HTTP_METHOD method, std::string expire_t, std::string path);
        std::string getAuthorizationInfo(std::string signature);
        Poco::Net::HTTPClientSession* getHTTPClientSession(Poco::URI& uri);
        void sendRequest(Poco::Net::HTTPClientSession& session, Poco::URI uri, TWS_HTTP_METHOD method, std::string signature = "", Poco::DynamicStruct requestBody = Poco::DynamicStruct());
        void getResponse(Poco::Net::HTTPClientSession& session, Poco::DynamicStruct& result);
        void getResponse(Poco::Net::HTTPClientSession& session, Poco::Dynamic::Array& result);
        
    protected:
        static const std::string convertHTTPMethodToString(TWS_HTTP_METHOD method);
        static const std::string convertHTTPMethodToPocoHTTPMethod(TWS_HTTP_METHOD method);
        static Poco::DynamicStruct stringToJSON(const std::string& str);
        static const std::string pocoArrayToString(const Poco::Dynamic::Array array);

    protected:
        std::string m_api_key;
        std::string m_api_secret;
    private:
        std::string m_stom_host;
        std::string m_stor_host;
        std::string m_stid_host;
        std::string m_cloud3d_host;
        std::string m_api_version;
        long long m_expire_seconds;
        std::string m_auth_schema;
    };
}

#endif /* defined(__TWS_SDK_CPP__tws__) */
