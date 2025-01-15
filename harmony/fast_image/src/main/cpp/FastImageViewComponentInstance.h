/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#pragma once

#include "FastImageNode.h"
#include "RNOH/generated/components/BaseFastImageViewComponentInstance.h"

namespace rnoh {
class FastImageViewComponentInstance : public BaseFastImageViewComponentInstance,
                                       public FastImageNodeDelegate {
private:
    // 持有的 ArkUINode 对象
    FastImageNode m_imageNode;

public:
    FastImageViewComponentInstance(Context context);
    void onPropsChanged(SharedConcreteProps const &props) override;

    // 实现委托类中的方法
    void onComplete(float width, float height) override;
    void onProgress(uint32_t loaded, uint32_t total) override;
    void onError(int32_t errorCode) override;
    void onLoadStart();

    FastImageNode &getLocalRootArkUINode() override;
};
} // namespace rnoh
