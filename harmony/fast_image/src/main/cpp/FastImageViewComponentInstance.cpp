#include "FastImageViewComponentInstance.h"
#include "Props.h"
#include <react/renderer/core/ConcreteState.h>
#include <sstream>

namespace rnoh {

FastImageViewComponentInstance::FastImageViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)) {
    this->getLocalRootArkUINode().setNodeDelegate(this);
    this->getLocalRootArkUINode().setInterpolation(ARKUI_IMAGE_INTERPOLATION_HIGH);
    this->getLocalRootArkUINode().setDraggable(false);
}
std::string FastImageViewComponentInstance::FindLocalCacheByUri(std::string const &uri) {
    if (uri.find("http", 0) != 0) {
        return uri;
    }

    auto rnInstance = m_deps->rnInstance.lock();
    if (!rnInstance) {
        return uri;
    }

    auto turboModule = rnInstance->getTurboModule("ImageLoader");
    if (!turboModule) {
        return uri;
    }

    auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
    if (!arkTsTurboModule) {
        return uri;
    }

    auto cache = arkTsTurboModule->callSync("getCacheFilePath", {uri});
    return cache.asString();
}

void FastImageViewComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
    CppComponentInstance::onPropsChanged(props);
    LOG(INFO) << "[FastImage] Props->tinColor: " << props->tintColor;
    LOG(INFO) << "[FastImage] Props->source.uri: " << props->source.uri;
    LOG(INFO) << "[FastImage] Props->resizeMode: " << facebook::react::toString(props->resizeMode);
    LOG(INFO) << "[FastImage] Props->defaultSource: " << props->defaultSource;
    
    if (!props->source.headers.empty()) {
        for (auto header : props->source.headers) {
            LOG(INFO) << "[FastImage] Props->headers.name: " << header.name;
            LOG(INFO) << "[FastImage] Props->headers.value: " << header.value;
        }
    }

    if (!m_props || m_props->source.uri != props->source.uri) {
        m_uri = props->source.uri;
        std::string uri = FindLocalCacheByUri(props->source.uri);
        this->getLocalRootArkUINode().setSources(uri);
        if (!m_uri.empty()) {
            onLoadStart();
        }
    }
    if (!m_props || m_props->resizeMode != props->resizeMode) {
        this->getLocalRootArkUINode().setResizeMode(convertToImageResizeMode(props->resizeMode));
    }

    if (!m_props || m_props->defaultSource != props->defaultSource) {

        if (!(props->defaultSource.empty())) {
            this->getLocalRootArkUINode().setAlt(props->defaultSource);
        }
    }

    if (!m_props || m_props->tintColor != props->tintColor) {
        this->getLocalRootArkUINode().setTintColor(props->tintColor);
    }
}

// void FastImageViewComponentInstance::onStateChanged(SharedConcreteState const& state) {
//   CppComponentInstance::onStateChanged(state);
//   auto vector = {state->getData().getImageSource()};
//   this->getLocalRootArkUINode().setSources(vector);
//   this->getLocalRootArkUINode().setBlur(state->getData().getBlurRadius());
// }

ImageNode &FastImageViewComponentInstance::getLocalRootArkUINode() { return m_imageNode; }

void FastImageViewComponentInstance::onComplete(float width, float height) {
    if (m_eventEmitter == nullptr) {
        return;
    }
    m_eventEmitter->onFastImageLoad({width, height});
    m_eventEmitter->onFastImageProgress({1, 1});
    m_eventEmitter->onFastImageLoadEnd({});
}

void FastImageViewComponentInstance::onError(int32_t errorCode) {
    if (m_eventEmitter == nullptr) {
        return;
    }
    m_eventEmitter->onFastImageLoadEnd({});
}

void FastImageViewComponentInstance::onLoadStart() {
    if (m_eventEmitter) {
        m_eventEmitter->onFastImageLoadStart({});
    }
}

facebook::react::ImageResizeMode
FastImageViewComponentInstance::convertToImageResizeMode(facebook::react::FastImageViewResizeMode mode) {
    switch (mode) {
    case facebook::react::FastImageViewResizeMode::Contain:
        return facebook::react::ImageResizeMode::Contain;
    case facebook::react::FastImageViewResizeMode::Cover:
        return facebook::react::ImageResizeMode::Cover;
    case facebook::react::FastImageViewResizeMode::Stretch:
        return facebook::react::ImageResizeMode::Stretch;
    case facebook::react::FastImageViewResizeMode::Center:
        return facebook::react::ImageResizeMode::Center;
    }
}
} // namespace rnoh
