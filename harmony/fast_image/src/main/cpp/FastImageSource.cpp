//
// Created on 2024/6/18.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "FastImageSource.h"
#include "Props.h"
#include "Uri.h"


namespace rnoh {
    bool FastImageSource::isAllowed(char c, const std::string &allow) { return allow.find(c) != std::string::npos; }

    std::string FastImageSource::uriEncode(const std::string &s, const std::string &allow) {
        if (s.empty()) {
            return s;
        }
        std::ostringstream encoded;
        size_t oldLength = s.length();
        size_t current = 0;
    
        while (current < oldLength) {
            size_t nextToEncode = current;
            while (nextToEncode < oldLength && isAllowed(s[nextToEncode], allow)) {
                nextToEncode++;
            }
            if (nextToEncode == oldLength) {
                if (current == 0) {
                    return s;
                } else {
                    encoded << s.substr(current, oldLength - current);
                    return encoded.str();
                }
            }
            if (nextToEncode > current) {
                encoded << s.substr(current, nextToEncode - current);
            }
            current = nextToEncode;
            size_t nextAllowed = current + 1;
            while (nextAllowed < oldLength && !isAllowed(s[nextAllowed], allow)) {
                nextAllowed++;
            }
            std::string toEncode = s.substr(current, nextAllowed - current);
            for (char c : toEncode) {
                encoded << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                        << static_cast<int>(static_cast<unsigned char>(c));
            }
    
            current = nextAllowed;
        }
        return encoded.str();
    }

    bool FastImageSource::isValidBase64(const std::string &str) {
        const std::regex base64_regex(R"(^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?$)");
        return std::regex_match(str, base64_regex);
    }
    
    bool FastImageSource::startsWith(const std::string &str, const std::string &prefix) {
        if (str.length() < prefix.length()) {
            return false;
        }
        return str.substr(0, prefix.length()) == prefix;
    }

    std::string FastImageSource::getSource() {
        if (source.empty()) {
            return source;
        }
        std::string handleResult = source;
        Uri c_uri(source);
        if ((c_uri.scheme() == "http" || c_uri.scheme() == "https" || c_uri.scheme() == "ftp") && c_uri.host() != "localhost") {
            DLOG(INFO) << "[liwang_Uri] scheme=" << c_uri.scheme();
            DLOG(INFO) << "[liwang_Uri] 需要转义的网络资源地址 path=" << c_uri.path();
            std::string subPrefix = c_uri.scheme() + "://";
            handleResult = subPrefix + uriEncode(source.substr(subPrefix.length(), source.length()), allowChars);
            DLOG(INFO) << "[liwang_Uri] 转义结果 handleResult=" << handleResult;
            return handleResult;
        }
        //base64's scheme is [data], need add by self
        if (isValidBase64(source) && c_uri.scheme() == "") {
            handleResult = "data:image/jpeg;base64," + source;
            return handleResult;
        }
        return handleResult;
    }
}
