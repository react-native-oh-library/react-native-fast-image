/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#include "FastImageNode.h"
#include "RNOH/arkui/NativeNodeApi.h"

static constexpr ArkUI_NodeEventType IMAGE_NODE_EVENT_TYPES[] = {NODE_IMAGE_ON_COMPLETE, NODE_IMAGE_ON_ERROR,
                                                                 NODE_IMAGE_ON_DOWNLOAD_PROGRESS};

namespace rnoh {

FastImageNode::FastImageNode()
    : ArkUINode(
          // 通过 NativeNodeApi 创建ArkUI节点，返回 ArkUI_NodeHandle
          // TODO 替换成 imageknife 的创建接口
          NativeNodeApi::getInstance()->createNode(ArkUI_NodeType::ARKUI_NODE_IMAGE)
          ),
      m_fastImageNodeDelegate(nullptr) {
    // 通过 NativeNodeApi 注册 ArkUI 原生事件
    // TODO 原生事件由 imageknife 注册
    for (auto eventType : IMAGE_NODE_EVENT_TYPES) {
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, eventType, eventType, this));
    }
}

FastImageNode::~FastImageNode() {
    // 通过 NativeNodeApi 取消注册 ArkUI 原生事件
    for (auto eventType : IMAGE_NODE_EVENT_TYPES) {
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, eventType);
    }
}

void FastImageNode::setNodeDelegate(FastImageNodeDelegate *fastImageNodeDelegate) {
    m_fastImageNodeDelegate = fastImageNodeDelegate;
}

/**
 * @brief 监听到原生ArkUI事件后，RN框架统一分发事件到对应 ArkUINode 的 onNodeEvent
 * @param eventType
 * @param eventArgs
 */
void FastImageNode::onNodeEvent(ArkUI_NodeEventType eventType, EventArgs &eventArgs) {
    // TODO 对接 imageknife 的事件回调入口
    // 通过委托类 m_fastImageNodeDelegate 将事件传递到 componentInstance
    if (eventType == ArkUI_NodeEventType::NODE_IMAGE_ON_COMPLETE) {
        if (m_fastImageNodeDelegate != nullptr && eventArgs[0].i32 == 1) {
            m_fastImageNodeDelegate->onComplete(eventArgs[1].f32, eventArgs[2].f32);
        }
    }

    if (eventType == ArkUI_NodeEventType::NODE_IMAGE_ON_ERROR) {
        if (m_fastImageNodeDelegate != nullptr) {
            m_fastImageNodeDelegate->onError(eventArgs[0].i32);
        }
    }

    if (eventType == ArkUI_NodeEventType::NODE_IMAGE_ON_DOWNLOAD_PROGRESS) {
        if (m_fastImageNodeDelegate != nullptr) {
            m_fastImageNodeDelegate->onProgress(eventArgs[0].u32, eventArgs[1].u32);
        }
    }
}

/**
 * @brief 设置 NODE_IMAGE_SRC 属性
 * @param uri 图片uri
 * @return *this
 */
FastImageNode &FastImageNode::setSources(std::string const &uri) {
    // TODO 替换成 imageknife 的属性设置接口
    ArkUI_AttributeItem item;
    item = {.string = uri.c_str()};
    maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_IMAGE_SRC, &item));
    return *this;
}

/**
 * @brief 设置 NODE_IMAGE_INTERPOLATION 属性
 * @param interpolation 图像插值质量枚举值(ArkUI_ImageInterpolation)
 * @return *this
 */
FastImageNode &FastImageNode::setInterpolation(int32_t interpolation) {
    // TODO 替换成 imageknife 的属性设置接口
    ArkUI_NumberValue value[] = {{.i32 = interpolation}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_IMAGE_INTERPOLATION, &item));
    return *this;
}

/**
 * @brief 设置 NODE_IMAGE_DRAGGABLE 属性
 * @param draggable 图片是否可拖拽
 * @return *this
 */
FastImageNode &FastImageNode::setDraggable(bool draggable) {
    // TODO 替换成 imageknife 的属性设置接口
    ArkUI_NumberValue value[] = {{.i32 = static_cast<int32_t>(draggable)}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_IMAGE_DRAGGABLE, &item));
    return *this;
}

} // namespace rnoh
