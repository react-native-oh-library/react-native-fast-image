
#include "FastImageSourceResolver.h"
#include <fstream>
#include <multimedia/image_framework/image/image_source_native.h>

namespace rnoh {

std::string FastImageSourceResolver::resolveImageSources(ImageSourceUpdateListener &listener, std::string uri) {
    //先找是不是还在下载
    if(pendingSet.find(uri) != pendingSet.end()){
        removeListener(&listener);
        addListenerForURI(uri, &listener);
        return "";
    }
    DownloadUri downloadUri;
    std::string fileUri = downloadUri.getUri(uri);
    bool exist = remoteImageSourceMap.get(fileUri);
    if(exist){
        uri = "file://"+ fileCacheDir + fileUri;
    }
    
    return uri;
}

void FastImageSourceResolver::addListenerForURI(const std::string &uri, ImageSourceUpdateListener *listener) {
    listener->observedUri = uri;
    auto it = uriListenersMap.find(uri);
    if (it == uriListenersMap.end()) {
        uriListenersMap.emplace(uri, std::initializer_list<ImageSourceUpdateListener *>{listener});
        return;
    }
    if (std::find(it->second.begin(), it->second.end(), listener) != it->second.end()) {
        return;
    }
    it->second.push_back(listener);
}

void FastImageSourceResolver::removeListenerForURI(const std::string &uri, ImageSourceUpdateListener *listener) {
    auto it = uriListenersMap.find(uri);
    if (it == uriListenersMap.end()) {
        return;
    }
    auto &listeners = it->second;
    auto listenerPos = std::find(listeners.begin(), listeners.end(), listener);
    if (listenerPos != listeners.end()) {
        listeners.erase(listenerPos);
        if (listeners.empty()) {
            uriListenersMap.erase(uri);
        }
    }
}

void FastImageSourceResolver::removeListener(ImageSourceUpdateListener *listener) {
    if(!listener->observedUri.empty()){
        removeListenerForURI(listener->observedUri, listener);
    }
}

void FastImageSourceResolver::imageDownloadComplete(std::string uri,std::string fileUri){
    auto pend = pendingSet.find(uri);
    if(pend != pendingSet.end()){
        pendingSet.erase(uri);
    }
    
    remoteImageSourceMap.put(fileUri, true);
    auto it = uriListenersMap.find(uri);
    if (it == uriListenersMap.end()) {
        return;
    }
    
    auto &listeners = it->second;
    for (auto listener : listeners) {
        listener->onImageSourceCacheUpdate(fileUri);
        removeListenerForURI(uri, listener);
    }
}
void FastImageSourceResolver::imageDownloadFail(std::string uri){
    auto pend = pendingSet.find(uri);
    if(pend != pendingSet.end()){
        pendingSet.erase(uri);
    }

    auto it = uriListenersMap.find(uri);
    if (it == uriListenersMap.end()) {
        return;
    }
    auto &listeners = it->second;
    for (auto listener : listeners) {
        listener->onImageSourceCacheDownloadFileFail();
        removeListenerForURI(uri, listener);
    }
}
}