/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#pragma once

#include "RNOH/arkui/ArkUINode.h"

namespace rnoh {

/**
 * fast-image原生事件委托对象类，FastImageNode 可以通过 FastImageNodeDelegate 通知外部有关事件
 */
class FastImageNodeDelegate {
public:
    virtual ~FastImageNodeDelegate() = default;
    /**
     * @brief 处理加载完成事件
     * @param width
     * @param height
     */
    virtual void onComplete(float width, float height){};
    virtual void onError(int32_t errorCode){};
    virtual void onProgress(uint32_t loaded, uint32_t total){};
};

class FastImageNode : public ArkUINode {
protected:
    FastImageNodeDelegate *m_fastImageNodeDelegate;

public:
    FastImageNode();
    ~FastImageNode();

    FastImageNode &setSources(std::string const &uri);
    FastImageNode &setInterpolation(int32_t interpolation);
    FastImageNode &setDraggable(bool draggable);
    // TODO 提供更多的属性设置API

    void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs &eventArgs) override;
    void setNodeDelegate(FastImageNodeDelegate *fastImageNodeDelegate);
};
} // namespace rnoh
