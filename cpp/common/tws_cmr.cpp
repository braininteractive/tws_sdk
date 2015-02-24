//
//  tws_cmr.cpp
//  TWS_SDK_CPP
//
//  Created by Jinkyou Son on 2/16/15.
//  Copyright (c) 2015 Jinkyou Son. All rights reserved.
//

#include "tws_cmr.h"

#include <fstream>
#include <thread>
#include <chrono>

#include <Poco/URIStreamOpener.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>

namespace TWS_SDK {
    std::string TWS_CMR::CHUNK_EXTENSION = ".tws";
    
    Poco::DynamicStruct TWS_CMR::createCMR(std::string stor_id, std::string engine_version /* = "latest" */, bool preserve_downloaded_file /* = false */, bool mesh_heal /* = false */, bool clustering /* = false */, bool wait_to_complete /* = false */, bool check_existing /* = false */)
    {
        Poco::DynamicStruct m = getModel(stor_id);
        if (m.empty())
        {
            throw std::invalid_argument("Invalid stor_id: " + stor_id);
        }
        
        Poco::DynamicStruct meta = m["meta"].extract<Poco::DynamicStruct>();
        
        if (check_existing)
        {
            if (!meta.empty() && !meta["cmr_status"].isEmpty())
            {
                std::string cmr_status = meta["cmr_status"].extract<std::string>();
                if (cmr_status == "processing" || cmr_status == "complete" || cmr_status == "error")
                {
                    Poco::DynamicStruct status;
                    status["cmr_status"] = cmr_status;
                    return status;
                }
            }
        }
        
        Poco::DynamicStruct session = createSession("3600", engine_version);
        if (session.empty())
        {
            return Poco::DynamicStruct();
        }
        
        try
        {
            std::string session_id = session["id"].extract<std::string>();
            
            if (!meta.empty() && wait_to_complete)
            {
                meta["cmr_status"] = "processing";
                updateModel(stor_id, meta);
            }
            
            std::string code =
"from cmr import Cmr\n\
c = Cmr(api_key='" + m_api_key + "', api_secret='" + m_api_secret + "', keep_downloaded_files=" + (preserve_downloaded_file ? "True" : "False") + ", mesh_heal=" + (mesh_heal ? "True" : "False") + ", max_regions=" + (clustering ? "200" : "1") + ", max_triangles=0)\n\
conv_result = c.convert('" + stor_id + "')\n\
print(json.dumps(conv_result))";
            
            Poco::DynamicStruct run = createRun(session_id, "blender", code);
            
            if (wait_to_complete)
            {
                std::string run_id = std::to_string(run["id"].extract<int>());
                
                int iRet = 0;
                while (run["state"] != "complete" && run["state"] != "error" && iRet < 3600)
                {
                    std::chrono::milliseconds dura(1000);
                    std::this_thread::sleep_for(dura);
                    
                    run = getRun(session_id, run_id);
                    iRet++;
                    std::cout << "runrunrun" << std::endl;
                }
                
                if (!meta.empty())
                {
                    m = getModel(stor_id);
                    meta = m["meta"].extract<Poco::DynamicStruct>();
                    
                    if (!meta.empty() && !meta["cmr_status"].isEmpty())
                    {
                        std::string cmr_status = meta["cmr_status"].extract<std::string>();
                        
                        try
                        {
                            Poco::JSON::Parser parser;
                            Poco::Dynamic::Var var = parser.parse(run["result"].toString());
                            Poco::JSON::Object::Ptr object = var.extract<Poco::JSON::Object::Ptr>();
                            
                            Poco::DynamicStruct result = *object;
                            
                            if (!result["CMR"].isEmpty() && !result["CMR"][stor_id].isEmpty())
                            {
                                meta["cmr_status"] = "complete";
                            }
                            else
                            {
                                meta["cmr_status"] = "error";
                            }
                        }
                        catch (Poco::Exception e)
                        {
                            meta["cmr_status"] = "error";
                        }
                        
                        updateModel(stor_id, meta);
                    }
                }
                closeSession(session_id);
                return run;
            }
            return run;
        }
        catch (Poco::Exception e)
        {
            std::cout << e.displayText() << std::endl;
            throw e;
        }
        catch (std::exception e)
        {
            std::cout << e.what() << std::endl;
            throw e;
        }
    }
    
    std::string TWS_CMR::getCMR(std::string stor_id, long expire_seconds /* = 0 */)
    {
        Poco::DynamicStruct m = getModel(stor_id);
        std::cout << m.toString() << std::endl;
        Poco::DynamicStruct cmr;
        
        try
        {
            Poco::DynamicStruct meta = m["meta"].extract<Poco::DynamicStruct>();
            
            if (!meta["stov_cmr"].isEmpty())
            {
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var var = parser.parse(meta["stov_cmr"].toString());
                Poco::JSON::Object::Ptr object = var.extract<Poco::JSON::Object::Ptr>();
                cmr = *object;
            }
        }
        catch (Poco::Exception e)
        {
            std::cout << e.displayText() << std::endl;
            return "";
        }
        
        if (cmr.empty())
        {
            return "";
        }
        
        std::string result;
        
        try
        {
            std::unordered_map<std::string, std::string> mapLinks;
            if (expire_seconds != 0)
            {
                mapLinks["expire_seconds"] = std::to_string(expire_seconds);
                traverseCMR(cmr, mapLinks, &TWS_CMR::putURL);
                result = cmr.toString();
            }
        }
        catch (Poco::Exception e)
        {
            std::cout << e.displayText() << std::endl;
            return "";
        }
        catch (std::exception e)
        {
            std::cout << e.what() << std::endl;
            return "";
        }
        
        return result;
    }
    
    std::string TWS_CMR::convertToLocalCMR(std::string cmr_json_string, std::string base_directory)
    {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var var = parser.parse(cmr_json_string);
        Poco::DynamicStruct cmr = var.extract<Poco::DynamicStruct>();
        
        std::unordered_map<std::string, std::string> mapLinks;
        mapLinks["base_directory"] = base_directory;
        traverseCMR(cmr, mapLinks, &TWS_CMR::convertURL2Local);
        
        return cmr.toString();
    }
    
    bool TWS_CMR::putURL(Poco::DynamicStruct& jsonObj, std::unordered_map<std::string, std::string>& mapLinks)
    {
        if (jsonObj["class_type"].isEmpty() || jsonObj["class_type"] != "StovChunk")
        {
            return false;
        }
        
        if (jsonObj["id"].isEmpty() || jsonObj["stor_id"].isEmpty())
        {
            return false;
        }
        
        std::string stov_id = jsonObj["id"].extract<std::string>();
        std::string stor_id = jsonObj["stor_id"].extract<std::string>();
        
        long expire_seconds = 0;
        
        if (mapLinks.find("expire_seconds") != mapLinks.end())
        {
            expire_seconds = std::stoi(mapLinks["expire_seconds"]);
        }
        
        if (mapLinks.find(stor_id) != mapLinks.end())
        {
            std::string filename;
            
            if (!jsonObj["filename"].isEmpty())
            {
                filename = jsonObj["filename"].extract<std::string>();
            }
            else
            {
                filename = stov_id + CHUNK_EXTENSION;
            }
            
            std::string link = getLink(stor_id, filename, expire_seconds);
            
            if (!link.empty())
            {
                mapLinks[stor_id] = link;
                jsonObj["url"] = link;
            }
        }
        
        return true;
    }
    
    bool TWS_CMR::convertURL2Local(Poco::DynamicStruct& jsonObj, std::unordered_map<std::string, std::string>& mapLinks)
    {
        if (jsonObj["class_type"].isEmpty() || jsonObj["class_type"] != "StovChunk")
        {
            return false;
        }
        
        if (mapLinks.find("base_directory") == mapLinks.end())
        {
            return false;
        }
        
        if (jsonObj["id"].isEmpty() || jsonObj["stor_id"].isEmpty())
        {
            return false;
        }
        
        std::string base_dir = mapLinks["base_directory"];
        
        std::string stov_id = jsonObj["id"].extract<std::string>();
        std::string stor_id = jsonObj["stor_id"].extract<std::string>();
        
        if (mapLinks.find(stor_id) != mapLinks.end())
        {
            if (!jsonObj["url"].isEmpty())
            {
                std::string filename, link;
                
                if (!jsonObj["filename"].isEmpty())
                {
                    filename = base_dir + "/" + jsonObj["filename"].extract<std::string>();
                }
                else
                {
                    filename = base_dir + "/" + stov_id + CHUNK_EXTENSION;
                }
                
                try
                {
                    Poco::URI uri(jsonObj["url"].extract<std::string>());
                    std::ofstream fileWriter(filename);
                    Poco::SharedPtr<std::istream> fileReader = Poco::URIStreamOpener::defaultOpener().open(uri);
                    Poco::StreamCopier::copyStream(*fileReader, fileWriter);
                    
                    link = filename;
                }
                catch (Poco::Exception e)
                {
                    std::cout << e.displayText() << std::endl;
                }
                catch (std::exception e)
                {
                    std::cout << e.what() << std::endl;
                }
                
                if (!link.empty())
                {
                    mapLinks[stor_id] = link;
                    jsonObj["filename"] = link;
                }
            }
        }
        
        return true;
    }
    
    void TWS_CMR::traverseCMR(Poco::DynamicStruct& jsonObj, std::unordered_map<std::string, std::string>& cache, bool(TWS_CMR::*callback)(Poco::DynamicStruct&, std::unordered_map<std::string, std::string>&))
    {
        if (jsonObj.empty())
            return;
        
        TWS_CMR tws_cmr("", "", "");
        (this->*callback)(jsonObj, cache);
        
        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it)
        {
            if (it->second.isArray())
            {
                Poco::Dynamic::Array arr = it->second.extract<Poco::Dynamic::Array>();
                for (auto itArr = arr.begin(); itArr != arr.end(); ++itArr)
                {
                    if (itArr->isStruct())
                    {
                        Poco::DynamicStruct child = itArr->extract<Poco::DynamicStruct>();
                        traverseCMR(child, cache, callback);
                    }
                }
            }
            else if (it->second.isStruct())
            {
                Poco::DynamicStruct child = it->second.extract<Poco::DynamicStruct>();
                traverseCMR(child, cache, callback);
            }
        }
    }
}