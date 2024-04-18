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

import { TurboModule, TurboModuleContext } from '@rnoh/react-native-openharmony/ts';
import Logger from './Logger'

export type Priority = 'low' | 'normal' | 'high'

type Cache = 'immutable' | 'web' | 'cacheOnly'

type Source = {
  uri?: string
  headers?: { [key: string]: string }
  priority?: Priority
  cache?: Cache
}

export class RNCFastImageViewTurboModule extends TurboModule {
  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    Logger.debug('[RNOH]:RNCFastImageViewTurboModule constructor');
  }

  preload(sources: Source[]) {
    Logger.debug('[RNOH]:RNCFastImageViewTurboModule call preload', JSON.stringify(sources));
  }

  clearMemoryCache(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      Logger.debug('[RNOH]:RNCFastImageViewTurboModule call clearMemoryCache');
      resolve()
    });
  }

  clearDiskCache(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      Logger.debug('[RNOH]:RNCFastImageViewTurboModule call clearDiskCache');
      resolve()
    });
  }
}
