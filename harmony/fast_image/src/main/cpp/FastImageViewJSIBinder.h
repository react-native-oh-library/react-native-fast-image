/**
 * MIT License
 * 
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANT KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh{
  class FastImageViewJSIBinder : public ViewComponentJSIBinder{
    facebook::jsi::Object createNativeProps(facebook::jsi::Runtime &rt) override
    {
      auto object = ViewComponentJSIBinder::createNativeProps(rt);
      object.setProperty(rt,"source","string");
      object.setProperty(rt,"resizeMode","string");
      object.setProperty(rt,"headers","string");
      object.setProperty(rt,"priority","string");
      object.setProperty(rt,"cache","string");
      object.setProperty(rt,"tintColor","Color");
      object.setProperty(rt,"defaultSource","string");
      return object;
    }

    facebook::jsi::Object createDirectEventTypes(facebook::jsi::Runtime &rt) override
    {
      facebook::jsi::Object events(rt);
      events.setProperty(rt,"topLoadStart",createDirectEvent(rt,"onLoadStart"));
      events.setProperty(rt,"topProgress",createDirectEvent(rt,"onProgress"));
      events.setProperty(rt,"topLoad",createDirectEvent(rt,"onLoad"));
      events.setProperty(rt,"topError",createDirectEvent(rt,"onError"));
      events.setProperty(rt,"topLoadEnd",createDirectEvent(rt,"onLoadEnd"));
      return events;
    }
  };
} // namespace rnoh