//
// Created on 2024/8/31.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef NETDOWNLOADDEMO_RESPONSEPROCCESSOR_H
#define NETDOWNLOADDEMO_RESPONSEPROCCESSOR_H

#include <cstdio>
#include <filemanagement/file_uri/oh_file_uri.h>
#include <map>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unordered_set>
#include <filesystem>
#include "RemoteCommunicationKit/rcp.h"
#include "RNCFastImageViewTurboModule.h"

class HttpTaskProcessor
{
public:
    
    HttpTaskProcessor()
    {
        taskList_.push_back(this);
    }
    
    ~HttpTaskProcessor()
    {
        if (request_ != nullptr) {
            // 释放请求资源
            HMS_Rcp_DestroyRequest(request_);
            request_ = nullptr;   
        }
    }
    
    
    static void clearTaskList()
    {
        taskList_.clear();
    }
    
    void createParentPath(const fs::path& filePath) {
        fs::path parentPath = filePath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            fs::create_directories(parentPath);
        }
    }
    
    size_t saveImage(const char * buffer, uint32_t length)
    {
        size_t status = 0;
        if (filePath_ == "")
            return status;
        createParentPath(filePath_);
        fp_ = fopen(filePath_.c_str(),"w");
        if (fp_)
        {
            status = fwrite(buffer,sizeof(char), length, fp_);
            fclose(fp_);    
        }
        else
            DLOG(ERROR) << "[FastImage] write to file Failed!!" << filePath_;
        
        return status;
    }
    
    void markStart()
    {
        finished_ = false;
        auto now = std::chrono::high_resolution_clock::now();
        startTime_ = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch();
    }
    
    void markEnd()
    {
        // 记录时间戳，标记任务结束使得keep() 返回false
        auto now = std::chrono::high_resolution_clock::now();
        endTime_ = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch();
        finished_ = true;
    }
    
    bool keep()
    {
        return !finished_; 
    }
    
    void echoSpendTime()
    {
        DLOG(INFO) << "[FastImage] HttpTaskProcessor Spend Time(ns): " << endTime_.count() - startTime_.count();
    }
    
    void launchHttpRequest(const char * imgUrl);
    void launchHttpRequest(const char * imgUrl,const std::map<std::string, std::string>& headers);
    
    void pushThreadID(std::string id);
    
    void* getImgDescOfPixelMap()
    {
        return imgDesc;
    }
    
    static void waitAllTask();
    static void destroyAllTask();
    static void echoThreadIds();
    
    std::shared_ptr<rnoh::FastImageSourceResolver> instance;
    
    std::string imgUrl;
    std::string filePath_;
private:
    static std::vector<HttpTaskProcessor*>  taskList_;
    static std::unordered_set<std::string>  threadIds;
    
    void* imgDesc;  // 解码后 pixelMap的描述对象地址 
    void* handleFFRT;  // 异步任务handle 用于释放资源
    void* queueHandleFFRT; // 异步任务队列handle 用于释放资源
    
    FILE* fp_ = nullptr;
    
    std::chrono::nanoseconds startTime_;
    std::chrono::nanoseconds  endTime_;
    
    bool finished_ = false;
    
    Rcp_Request *request_ = nullptr;
    Rcp_Session *session_ = nullptr;
    
    Rcp_ResponseCallbackObject responseCallback_;
};


#endif //NETDOWNLOADDEMO_RESPONSEPROCCESSOR_H
