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
#include <napi/native_api.h>
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"
#include "EventEmitters.h"

using namespace facebook;
namespace rnoh {

enum FastImageEventType {
    FAST_IMAGE_LOAD_START = 0,
    FAST_IMAGE_PROGRESS = 1,
    FAST_IMAGE_LOAD = 2,
    FAST_IMAGE_ERROR = 3,
    FAST_IMAGE_LOAD_END = 4,
};

FastImageEventType getFastImageEventType(ArkJS &arkJs, napi_value eventObject)
{
    auto eventType = arkJs.getString(arkJs.getObjectProperty(eventObject, "type"));
    if (eventType == "onLoadStart") {
        return FastImageEventType::FAST_IMAGE_LOAD_START;
    } else if (eventType == "onProgress") {
        return FastImageEventType::FAST_IMAGE_PROGRESS;
    } else if (eventType == "onLoad") {
        return FastImageEventType::FAST_IMAGE_LOAD;
    } else if (eventType == "onError") {
        return FastImageEventType::FAST_IMAGE_ERROR;
    } else if (eventType == "onLoadEnd") {
        return FastImageEventType::FAST_IMAGE_LOAD_END;
    } else {
        throw std::runtime_error("Unknown Page event type");
    }
}

class FastImageEventEmitRequestHandler : public EventEmitRequestHandler {
    public:
        void handleEvent(EventEmitRequestHandler::Context const &ctx) override {
            if (ctx.eventName != "FastImageView") {
                return;
            }
            ArkJS arkJs(ctx.env);
            auto eventEmitter = ctx.shadowViewRegistry->getEventEmitter<react::FastImageViewEventEmitter>(ctx.tag);
            if (eventEmitter == nullptr) {
                return;
            }

            switch (getFastImageEventType(arkJs,ctx.payload)) {
              case FastImageEventType::FAST_IMAGE_LOAD_START: {
                react::FastImageViewEventEmitter::OnLoadStart event{};
                eventEmitter->onLoadStart(event);
                break;
              }
              case FastImageEventType::FAST_IMAGE_PROGRESS: {
                int loaded = (int)arkJs.getDouble(arkJs.getObjectProperty(ctx.payload, "loaded"));
                int total = (int)arkJs.getDouble(arkJs.getObjectProperty(ctx.payload, "total"));
                
                react::FastImageViewEventEmitter::OnProgress event{loaded, total};
                eventEmitter->onProgress(event);
                break;
              }
              case FastImageEventType::FAST_IMAGE_LOAD: {
                int width = (int)arkJs.getDouble(arkJs.getObjectProperty(ctx.payload, "width"));
                int height = (int)arkJs.getDouble(arkJs.getObjectProperty(ctx.payload, "height"));
                
                react::FastImageViewEventEmitter::OnLoad event{width, height};
                eventEmitter->onLoad(event);
                break;
              }
              case FastImageEventType::FAST_IMAGE_ERROR:{
                react::FastImageViewEventEmitter::OnError event{};
                eventEmitter->onError(event);
                break;
              }
              case FastImageEventType::FAST_IMAGE_LOAD_END:{
                react::FastImageViewEventEmitter::OnLoadEnd event{};
                eventEmitter->onLoadEnd(event);
                break;
              }
              default:
                break;
            }
        };
};
} // namespace rnoh