#pragma once

#include "Props.h"
#include "RNOH/ArkTSTurboModule.h"
#include <ReactCommon/RuntimeExecutor.h>
#include <react/renderer/imagemanager/primitives.h>
#include "FastImageLoaderTurboModule.h"
#include "RNOH/ArkTSMessageHub.h"
#include "RNOH/Assert.h"
#include "RNOH/RNInstance.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"

constexpr char FAST_IMAGE_SOURCE_PENDING[] = "FAST_IMAGE_SOURCE_PENDING";

namespace rnoh {

class JSI_EXPORT FastImageLoaderTurboModule : public ArkTSTurboModule {
public:
    using Shared = std::shared_ptr<FastImageLoaderTurboModule>;

    FastImageLoaderTurboModule(const ArkTSTurboModule::Context ctx, const std::string name);

    class FastImageSourceResolver : public ArkTSMessageHandler {
    public:
        using Shared = std::shared_ptr<FastImageSourceResolver>;

        FastImageSourceResolver(std::weak_ptr<RNInstance> rnInstance)
            : m_rnInstance(rnInstance) {}

        class ImageSourceUpdateListener {
        public:
            std::string observedUri;

            ImageSourceUpdateListener(FastImageSourceResolver::Shared const &FastImageSourceResolver)
                : m_FastImageSourceResolver(FastImageSourceResolver){};

            ~ImageSourceUpdateListener() { 
                if(m_FastImageSourceResolver != nullptr){
                    m_FastImageSourceResolver->removeListener(this);
                }            
            }

            virtual void onImageSourceCacheUpdate(std::string fileUri) = 0;
            virtual void onImageSourceCacheDownloadFileFail() = 0;

        private:
            FastImageSourceResolver::Shared const &m_FastImageSourceResolver;
        };

        std::string resolveImageSources(ImageSourceUpdateListener &listener, std::string uri) {
            // Subscribe to get information about prefetched URIs.
            if (uriListenersMap.find(uri) == uriListenersMap.end()) {
                removeListener(&listener);
                addListenerForURI(uri, &listener);
            }

            if (auto it = remoteImageSourceMap.find(uri);
                it == remoteImageSourceMap.end() || it->second == FAST_IMAGE_SOURCE_PENDING) {
                auto resolvedFileUri = getPrefetchedImageFileUri(uri);
                if (!resolvedFileUri.empty()) {
                    remoteImageSourceMap.emplace(uri, resolvedFileUri);
                }
            }

            if (auto it = remoteImageSourceMap.find(uri); it != remoteImageSourceMap.end()) {
                if (it->second == FAST_IMAGE_SOURCE_PENDING) {
                    return "";
                }
                uri = it->second;
            }
        
            return uri;
        }

        void addListenerForURI(const std::string &uri, ImageSourceUpdateListener *listener) {
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

        void removeListenerForURI(const std::string &uri, ImageSourceUpdateListener *listener) {
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

        void removeListener(ImageSourceUpdateListener *listener) {
            removeListenerForURI(listener->observedUri, listener);
        }
    
        void clearRemoteImageSourceMap(){
            remoteImageSourceMap.clear();
        }

    protected:
        virtual void handleArkTSMessage(const Context& ctx) override {
            if (ctx.messageName == "UPDATE_FAST_IMAGE_SOURCE_MAP") {
                auto remoteUri = ctx.messagePayload["remoteUri"].asString();
                auto fileUri = ctx.messagePayload["fileUri"].asString();
                auto it = uriListenersMap.find(remoteUri);
                if (it == uriListenersMap.end()) {
                    return;
                }
                auto &listeners = it->second;
                remoteImageSourceMap.insert_or_assign(remoteUri, fileUri);
                for (auto listener : listeners) {
                    listener->onImageSourceCacheUpdate(fileUri);
                    removeListenerForURI(remoteUri, listener);
                }
            } else if (ctx.messageName == "FAST_IMAGE_DOWNLOAD_FILE_FAIL") {
                auto remoteUri = ctx.messagePayload["remoteUri"].asString();
                auto it = uriListenersMap.find(remoteUri);
                if (it == uriListenersMap.end()) {
                    return;
                }
                auto &listeners = it->second;
                for (auto listener : listeners) {
                    listener->onImageSourceCacheDownloadFileFail();
                    removeListenerForURI(remoteUri, listener);
                }
            }
        }

    private:
        std::unordered_map<std::string, std::vector<ImageSourceUpdateListener *>> uriListenersMap;
        std::unordered_map<std::string, std::string> remoteImageSourceMap;
        std::weak_ptr<RNInstance> m_rnInstance;

        std::string getPrefetchedImageFileUri(std::string uri) {
            auto rnInstance = m_rnInstance.lock();
            if (rnInstance == nullptr) {
                return {};
            }
            auto imageLoaderTurboModule = rnInstance->getTurboModule<FastImageLoaderTurboModule>("FastImageLoader");
            if (imageLoaderTurboModule != nullptr) {
                auto prefetchResult = imageLoaderTurboModule->callSync("getPrefetchResult", {uri});
                if (prefetchResult == "pending") {
                    // The uri will be updated in onMessageReceived method.
                    return FAST_IMAGE_SOURCE_PENDING;
                }
                if (prefetchResult != nullptr) {
                    return prefetchResult.asString();
                }
                
            }
            return {};
        }
    };

    std::shared_ptr<FastImageSourceResolver> m_FastImageSourceResolver = nullptr;

private:

};

} // namespace rnoh