//
// Created on 2024/6/18.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "FastImageSource.h"
#include <regex>

namespace rnoh {

    bool FastImageSource::isValidBase64(const std::string &str) {
        const std::regex base64_regex(R"(^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?$)");
        return std::regex_match(str, base64_regex);
    }

    bool FastImageSource::isUriRes(const Uri &uri) {
        return (uri.scheme() == httpPrefix || uri.scheme() == httpsPrefix || uri.scheme() == ftpPrefix) && uri.host() != localhostPrefix;
    }

    std::string FastImageSource::getUri() {
        if (source.uri.empty()) {
            return source.uri;
        }
        std::string handleResult = source.uri;
        Uri c_uri(handleResult);
        if (isUriRes(c_uri)) {
            DLOG(INFO) << "[FastImage] uri path=" << c_uri.path();
            handleResult = c_uri.getEncodeUri();
            DLOG(INFO) << "[FastImage] encode result=" << handleResult;
            return handleResult;
        }
        //base64's scheme is [data], need add by self
        if (isValidBase64(handleResult) && c_uri.scheme() == "") {
            handleResult = "data:image/jpeg;base64," + handleResult;
            return handleResult;
        }
        return handleResult;
    }
}
