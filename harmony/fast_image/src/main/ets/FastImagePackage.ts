/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

import { RNPackage, TurboModulesFactory } from '@rnoh/react-native-openharmony/ts';
import type {
  TurboModule,
  TurboModuleContext,
} from '@rnoh/react-native-openharmony/ts';
import { RNCFastImageViewTurboModule } from './RNCFastImageViewTurboModule';
// import app from '@system.app'
import { RNCFastImageView } from './generated/turboModules/RNCFastImageView';


class FastImageTurboModulesFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule | null {
    if (name === RNCFastImageView.NAME) {
      return new RNCFastImageViewTurboModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    if (name === RNCFastImageView.NAME) {
      return true;
    }
    return false;
  }
}

export class FastImagePackage extends RNPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    /* TODO 初始化缓存参数参数，如：
      if (typeof globalThis.IsSetImageRawDataCacheSize === 'undefined') {
        app.setImageCacheCount(200);
        // ImageRawDataCache：缓存解码前的图片数据和PixelMap的数据(datashare thumbnail格式）
        app.setImageRawDataCacheSize(100 * 1024 * 1024);
        app.setImageFileCacheSize(100 * 1024 * 1024);
        globalThis.IsSetImageRawDataCacheSize = true;
      }
     */
    return new FastImageTurboModulesFactory(ctx);
  }
}
