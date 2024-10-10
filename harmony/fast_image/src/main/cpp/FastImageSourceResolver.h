
#ifndef FASTIMAGESOURCERESOLVER_H
#define FASTIMAGESOURCERESOLVER_H

#include "RNOH/RNInstance.h"
#include <memory>
#include <unordered_set>
#include <filesystem>
#include "downloadUtils/DownloadUri.h"
#include "downloadUtils/LRUCache.h"

namespace fs = std::filesystem;

namespace rnoh {

class FastImageSourceResolver {
    public:
        using Shared = std::shared_ptr<FastImageSourceResolver>;
        
        FastImageSourceResolver(std::string fileCacheDir)
            : uriListenersMap(),fileCacheDir(fileCacheDir)
            ,remoteImageSourceMap(128,[this](std::string fileUri,bool a) {
                fs::path filePath = this->fileCacheDir + fileUri;
                if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
                    return fs::remove(filePath);
                }
                return false;
            }){}

        class ImageSourceUpdateListener {
        public:
            std::string observedUri;

            ImageSourceUpdateListener(FastImageSourceResolver::Shared const &FastImageSourceResolver)
                : m_FastImageSourceResolver(FastImageSourceResolver){};

            ~ImageSourceUpdateListener() {
                if(!m_FastImageSourceResolver){
                    m_FastImageSourceResolver->removeListener(this);
                }            
            }

            virtual void onImageSourceCacheUpdate(std::string image) = 0;
            virtual void onImageSourceCacheDownloadFileFail() = 0;

        private:
            FastImageSourceResolver::Shared const &m_FastImageSourceResolver;
        };
    
        std::string resolveImageSources(ImageSourceUpdateListener &listener, std::string uri);
        void addListenerForURI(const std::string &uri, ImageSourceUpdateListener *listener);
        void removeListenerForURI(const std::string &uri, ImageSourceUpdateListener *listener);
        void removeListener(ImageSourceUpdateListener *listener);

    public:
        void imageDownloadComplete(std::string uri,std::string fileUri);
        void imageDownloadFail(std::string uri);
    
        std::unordered_map<std::string, std::vector<ImageSourceUpdateListener *>> uriListenersMap;//网络图片与image组件映射
        LRUCache<std::string, bool> remoteImageSourceMap;//网络图片uri与磁盘文件地址映射
        std::unordered_set<std::string> pendingSet;//正在下载的文件
        std::string fileCacheDir;
        
    };
}

#endif //FASTIMAGESOURCERESOLVER_H