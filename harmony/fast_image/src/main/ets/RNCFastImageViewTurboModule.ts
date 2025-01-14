/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

import { TurboModule, RNOHLogger, TurboModuleContext } from '@rnoh/react-native-openharmony/ts';
import { FastImageLoaderTurboModule } from './FastImageLoaderTurboModule';
import { RNCFastImageView } from "./generated/turboModules/RNCFastImageView"

export class RNCFastImageViewTurboModule extends TurboModule implements RNCFastImageView.Spec {
  private logger: RNOHLogger;

  constructor(ctx: TurboModuleContext) {
    super(ctx);
    this.logger = ctx.logger.clone("RNCFastImageView");
  }

  preload(sources: RNCFastImageView.Source[]): void {
    // NO-OP Implemented in C++
  }

  clearMemoryCache(): Promise<void> {
    let imageLoaderTurboModule: FastImageLoaderTurboModule = this.ctx.rnInstance.getTurboModule("FastImageLoader");
    return imageLoaderTurboModule.memoryCacheClear();
  }

  clearDiskCache(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      let imageLoaderTurboModule: FastImageLoaderTurboModule = this.ctx.rnInstance.getTurboModule("FastImageLoader");
      try {
        imageLoaderTurboModule.diskCacheClear();
      } catch (e) {
        this.logger.error("clearDiskCache BusinessError code:" + e.code)
        reject("clearDiskCache BusinessError code:" + e.code)
      }
      resolve(null)
    });
  }
}
