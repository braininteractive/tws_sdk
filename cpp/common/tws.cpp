//
//  tws.cpp
//  TWS_SDK_CPP
//
//  Created by Jinkyou Son on 2/11/15.
//  Copyright (c) 2015 Jinkyou Son. All rights reserved.
//

#include "tws.h"

namespace TWS_SDK
{
    TWS::TWS(std::string key, std::string secret, std::string api_version /*= ""*/, std::string stom_host /*= ""*/, std::string stor_host /*= ""*/, std::string stid_host /*= ""*/)
    {
        try
        {
            m_api_key = key;
            m_api_secret = secret;
            if (!api_version.empty()) m_api_version = api_version;
            if (!stom_host.empty()) m_stom_host = stom_host;
            if (!stor_host.empty()) m_stor_host = stor_host;
            if (!stid_host.empty()) m_stid_host = stid_host;
            
            //checkParams();
        }
        catch (std::exception e)
        {
            throw e;
        }
    }
    
    /*
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
    }*/
}
