/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#include "FastImageViewComponentInstance.h"
#include <react/renderer/components/react_native_fast_image/Props.h>

namespace rnoh {

FastImageViewComponentInstance::FastImageViewComponentInstance(Context context)
    : BaseFastImageViewComponentInstance(std::move(context)) {
    // 设置委托，FastImageViewComponentInstance 实现了 FastImageNodeDelegate 接口
    this->getLocalRootArkUINode().setNodeDelegate(this);
    // 设置一些属性的默认值（不对RN前端暴露的属性）
    this->getLocalRootArkUINode().setInterpolation(ARKUI_IMAGE_INTERPOLATION_HIGH);
    this->getLocalRootArkUINode().setDraggable(false);
}

/**
 * @brief RN前端属性变化时进入的回调，通常在这里将属取出封装后设置给 ArkUI 节点
 * @param props SharedConcreteProps对象
 */
void FastImageViewComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
    CppComponentInstance::onPropsChanged(props);

    // 通过 getLocalRootArkUINode 获取 ImageNode 实例，并调用方法设置属性
    if (!m_props || m_props->source.uri != props->source.uri) {
        if (!props->source.uri.empty()) {
            onLoadStart();
        }
        this->getLocalRootArkUINode().setSources(props->source.uri); 
    }
}

/**
 * @brief 访问 ComponentInstance 对应的 ArkUINode 对象
 */
FastImageNode &FastImageViewComponentInstance::getLocalRootArkUINode() { return m_imageNode; }

void FastImageViewComponentInstance::onProgress(uint32_t loaded, uint32_t total) {
    if (m_eventEmitter) {
        m_eventEmitter->onFastImageProgress({static_cast<int>(loaded), static_cast<int>(total)});
    }
}


void FastImageViewComponentInstance::onComplete(float width, float height) {
    if (m_eventEmitter) {
        m_eventEmitter->onFastImageLoad({width, height});
        m_eventEmitter->onFastImageLoadEnd({});
    }
}

void FastImageViewComponentInstance::onError(int32_t errorCode) {
    if (m_eventEmitter) {
        m_eventEmitter->onFastImageError({});
        m_eventEmitter->onFastImageLoadEnd({});
    }
}

void FastImageViewComponentInstance::onLoadStart() {
    if (m_eventEmitter) {
        m_eventEmitter->onFastImageLoadStart({});
    }
}

} // namespace rnoh