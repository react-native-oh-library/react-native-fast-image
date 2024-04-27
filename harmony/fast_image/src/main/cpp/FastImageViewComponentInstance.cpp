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

void FastImageViewComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
    CppComponentInstance::onPropsChanged(props);
    LOG(INFO) << "[FastImage] Props->tinColor: " << props->tintColor;
    LOG(INFO) << "[FastImage] Props->source.uri: " << props->source.uri;
    LOG(INFO) << "[FastImage] Props->resizeMode: " << facebook::react::toString(props->resizeMode);
    if (!props->source.headers.empty()) {
        for (auto header: props->source.headers) {
            LOG(INFO) << "[FastImage] Props->headers.name: " << header.name;
            LOG(INFO) << "[FastImage] Props->headers.value: " << header.value;
        }
    }
    if (!m_props || m_props->source.uri != props->source.uri) {
        facebook::react::ImageSources imageSoures;
        facebook::react::ImageSource imageSoure;
        imageSoure.uri = props->source.uri;
        imageSoures.push_back(imageSoure);
        this->getLocalRootArkUINode().setSources(imageSoures);
        if (!this->getLocalRootArkUINode().getUri().empty()) {
            onLoadStart();
        }
    }
    if (!m_props || m_props->resizeMode != props->resizeMode) {
        this->getLocalRootArkUINode().setResizeMode(convertToImageResizeMode(props->resizeMode));
    }

    if (!m_props || m_props->defaultSource != props->defaultSource) {
        this->getLocalRootArkUINode().setAlt(props->defaultSource);
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

facebook::react::ImageResizeMode FastImageViewComponentInstance::convertToImageResizeMode(facebook::react::FastImageViewResizeMode mode) {
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
