/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#pragma once

// This file was generated.

#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh {
class FastImageViewJSIBinder : public ViewComponentJSIBinder {
protected:
    facebook::jsi::Object createNativeProps(facebook::jsi::Runtime &rt) override {
        auto object = ViewComponentJSIBinder::createNativeProps(rt);
        object.setProperty(rt, "source", "Object");
        object.setProperty(rt, "defaultSource", true);
        object.setProperty(rt, "resizeMode", true);
        object.setProperty(rt, "tintColor", true);
        return object;
    }

    facebook::jsi::Object createBubblingEventTypes(facebook::jsi::Runtime &rt) override {
        facebook::jsi::Object events(rt);
        events.setProperty(rt, "topFastImageError", createBubblingCapturedEvent(rt, "onFastImageError"));
        events.setProperty(rt, "topFastImageLoad", createBubblingCapturedEvent(rt, "onFastImageLoad"));
        events.setProperty(rt, "topFastImageLoadEnd", createBubblingCapturedEvent(rt, "onFastImageLoadEnd"));
        events.setProperty(rt, "topFastImageLoadStart", createBubblingCapturedEvent(rt, "onFastImageLoadStart"));
        events.setProperty(rt, "topFastImageProgress", createBubblingCapturedEvent(rt, "onFastImageProgress"));
        return events;
    }

    facebook::jsi::Object createDirectEventTypes(facebook::jsi::Runtime &rt) override {
        facebook::jsi::Object events(rt);
        return events;
    }
};
} // namespace rnoh
