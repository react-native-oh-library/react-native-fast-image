#include "FastImageViewComponentInstance.h"
#include "Props.h"
#include <react/renderer/core/ConcreteState.h>
#include <sstream>

namespace rnoh {

FastImageViewComponentInstance::FastImageViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)) {
  this->getLocalRootArkUINode().setNodeDelegate(this);
  this->getLocalRootArkUINode().setInterpolation(
      ARKUI_IMAGE_INTERPOLATION_HIGH);
  this->getLocalRootArkUINode().setDraggable(false);
}

void FastImageViewComponentInstance::onPropsChanged(SharedConcreteProps const& props) {
  CppComponentInstance::onPropsChanged(props);
//   auto rawProps = ImageRawProps::getFromDynamic(props->rawProps);
    LOG(INFO) << "[FastImage] Props->tinColor: " << props->tintColor;

}

// void FastImageViewComponentInstance::onStateChanged(SharedConcreteState const& state) {
//   CppComponentInstance::onStateChanged(state);
//   auto vector = {state->getData().getImageSource()};
//   this->getLocalRootArkUINode().setSources(vector);
//   this->getLocalRootArkUINode().setBlur(state->getData().getBlurRadius());
// }

ImageNode& FastImageViewComponentInstance::getLocalRootArkUINode() {
  return m_imageNode;
}

// void ImageComponentInstance::onComplete(float width, float height) {
//   if (m_eventEmitter == nullptr) {
//     return;
//   }
//
//   std::string uri = this->getLocalRootArkUINode().getUri();
//   m_eventEmitter->dispatchEvent("load", [=](facebook::jsi::Runtime& runtime) {
//     auto payload = facebook::jsi::Object(runtime);
//     auto source = facebook::jsi::Object(runtime);
//     source.setProperty(runtime, "width", width);
//     source.setProperty(runtime, "height", height);
//     source.setProperty(runtime, "uri", uri.c_str());
//     payload.setProperty(runtime, "source", source);
//     return payload;
//   });
//   m_eventEmitter->onLoadEnd();
// }

// void ImageComponentInstance::onError(int32_t errorCode) {
//   if (m_eventEmitter == nullptr) {
//     return;
//   }
//
//   std::string errMsg = "error message: the image load failed, ";
//   if (errorCode == 401) {
//     errMsg =
//         "error message: the image could not be obtained because the image path is invalid, ";
//   } else if (errorCode == 103101) {
//     errMsg = "error message: the image format is not supported, ";
//   }
//
//   errMsg += std::string("error code: ") + std::to_string(errorCode);
//   m_eventEmitter->dispatchEvent(
//       "error", [errMsg](facebook::jsi::Runtime& runtime) {
//         auto payload = facebook::jsi::Object(runtime);
//         auto source = facebook::jsi::Object(runtime);
//         source.setProperty(runtime, "error", errMsg);
//         payload.setProperty(runtime, "source", source);
//         return payload;
//       });
//   m_eventEmitter->onLoadEnd();
// }
//
// void ImageComponentInstance::onLoadStart() {
//   if (m_eventEmitter) {
//     m_eventEmitter->onLoadStart();
//   }
// }

// FastImageViewComponentInstance::ImageRawProps
// FastImageViewComponentInstance::ImageRawProps::getFromDynamic(folly::dynamic value) {
//   auto resizeMode = (value.count("resizeMode") > 0)
//       ? std::optional(value["resizeMode"].asString())
//       : std::nullopt;
//   auto focusable = (value.count("focusable") > 0)
//       ? std::optional(value["focusable"].asBool())
//       : std::nullopt;
//   auto alt = (value.count("alt") > 0) ? std::optional(value["alt"].asString())
//                                       : std::nullopt;
//   auto loadingIndicatorSource = (value.count("loadingIndicatorSource") > 0)
//       ? std::optional(value["loadingIndicatorSource"].at("uri").getString())
//       : std::nullopt;
//
//   return {resizeMode, focusable, alt, loadingIndicatorSource};
// }

} // namespace rnoh
