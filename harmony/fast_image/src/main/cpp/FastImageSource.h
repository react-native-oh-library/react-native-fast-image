/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <react/renderer/components/react_native_fast_image/Props.h>
#include "Uri.h"
#include <string>

namespace rnoh {
    class FastImageSource{
        public:
        FastImageSource(facebook::react::FastImageViewSourceStruct source_){
            source = source_;
        };
    
        std::string getUri();
        
        private:
        facebook::react::FastImageViewSourceStruct source;
        const std::string httpPrefix = "http";
        const std::string httpsPrefix = "https";
        const std::string ftpPrefix = "ftp";
        const std::string localhostPrefix = "localhost";

        bool isValidBase64(const std::string &str);
        
        bool isUriRes(const Uri &uri);
    
    };
}
