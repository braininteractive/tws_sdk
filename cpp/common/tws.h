//
//  tws.h
//  TWS_SDK_CPP
//
//  Created by Jinkyou Son on 2/11/15.
//  Copyright (c) 2015 Jinkyou Son. All rights reserved.
//

#ifndef tws_sdk_cpp_tws_h
#define tws_sdk_cpp_tws_h

#include <string>

namespace TWS_SDK
{
    class TWS
    {
    public:
        TWS(std::string key, std::string secret, std::string api_version = "", std::string stom_host = "", std::string stor_host = "", std::string stid_host = "");
        
    private:
        //void checkParams();
        
    protected:
        static std::string m_api_key;
        static std::string m_api_secret;
    private:
        static std::string m_stom_host;
        static std::string m_stor_host;
        static std::string m_stid_host;
        static std::string m_api_version;
        static long m_expire_seconds;
    };
}

#endif
