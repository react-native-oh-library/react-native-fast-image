/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

import { TurboModule, RNOHLogger, TurboModuleContext } from '@rnoh/react-native-openharmony/ts';
import { RNCFastImageView } from "./generated/turboModules/RNCFastImageView"

export class RNCFastImageViewTurboModule extends TurboModule implements RNCFastImageView.Spec {
  private logger: RNOHLogger;

  constructor(ctx: TurboModuleContext) {
    super(ctx);
    this.logger = ctx.logger.clone("RNCFastImageView");
  }

  preload(sources: RNCFastImageView.Source[]): void {
    this.logger.warn(`ArkTS preload() has not been implemented yet.`)
    // TODO 调用 imageknife 的 ArkTS preload 接口（若无C++接口）
  }

  clearMemoryCache(): Promise<void> {
    this.logger.warn(`ArkTS clearMemoryCache() has not been implemented yet.`)
    // TODO 调用imageknife 的 ArkTS 清除内存缓存接口（若无C++接口）
    return;
  }

  clearDiskCache(): Promise<void> {
    this.logger.warn(`ArkTS clearDiskCache() has not been implemented yet.`)
    // TODO 调用 imageknife 的 ArkTS 清除磁盘缓存接口（若无C++接口）
    return;
  }
}
