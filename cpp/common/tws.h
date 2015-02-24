//
//  tws.h
//  TWS_SDK_CPP
//
//  Created by Jinkyou Son on 2/11/15.
//  Copyright (c) 2015 Jinkyou Son. All rights reserved.
//

#ifndef _TWS_SDK_CPP__tws__
#define _TWS_SDK_CPP__tws__

#define POCO_STATIC

#include <string>
#include <unordered_map>
#include <Poco/DynamicStruct.h>
#include <Poco/Array.h>
#include <Poco/URI.h>
#include <Poco/JSON/Object.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPResponse.h>

#undef DELETE

namespace TWS_SDK
{
    enum class HTTP_METHOD
    {
        GET, PUT, POST, DELETE
    };
    
    class TWS
    {
    public:
        TWS(std::string key, std::string secret, std::string api_version = "", std::string stom_host = "", std::string stor_host = "", std::string stid_host = "");
    
        Poco::DynamicStruct authenticate();
        Poco::DynamicStruct createModel(Poco::DynamicStruct meta = Poco::DynamicStruct(), std::string upload_id = "");
        Poco::DynamicStruct getModel(std::string id = "");
        Poco::Dynamic::Array getModels(std::unordered_map<std::string, std::string> query_params = std::unordered_map<std::string, std::string>());
        Poco::DynamicStruct updateModel(std::string id, Poco::DynamicStruct meta = Poco::DynamicStruct());
        Poco::DynamicStruct deleteModel(std::string id);
        Poco::DynamicStruct presignedUploadForm(std::unordered_map<std::string, std::string> starts_with_hash = std::unordered_map<std::string, std::string>(), std::string ip_str = "");
        Poco::DynamicStruct uploadModel(std::string path, Poco::DynamicStruct opts = Poco::DynamicStruct(), std::string ip_str = "");
        std::string getLink(std::string id, std::string filename = "", long expire_seconds = 0);
        
        Poco::DynamicStruct requetTPTX(std::unordered_map<std::string, std::string> opts = std::unordered_map<std::string, std::string>());
        Poco::DynamicStruct poleTPTX(std::string job_id);
        
        Poco::DynamicStruct createSession(std::string timeout = "60", std::string engine_ver = "latest");
        Poco::Dynamic::Array getSessions(std::unordered_map<std::string, std::string> query_params = std::unordered_map<std::string, std::string>());
        Poco::DynamicStruct getSession(std::string id);
        Poco::DynamicStruct closeSession(std::string id);
        Poco::DynamicStruct createRun(std::string id, std::string platform, std::string code);
        Poco::Dynamic::Array getRuns(std::string id, std::string platform = "");
        Poco::DynamicStruct getRun(std::string session_id, std::string run_id);
        

    private:
        void checkParams();
        std::string signature(std::string string_to_sign);
        long expire(long expire_seconds = 0);
        
        std::string getSignature(HTTP_METHOD method, std::string expire_t, std::string path);
        std::string getAuthorizationInfo(std::string signature);
        Poco::Net::HTTPClientSession* getHTTPClientSession(Poco::URI& uri);
        void sendRequest(Poco::Net::HTTPClientSession& session, Poco::URI uri, HTTP_METHOD method, std::string signature = "", Poco::DynamicStruct requestBody = Poco::DynamicStruct());
        void getResponse(Poco::Net::HTTPClientSession& session, Poco::DynamicStruct& result);
        void getResponse(Poco::Net::HTTPClientSession& session, Poco::Dynamic::Array& result);
        
        const std::string convertHTTPMethodToString(HTTP_METHOD method);
        const std::string convertHTTPMethodToPocoHTTPMethod(HTTP_METHOD method);

    protected:
        static std::string m_api_key;
        static std::string m_api_secret;
    private:
        static std::string m_stom_host;
        static std::string m_stor_host;
        static std::string m_stid_host;
        static std::string m_cloud3d_host;
        static std::string m_api_version;
        static long m_expire_seconds;
        static std::string m_auth_schema;
        
    };
}

#endif /* defined(__TWS_SDK_CPP__tws__) */
