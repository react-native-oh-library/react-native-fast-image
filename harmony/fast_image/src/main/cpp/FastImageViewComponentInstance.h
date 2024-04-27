#pragma once

#include "EventEmitters.h"
#include "ShadowNodes.h"
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/ImageNode.h"

namespace rnoh {
class FastImageViewComponentInstance : public CppComponentInstance<facebook::react::FastImageViewShadowNode>, public ImageNodeDelegate {
private:
    ImageNode m_imageNode;

public:
    FastImageViewComponentInstance(Context context);
    void onPropsChanged(SharedConcreteProps const &props) override;
//     void onStateChanged(SharedConcreteState const &state) override;

    void onComplete(float width, float height) override;
    void onError(int32_t errorCode) override;
    void onLoadStart();
    
    facebook::react::ImageResizeMode convertToImageResizeMode(facebook::react::FastImageViewResizeMode mode);

    ImageNode &getLocalRootArkUINode() override;
};
} // namespace rnoh
