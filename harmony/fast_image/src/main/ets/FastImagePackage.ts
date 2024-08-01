
/*
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

import { RNPackage, TurboModulesFactory,
  WorkerTurboModule,
  WorkerTurboModuleContext,
  WorkerTurboModuleFactory } from '@rnoh/react-native-openharmony/ts';
import type {
  TurboModule,
  TurboModuleContext,
  DescriptorWrapperFactoryByDescriptorTypeCtx,
  DescriptorWrapperFactoryByDescriptorType,
} from '@rnoh/react-native-openharmony/ts';
import { RNCFastImageViewTurboModule } from './RNCFastImageViewTurboModule';
import app from '@system.app'
import {RNCFastImageView} from './TMSpecs'
import {FastImageView} from './RNCSpecs'
import {FastImageLoaderTurboModule} from './FastImageLoaderTurboModule'


class FastImageTurboModulesFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule | null {
    if (name === RNCFastImageView.NAME) {
      return new RNCFastImageViewTurboModule(this.ctx);
    } else if(name === FastImageLoaderTurboModule.NAME){
      return new FastImageLoaderTurboModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    if(name === RNCFastImageView.NAME){
      return true;
    }else if(name === FastImageLoaderTurboModule.NAME){
      return true;
    }
    return false;
  }
}


export class FastImagePackage extends RNPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    if (typeof globalThis.IsSetImageRawDataCacheSize === 'undefined') {
      app.setImageCacheCount(200);
      // ImageRawDataCache：缓存解码前的图片数据和PixelMap的数据(datashare thumbnail格式）
      app.setImageRawDataCacheSize(100*1024*1024);
      app.setImageFileCacheSize(100*1024*1024);
      globalThis.IsSetImageRawDataCacheSize = true;
    }
    return new FastImageTurboModulesFactory(ctx);
  }
  createDescriptorWrapperFactoryByDescriptorType(ctx: DescriptorWrapperFactoryByDescriptorTypeCtx): DescriptorWrapperFactoryByDescriptorType {
    return {
      [FastImageView.NAME]: (ctx) => new FastImageView.DescriptorWrapper(ctx.descriptor)
    }
  }

}
