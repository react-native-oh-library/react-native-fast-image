/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#include "RNCFastImageViewTurboModule.h"
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
    // TODO 调用 imageknife 的 C++ preload接口
}

static jsi::Value hostFunction_RNCFastImageViewTurboModule_clearMemoryCache(jsi::Runtime &rt,
                                                                            react::TurboModule &turboModule,
                                                                            const jsi::Value *args, size_t count) {
    auto self = static_cast<RNCFastImageViewTurboModule *>(&turboModule);
    self->clearMemoryCache(rt);
    return self->callAsync(rt, "clearMemoryCache", args, count);
}

void RNCFastImageViewTurboModule::clearMemoryCache(facebook::jsi::Runtime &rt) {
    // TODO: 调用 imageknife 的 C++清除内存缓存接口
}

static jsi::Value hostFunction_RNCFastImageViewTurboModule_clearDiskCache(jsi::Runtime &rt,
                                                                          react::TurboModule &turboModule,
                                                                          const jsi::Value *args, size_t count) {
    auto self = static_cast<RNCFastImageViewTurboModule *>(&turboModule);
    self->clearDiskCache(rt);
    return self->callAsync(rt, "clearDiskCache", args, count);
}
void RNCFastImageViewTurboModule::clearDiskCache(facebook::jsi::Runtime &rt) {
    // TODO: 调用 imageknife 的 C++清除磁盘缓存接口
}

RNCFastImageViewTurboModule::RNCFastImageViewTurboModule(const ArkTSTurboModule::Context ctx, const std::string name)
    : ArkTSTurboModule(ctx, name) {
    methodMap_["preload"] = MethodMetadata{0, hostFunction_RNCFastImageViewTurboModule_preload};
    methodMap_["clearMemoryCache"] = MethodMetadata{0, hostFunction_RNCFastImageViewTurboModule_clearMemoryCache};
    methodMap_["clearDiskCache"] = MethodMetadata{0, hostFunction_RNCFastImageViewTurboModule_clearDiskCache};
}