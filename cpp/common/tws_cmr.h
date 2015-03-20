//
//  tws_cmr.h
//  TWS_SDK_CPP
//
//  Created by Jinkyou Son on 2/16/15.
//  Copyright (c) 2015 Jinkyou Son. All rights reserved.
//

#ifndef __TWS_SDK_CPP__tws_cmr__
#define __TWS_SDK_CPP__tws_cmr__

#include <tws.h>

namespace TWS_SDK {
    class TWS_CMR : public TWS
    {
    public:
        TWS_CMR(const std::string& key, const std::string& secret, const std::string& stom_host = "", const std::string& stor_host = "", const std::string& api_version = "")
        : TWS(key, secret, stom_host, stor_host, "", api_version)
        {
        
        }
        
        std::string createCMR(std::string stor_id, std::string engine_version = "latest", bool preserve_downloaded_file = false, bool mesh_heal = false, bool clustering = false, bool wait_to_complete = false, bool check_existing = false);
        std::string getCMR(std::string stor_id, long expire_seconds = 0);
        std::string convertToLocalCMR(std::string cmr_json_string, std::string base_directory);

        
    private:
        bool putURL(Poco::DynamicStruct& jsonObj, std::unordered_map<std::string, std::string>& mapLinks);
        bool convertURL2Local(Poco::DynamicStruct& jsonObj, std::unordered_map<std::string, std::string>& mapLinks);
        void traverseCMR(Poco::DynamicStruct& jsonObj, std::unordered_map<std::string, std::string>& cache, bool(TWS_CMR::*callback)(Poco::DynamicStruct&, std::unordered_map<std::string, std::string>&));
        
    private:
        static std::string CHUNK_EXTENSION;
    };
}

#endif /* defined(__TWS_SDK_CPP__tws_cmr__) */
