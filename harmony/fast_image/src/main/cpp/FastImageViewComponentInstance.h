#pragma once

#include "EventEmitters.h"
#include "ShadowNodes.h"
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/ImageNode.h"

namespace rnoh {
class FastImageViewComponentInstance : public CppComponentInstance<facebook::react::FastImageViewShadowNode>, public ImageNodeDelegate {
private:
    ImageNode m_imageNode;
    struct ImageRawProps {
        std::optional<std::string> resizeMethod;
        std::optional<bool> focusable;
        std::optional<std::string> alt;
        std::optional<std::string> loadingIndicatorSource;
//         static ImageRawProps getFromDynamic(folly::dynamic value);
    };
    ImageRawProps m_rawProps;

public:
    FastImageViewComponentInstance(Context context);
    void onPropsChanged(SharedConcreteProps const &props) override;
//     void onStateChanged(SharedConcreteState const &state) override;

//     void onComplete(float width, float height) override;
//     void onError(int32_t errorCode) override;
//     void onLoadStart();

    ImageNode &getLocalRootArkUINode() override;
};
} // namespace rnoh
