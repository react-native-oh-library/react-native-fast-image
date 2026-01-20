/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

// This file was generated.

export namespace RNCFastImageView {
  export const NAME = 'RNCFastImageView' as const

  export type Source = {uri?: string, headers?: Object, priority?: string, cache?: string}

  export interface Spec {
    clearMemoryCache(): Promise<void>;

    clearDiskCache(): Promise<void>;

  }
}
