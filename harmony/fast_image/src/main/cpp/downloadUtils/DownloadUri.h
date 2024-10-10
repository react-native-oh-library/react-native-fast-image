//
// Created on 2024/9/18.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_DOWNLOADURI_H
#define HARMONY_DOWNLOADURI_H

#include <string>
class DownloadUri {
public:

    std::string getUri(std::string uri);
};

#endif //HARMONY_DOWNLOADURI_H
