/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#include "RNCFastImageViewTurboModule.h"
#include "FastImageLoaderTurboModule.h"
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/RNInstance.h"

using namespace rnoh;
using namespace facebook;

static jsi::Value hostFunction_RNCFastImageViewTurboModule_preload(jsi::Runtime &rt, react::TurboModule &turboModule,
                                                                   const jsi::Value *args, size_t count) {
    auto self = static_cast<RNCFastImageViewTurboModule *>(&turboModule);
    self->preload(rt, args, count);
    return jsi::Value::null();
}
void RNCFastImageViewTurboModule::preload(facebook::jsi::Runtime &rt, const facebook::jsi::Value *args, size_t count) {
    auto rnInstance = m_ctx.instance.lock();
    auto turboModule = rnInstance->getTurboModule("FastImageLoader");
    auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
    facebook::jsi::Array sources = args[0].asObject(rt).asArray(rt);

    for (int i = 0; i < sources.size(rt); i++) {
        auto value = sources.getValueAtIndex(rt, i);
        if (!value.isObject())
            continue;
        auto source = value.getObject(rt);
        if (source.hasProperty(rt, "uri")) {
            if (source.hasProperty(rt, "headers")) {
                facebook::jsi::Value args[2] = {source.getProperty(rt, "uri"), source.getProperty(rt, "headers")};
                arkTsTurboModule->callAsync(rt, "prefetchImage", args, 2);
            } else {
                facebook::jsi::Value args[1] = {source.getProperty(rt, "uri")};
                arkTsTurboModule->callAsync(rt, "prefetchImage", args, 1);
            }
        }
    }
    return;
}

static jsi::Value hostFunction_RNCFastImageViewTurboModule_clearMemoryCache(jsi::Runtime &rt,
                                                                            react::TurboModule &turboModule,
                                                                            const jsi::Value *args, size_t count) {
    return static_cast<ArkTSTurboModule &>(turboModule).callAsync(rt, "clearMemoryCache", args, count);
}

static jsi::Value hostFunction_RNCFastImageViewTurboModule_clearDiskCache(jsi::Runtime &rt,
                                                                          react::TurboModule &turboModule,
                                                                          const jsi::Value *args, size_t count) {
    auto self = static_cast<RNCFastImageViewTurboModule *>(&turboModule);
    self->clearDiskCache(rt);
    return self->callAsync(rt, "clearDiskCache", args, count);
}
void RNCFastImageViewTurboModule::clearDiskCache(facebook::jsi::Runtime &rt) {
    auto rnInstance = m_ctx.instance.lock();
    if (!rnInstance) {
        return;
    }

    auto turboModule = rnInstance->getTurboModule("FastImageLoader");
    if (!turboModule) {
        return;
    }

    auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::FastImageLoaderTurboModule>(turboModule);
    if (!arkTsTurboModule) {
        return;
    }

    if (!arkTsTurboModule->m_FastImageSourceResolver) {
        return;
    } else {
        arkTsTurboModule->m_FastImageSourceResolver->clearRemoteImageSourceMap();
        return;
    }
}

RNCFastImageViewTurboModule::RNCFastImageViewTurboModule(const ArkTSTurboModule::Context ctx, const std::string name)
    : ArkTSTurboModule(ctx, name) {
    methodMap_["preload"] = MethodMetadata{0, hostFunction_RNCFastImageViewTurboModule_preload};
    methodMap_["clearMemoryCache"] = MethodMetadata{0, hostFunction_RNCFastImageViewTurboModule_clearMemoryCache};
    methodMap_["clearDiskCache"] = MethodMetadata{0, hostFunction_RNCFastImageViewTurboModule_clearDiskCache};
}