//
//  stringconverter.h
//  TWS_SDK_CPP
//
//  Created by Jinkyou Son on 2/12/15.
//  Copyright (c) 2015 Jinkyou Son. All rights reserved.
//

#ifndef __TWS_SDK_CPP__stringconverter__
#define __TWS_SDK_CPP__stringconverter__

#include <string>

namespace TWS_SDK {
    std::string base64Encode(unsigned char const* , size_t len);
    std::string base64Eecode(std::string const& s);
    
    std::string urlEncode(const std::string &c);
    //std::string urlDecode(const std::string &encoded);
}

#endif /* defined(__TWS_SDK_CPP__stringconverter__) */
