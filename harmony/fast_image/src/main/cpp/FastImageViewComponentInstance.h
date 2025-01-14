/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <react/renderer/components/react_native_fast_image/EventEmitters.h>
#include <react/renderer/components/react_native_fast_image/ShadowNodes.h>
#include "FastImageNode.h"
#include "FastImageLoaderTurboModule.h"
#include "RNOH/generated/components/BaseFastImageViewComponentInstance.h"
#include "RNOH/arkui/ImageNode.h"

namespace rnoh {
class FastImageViewComponentInstance : public BaseFastImageViewComponentInstance,
                                       public FastImageNodeDelegate,
                                       public FastImageLoaderTurboModule::FastImageSourceResolver::ImageSourceUpdateListener {
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
    void onImageSourceCacheUpdate(std::string fileUri) override;
    void onImageSourceCacheDownloadFileFail() override;
};
} // namespace rnoh
