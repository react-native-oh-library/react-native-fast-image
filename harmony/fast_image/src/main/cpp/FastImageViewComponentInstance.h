#ifndef FASTIMAGEVIEWCOMPONENTINSTANCE_H
#define FASTIMAGEVIEWCOMPONENTINSTANCE_H

#include "EventEmitters.h"
#include "FastImageNode.h"
#include "RNCFastImageViewTurboModule.h"
#include "FastImageSourceResolver.h"
#include "ShadowNodes.h"
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/ImageNode.h"

namespace rnoh {
class FastImageViewComponentInstance : public CppComponentInstance<facebook::react::FastImageViewShadowNode>,
                                       public FastImageNodeDelegate,
                                       public FastImageSourceResolver::ImageSourceUpdateListener {
private:
    FastImageNode m_imageNode;
    std::string m_uri;
    bool m_isReload = false;
    facebook::react::FastImageViewSourceStruct m_source;
    // used for find local cache of uri, if not find return uri
    std::string FindLocalCacheByUri(std::string const &uri);
    void GetHeaderUri(std::string const &uri,std::vector<facebook::react::FastImageViewSourceHeadersStruct> const &header);
    std::string getBundlePath();
    std::string getAbsolutePathPrefix(std::string const &bundlePath);
    std::optional<std::string> getTintColorFromDynamic(folly::dynamic value);

public:
    FastImageViewComponentInstance(Context context);
    void onPropsChanged(SharedConcreteProps const &props) override;
    //     void onStateChanged(SharedConcreteState const &state) override;

    void onComplete(float width, float height) override;
    void onProgress(uint32_t loaded, uint32_t total) override;
    void onError(int32_t errorCode) override;
    void onLoadStart();

    facebook::react::ImageResizeMode convertToImageResizeMode(facebook::react::FastImageViewResizeMode mode);

    FastImageNode &getLocalRootArkUINode() override;
    
    // ImageSourceResolver::ImageSourceUpdateListener
    void onImageSourceCacheUpdate(std::string image) override;
    void onImageSourceCacheDownloadFileFail() override;
};
} // namespace rnoh

#endif //FASTIMAGEVIEWCOMPONENTINSTANCE_H