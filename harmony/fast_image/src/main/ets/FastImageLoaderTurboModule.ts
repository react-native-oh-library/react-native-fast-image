/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

import {
  RemoteImageDiskCache,
  RemoteImageLoader,
  RemoteImageLoaderError,
  RemoteImageMemoryCache
} from './FastRemoteImageLoader';
import image from '@ohos.multimedia.image';
import { RemoteImageSource } from './FastRemoteImageLoader/RemoteImageSource';
import { TurboModule, RNOHLogger, TurboModuleContext } from '@rnoh/react-native-openharmony/ts';

export class FastImageLoaderTurboModule extends TurboModule {
  static NAME = "FastImageLoader" as const
  private logger: RNOHLogger;
  private imageLoader: RemoteImageLoader

  constructor(protected ctx: TurboModuleContext) {
    super(ctx)
    this.logger = ctx.logger.clone("RNCFastImageLoader");
    this.imageLoader = new RemoteImageLoader(
      new RemoteImageMemoryCache(128), new RemoteImageDiskCache(128, ctx.uiAbilityContext.cacheDir), ctx.uiAbilityContext,
      ({ remoteUri, fileUri }) => {
        ctx.rnInstance.postMessageToCpp('UPDATE_FAST_IMAGE_SOURCE_MAP', { remoteUri, fileUri });
      },
      ({ remoteUri }) => {
        ctx.rnInstance.postMessageToCpp('FAST_IMAGE_DOWNLOAD_FILE_FAIL', { remoteUri });
      },
    )
  }

  public getConstants() {
    return {}
  }

  public async getSize(uri: string): Promise<number[]> {
    if (uri && uri.length > 0) {
      const imageSource = await this.imageLoader.getImageSource(uri)
      const imageInfo = await imageSource.getImageSource().getImageInfo()
      return [imageInfo.size.width, imageInfo.size.height]
    } else {
      return [0, 0]
    }
  }

  public async getSizeWithHeaders(uri: string, headers: Object): Promise<{
    width: number,
    height: number
  } & Record<string, any>> {
    let srcHeaders = headers as Record<string, any>
    let destHeaders: Record<string, any> = {}

    if (srcHeaders['crossOrigin'] === 'use-credentials') {
      destHeaders['Access-Control-Allow-Credentials'] = 'true'
    }

    if (srcHeaders['referrerPolicy'] !== undefined) {
      destHeaders['Referrer-Policy'] = srcHeaders['referrerPolicy']
    }

    const imageSource = await this.imageLoader.getImageSource(uri, destHeaders)
    const imageInfo = await imageSource.getImageSource().getImageInfo()
    return Promise.resolve({ width: imageInfo.size.width, height: imageInfo.size.height })
  }

  public async prefetchImage(uri: string, headers?: object): Promise<boolean> {
    return this.imageLoader.prefetch(uri, headers);
  }

  public prefetchImageWithMetadata(uri: string, queryRootName: string, rootTag: number): Promise<boolean> {
    this.logger.warn("ImageLoader::prefetchImageWithMetadata is not supported")
    return Promise.resolve(false)
  }

  public getPrefetchResult(uri: string): string | undefined {
    return this.imageLoader.getPrefetchResult(uri);
  }

  public queryCache(uris: Array<string>): Promise<Object> {
    const cachedUriEntries = uris.map(uri =>
      [uri, this.imageLoader.queryCache(uri)]
    ).filter(([_uri, value]) => value !== undefined);
    const cachedUriMap = Object.fromEntries(cachedUriEntries)
    return Promise.resolve(cachedUriMap)
  }

  public async getRemoteImageSource(uri: string): Promise<RemoteImageSource> {
    try {
      const imageSource = await this.imageLoader.getImageSource(uri);
      return imageSource;
    }
    catch (e) {
      if (!(e instanceof RemoteImageLoaderError) && e instanceof Object && e.message) {
        throw new RemoteImageLoaderError(`Failed to load the image: ${e.message}`);
      }
      if (typeof e === 'string') {
        throw new RemoteImageLoaderError(e);
      }
      throw e;
    }
  }

  public async getImageSource(uri: string): Promise<image.ImageSource> {
    return (await this.getRemoteImageSource(uri)).getImageSource()
  }

  public async getPixelMap(uri: string): Promise<image.PixelMap> {
    try {
      return await this.imageLoader.getPixelMap(uri);
    }
    catch (e) {
      if (!(e instanceof RemoteImageLoaderError) && e instanceof Object && e.message) {
        throw new RemoteImageLoaderError(`Failed to load the image: ${e.message}`);
      }
      if (typeof e === 'string') {
        throw new RemoteImageLoaderError(e);
      }
      throw e;
    }
  }

  public getCacheFilePath(uri: string): string {
    return this.imageLoader.getCacheFilePath(uri);
  }

  public diskCacheClear(): Promise<void> {
    return this.imageLoader.diskCacheClear()
  }

  public memoryCacheClear(): Promise<void> {
    return this.imageLoader.memoryCacheClear()
  }

}