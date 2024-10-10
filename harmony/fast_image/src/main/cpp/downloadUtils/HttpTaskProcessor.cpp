//
// Created on 2024/9/2.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "HttpTaskProcessor.h"
#include <unistd.h>
#include <sstream>
#include <glog/logging.h>

std::vector<HttpTaskProcessor*>  HttpTaskProcessor::taskList_;
std::unordered_set<std::string>  HttpTaskProcessor::threadIds;

void ResponseCallback(void *usrCtx, Rcp_Response *response, uint32_t errCode)
{
    HttpTaskProcessor* processor = static_cast<HttpTaskProcessor*>(usrCtx);
    
    // 记录当前线程的ID
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    processor->pushThreadID(oss.str());
    
    if (response != NULL) {
        if (response->statusCode == RCP_OK){
            size_t status = processor->saveImage(response->body.buffer, response->body.length);
            if(processor->instance && status != 0){
                processor->instance->imageDownloadComplete(processor->imgUrl,processor->filePath_);
            } else if (processor->instance) {
                processor->instance->imageDownloadFail(processor->imgUrl);
            }
        }
            
        response->destroyResponse(response);
    } else {
        DLOG(ERROR) << "[FastImage] !!! Callback Response Is NULL, errCode: "<< errCode;
        if(processor->instance){
            processor->instance->imageDownloadFail(processor->imgUrl);
        }
    }
    
    processor->markEnd();
}

void HttpTaskProcessor::launchHttpRequest(const char * imgUrl)
{
    this->imgUrl = imgUrl;
    markStart();
    // 通过url创建http request 对象
    request_ = HMS_Rcp_CreateRequest(imgUrl);
    
    // 设置请求方式为GET
    request_->method = RCP_METHOD_GET;
    uint32_t errCode = 0;
    
    // 创建http 会话对象
    session_ = HMS_Rcp_CreateSession(NULL, &errCode);
    
    if (errCode)
    {
        DLOG(ERROR) << "[FastImage] launchHttpRequest create Session error: "<< errCode;
        if(instance){
           instance->imageDownloadFail(imgUrl); 
        }
        return;
    }
    
    // 设置响应数据处理的回调函数
    responseCallback_.callback = ResponseCallback;
    responseCallback_.usrCtx = this;
    
    // API异步发起请求
    errCode = HMS_Rcp_Fetch(session_, request_, &responseCallback_);
    if (errCode)
    {
        DLOG(ERROR) << "[FastImage] launchHttpRequest HMS_Rcp_Fetch error: "<< errCode;
        if(instance){
           instance->imageDownloadFail(imgUrl); 
        }
        markEnd();
    }
}


void HttpTaskProcessor::launchHttpRequest(const char * imgUrl,const std::map<std::string, std::string>& headers){
    this->imgUrl = imgUrl;
    markStart();
    // 通过url创建http request 对象
    request_ = HMS_Rcp_CreateRequest(imgUrl);
    Rcp_Headers* header = HMS_Rcp_CreateHeaders();
    //初始化headers数据
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        uint32_t error = HMS_Rcp_SetHeaderValue(header, it->first.c_str(), it->second.c_str());
        if(error){
            if(instance){
               instance->imageDownloadFail(imgUrl); 
            }
            DLOG(ERROR) << "[FastImage] launchHttpRequest create Session error: "<< error;
            return;
        }
    }
    
    // 设置请求方式为GET
    request_->method = RCP_METHOD_GET;
    request_->headers = header;
    uint32_t errCode = 0;
    
    // 创建http 会话对象
    session_ = HMS_Rcp_CreateSession(NULL, &errCode);
    if (errCode){
        DLOG(ERROR) << "[FastImage] launchHttpRequest create Session error: "<< errCode;
        return;
    }
    
    // 设置响应数据处理的回调函数
    responseCallback_.callback = ResponseCallback;
    responseCallback_.usrCtx = this;
    
    // API异步发起请求
    errCode = HMS_Rcp_Fetch(session_, request_, &responseCallback_);
    if (errCode)
    {
        DLOG(ERROR) << "[FastImage] launchHttpRequest HMS_Rcp_Fetch error: "<< errCode;
        markEnd();
    }
}

// static 
void HttpTaskProcessor::waitAllTask()
{
    bool wait = true;
    while(wait) {
        wait = false;
        for (auto* processor : taskList_) {
            if (processor->keep())
                wait = true;
        }
        if (wait) {
            usleep(1000*1000);
//             echoThreadIds();
        }
    }
}

// static
void HttpTaskProcessor::destroyAllTask()
{
    for (auto* processor : taskList_) {
        processor->echoSpendTime();
        delete processor;
    }
    
    taskList_.clear();
    threadIds.clear();
}

// static
void HttpTaskProcessor::echoThreadIds()
{
    for (auto id : threadIds) {
        DLOG(INFO) << "[FastImage] NetDownload Task Thread ID: "<< id.c_str();
    }
    
    DLOG(INFO) << "[FastImage] NetDownload Number Of Thread : "<< threadIds.size();
}


void HttpTaskProcessor::pushThreadID(std::string id)
{
    threadIds.insert(id);
}