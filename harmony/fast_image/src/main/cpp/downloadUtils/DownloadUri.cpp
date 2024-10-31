//
// Created on 2024/9/18.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "DownloadUri.h"
#include "Props.h"
#include "downloadUtils/HttpTaskProcessor.h"
#include <regex>
#include "FastImageSource.h"

bool isValidBase64(const std::string &str) {
    const std::regex base64_regex(R"(^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?$)");
    return std::regex_match(str, base64_regex);
}

bool isUriRes(const rnoh::Uri &uri) {
    return (uri.scheme() == "http" || uri.scheme() == "https" || uri.scheme() == "ftp") && uri.host() != "localhost";
}

std::regex pattern("[^a-zA-Z0-9]");
std::string DownloadUri::getUri(std::string uri) {
    if (uri.empty()) {
        return uri;
    }
    std::string handleResult = uri;
    rnoh::Uri c_uri(handleResult);
    if (isUriRes(c_uri)) {
        handleResult = std::regex_replace(handleResult, pattern, "");
        return handleResult;
    }
    //base64's scheme is [data], need add by self
    if (isValidBase64(handleResult) && c_uri.scheme() == "") {
        handleResult = "data:image/jpeg;base64," + handleResult;
        return handleResult;
    }
    return handleResult;
}