/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

/**
 * Used only in C-API based Architecture.
 */
#pragma once
#include <react/renderer/imagemanager/primitives.h>
#include "RNOH/arkui/ArkUINode.h"

namespace rnoh {

class FastImageNodeDelegate {
 public:
  virtual ~FastImageNodeDelegate() = default;
  virtual void onComplete(float width, float height){};
  virtual void onError(int32_t errorCode){};
  virtual void onProgress(uint32_t loaded, uint32_t total){};
};

class FastImageNode : public ArkUINode {
 protected:
  ArkUI_NodeHandle m_childArkUINodeHandle;
  FastImageNodeDelegate* m_fastImageNodeDelegate;

 public:
  FastImageNode();
  ~FastImageNode();
  FastImageNode& setSources(std::string const& uri, std::string prefix = "");
  FastImageNode& setResizeMode(facebook::react::ImageResizeMode const& mode);
  FastImageNode& setTintColor(facebook::react::SharedColor const& sharedColor);
  FastImageNode& setBlur(facebook::react::Float blur);
  FastImageNode& setObjectRepeat(
      facebook::react::ImageResizeMode const& resizeMode);
  FastImageNode& setCapInsets(facebook::react::EdgeInsets const& capInsets, float dpi);

  FastImageNode& setInterpolation(int32_t interpolation);
  FastImageNode& setDraggable(bool draggable);
  FastImageNode& setFocusable(bool focusable);
  FastImageNode& setResizeMethod(std::string const& resizeMethod);
  FastImageNode& setAlt(std::string const& uri, std::string prefix = "");
  FastImageNode& setFadeDuration(int32_t duration);

  FastImageNode& resetFocusable();
  FastImageNode& resetResizeMethod();
  FastImageNode& setAutoResize(bool autoResize);
  FastImageNode& resetSources();

  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;
  void setNodeDelegate(FastImageNodeDelegate* fastImageNodeDelegate);
};
} // namespace rnoh
