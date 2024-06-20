//
// Created on 2024/6/18.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include <sstream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <iostream>
#include <regex>

namespace rnoh {
    class FastImageSource{
        public:
        FastImageSource(std::string &source_){
            source = source_;
        };
    
        std::string getSource();
        
        private:
        std::string source;
        std::string allowChars = "@#&=*+-_.,:!?()/~'%;$";
    
        bool isAllowed(char c, const std::string &allow);

        std::string uriEncode(const std::string &s, const std::string &allow);

        bool isValidBase64(const std::string &str);
        
        bool startsWith(const std::string &str, const std::string &prefix);
    };
}
