#include "FastImageViewComponentInstance.h"
#include "FastImageSource.h"
#include "FastImageLoaderTurboModule.h"
#include "Props.h"
#include <iomanip>
#include <react/renderer/core/ConcreteState.h>
#include <sstream>
#include "colorUtils/Color.h"

namespace rnoh {

const std::string BUNDLE_HARMONY_JS = "bundle.harmony.js";
const std::string RAWFILE_PREFIX = "resource://RAWFILE/assets/";

FastImageViewComponentInstance::FastImageViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)),
      FastImageLoaderTurboModule::FastImageSourceResolver::ImageSourceUpdateListener(
          (std::dynamic_pointer_cast<rnoh::FastImageLoaderTurboModule>(
               m_deps->rnInstance.lock()->getTurboModule("FastImageLoader")))
              ->m_FastImageSourceResolver) {
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

    auto turboModule = rnInstance->getTurboModule("FastImageLoader");
    if (!turboModule) {
        return uri;
    }

    auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::FastImageLoaderTurboModule>(turboModule);
    if (!arkTsTurboModule) {
        return uri;
    }

    if (!arkTsTurboModule->m_FastImageSourceResolver) {
        return uri;
    } else {
        return arkTsTurboModule->m_FastImageSourceResolver->resolveImageSources(*this, uri);
    }
}

void FastImageViewComponentInstance::GetHeaderUri(
    std::string const &uri, std::vector<facebook::react::FastImageViewSourceHeadersStruct> const &header) {
    auto rnInstance = m_deps->rnInstance.lock();
    if (!rnInstance) {
        m_eventEmitter->onFastImageError({});
        return;
    }
    auto turboModule = rnInstance->getTurboModule("FastImageLoader");
    if (!turboModule) {
        m_eventEmitter->onFastImageError({});
        return;
    }
    auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::FastImageLoaderTurboModule>(turboModule);
    if (!arkTsTurboModule) {
        m_eventEmitter->onFastImageError({});
        return;
    }
    std::vector<ArkJS::IntermediaryArg> args;
    args.push_back(uri);
    folly::dynamic args_header = folly::dynamic::object();
    for (auto it = header.begin(); it != header.end(); ++it) {
        args_header[it->name] = it->value;
    }
    args.push_back(args_header);
    arkTsTurboModule->callSync("prefetchImage", args);
    return;
}

std::string FastImageViewComponentInstance::getBundlePath() {
    auto rnInstance = m_deps->rnInstance.lock();
    if (!rnInstance) {
        return BUNDLE_HARMONY_JS;
    }

    auto internalInstance = std::dynamic_pointer_cast<RNInstanceInternal>(rnInstance);
    if (!internalInstance) {
        return BUNDLE_HARMONY_JS;
    }

    return internalInstance->getBundlePath();
}

std::string FastImageViewComponentInstance::getAbsolutePathPrefix(std::string const &bundlePath) {
    if (bundlePath.find('/', 0) != 0) {
        return RAWFILE_PREFIX;
    }

    auto pos = bundlePath.rfind('/');
    if (pos == std::string::npos) {
        return RAWFILE_PREFIX;
    }

    std::string prefix = "file://" + bundlePath.substr(0, pos + 1);
    return prefix;
}

std::optional<std::string> FastImageViewComponentInstance::getTintColorFromDynamic(folly::dynamic value) {
    auto rawPropsColor = (value.count("tintColor") > 0) ? std::optional(value["tintColor"].asString()) : std::nullopt;
    return rawPropsColor;
}

void FastImageViewComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
    CppComponentInstance::onPropsChanged(props);
    DLOG(INFO) << "[FastImage] Props->tinColor: " << props->tintColor;
    DLOG(INFO) << "[FastImage] Props->source.uri: " << props->source.uri;
    DLOG(INFO) << "[FastImage] Props->resizeMode: " << facebook::react::toString(props->resizeMode);
    DLOG(INFO) << "[FastImage] Props->defaultSource: " << props->defaultSource;

    if (!props->source.headers.empty()) {
        for (auto header : props->source.headers) {
            DLOG(INFO) << "[FastImage] Props->headers.name: " << header.name;
            DLOG(INFO) << "[FastImage] Props->headers.value: " << header.value;
        }
    }

    if (!m_props || m_props->source.uri != props->source.uri) {
        this->m_isReload = false;
        this->m_source = props->source;
        m_uri = props->source.uri;
        std::string uri = FindLocalCacheByUri(m_uri);
        this->getLocalRootArkUINode().setAutoResize(true);

        if (uri.empty()) {
            if (!props->source.headers.empty()) {
                this->getLocalRootArkUINode().resetSources();
                GetHeaderUri(props->source.uri, props->source.headers);
            } else {
                this->getLocalRootArkUINode().resetSources();
            }
        } else {
            if (!props->source.headers.empty() && uri.find("http", 0) == 0) {
                this->getLocalRootArkUINode().resetSources();
                GetHeaderUri(props->source.uri, props->source.headers);
            } else {
                this->getLocalRootArkUINode().setSources(uri, getAbsolutePathPrefix(getBundlePath()));
            }
        }

        if (!m_uri.empty()) {
            onLoadStart();
        }
    }
    if (!m_props || m_props->resizeMode != props->resizeMode) {
        this->getLocalRootArkUINode().setResizeMode(convertToImageResizeMode(props->resizeMode));
    }

    if (!m_props || m_props->defaultSource != props->defaultSource) {

        if (!(props->defaultSource.empty())) {
            this->getLocalRootArkUINode().setAlt(props->defaultSource, getAbsolutePathPrefix(getBundlePath()));
        }
    }

    if (props->rawProps != nullptr) {
        auto tintColor = getTintColorFromDynamic(props->rawProps);
        if (tintColor.has_value()) {
            auto tintColorValue = fastimage::Color::FromString(tintColor.value()).GetValue();
            this->getLocalRootArkUINode().setTintColor(SharedColor(tintColorValue));
        }
    }
}

void FastImageViewComponentInstance::onImageSourceCacheUpdate(std::string fileUri) {
    this->getLocalRootArkUINode().setSources(fileUri, getAbsolutePathPrefix(getBundlePath()));
}
void FastImageViewComponentInstance::onImageSourceCacheDownloadFileFail() {
    if (m_eventEmitter == nullptr) {
        return;
    }
    m_eventEmitter->onFastImageError({});
    m_eventEmitter->onFastImageLoadEnd({});
}

// void FastImageViewComponentInstance::onStateChanged(SharedConcreteState const& state) {
//   CppComponentInstance::onStateChanged(state);
//   auto vector = {state->getData().getImageSource()};
//   this->getLocalRootArkUINode().setSources(vector);
//   this->getLocalRootArkUINode().setBlur(state->getData().getBlurRadius());
// }

FastImageNode &FastImageViewComponentInstance::getLocalRootArkUINode() { return m_imageNode; }

void FastImageViewComponentInstance::onProgress(uint32_t loaded, uint32_t total) {
    if (m_eventEmitter) {
        m_eventEmitter->onFastImageProgress({static_cast<int>(loaded), static_cast<int>(total)});
    }
}

void FastImageViewComponentInstance::onComplete(float width, float height) {
    if (m_eventEmitter == nullptr) {
        return;
    }
    m_eventEmitter->onFastImageLoad({width, height});
    m_eventEmitter->onFastImageLoadEnd({});
}

void FastImageViewComponentInstance::onError(int32_t errorCode) {
    if (!m_isReload) {
        FastImageSource fastImageSource(m_source);
        std::string uri = fastImageSource.getUri();
        m_isReload = true;
        if (uri.compare(m_source.uri)) {
            this->getLocalRootArkUINode().setSources(uri, getAbsolutePathPrefix(getBundlePath()));
            return;
        }
    }
    if (m_eventEmitter == nullptr) {
        return;
    }
    m_eventEmitter->onFastImageError({});
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