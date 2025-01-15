/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <ReactCommon/TurboModule.h>
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/RNInstance.h"

namespace rnoh {

class JSI_EXPORT RNCFastImageViewTurboModule : public ArkTSTurboModule {
public:
    RNCFastImageViewTurboModule(const ArkTSTurboModule::Context ctx, const std::string name);

    void preload(facebook::jsi::Runtime &rt,const facebook::jsi::Value *args, size_t count);
    void clearMemoryCache(facebook::jsi::Runtime &rt);
    void clearDiskCache(facebook::jsi::Runtime &rt);
};

} // namespace rnoh