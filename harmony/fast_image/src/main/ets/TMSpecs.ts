// This file was generated.

export namespace RNCFastImageView {
  export const NAME = 'RNCFastImageView' as const

  export type Source = {uri?: string, headers?: Object, priority?: string, cache?: string}

  export interface Spec {
    clearMemoryCache(): Promise<void>;

    clearDiskCache(): Promise<void>;

  }
}
