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

import http from '@ohos.net.http';
import ResponseCode from '@ohos.net.http';
import image from '@ohos.multimedia.image';
import { RNComponentContext, RNViewBase } from '@rnoh/react-native-openharmony'
import { ImageLoaderTurboModule } from '@rnoh/react-native-openharmony/src/main/ets/RNOHCorePackage/turboModules';
import { RemoteImageLoaderError } from '@rnoh/react-native-openharmony/src/main/ets/RemoteImageLoader';
import { FastImageView } from "./RNCSpecs"
import Logger from './Logger'

/**
 * @deprecated Use RNFastImage.NAME instead
 */
export const FAST_IMAGE_TYPE = "FastImageView"

class ImageSourceHolder {
  public source: string | Resource | PixelMap

  constructor(source: string | Resource | PixelMap) {
    this.source = source
  }
}

@Component
export struct RNFastImage {
  public static readonly NAME = FastImageView.NAME
  ctx!: RNComponentContext
  tag: number = 0
  // workaround: `@State` fields cannot be unions of primitive (`string`) and object (`Resource`, `PixelMap`) types
  @State imageSource: ImageSourceHolder | undefined = undefined
  @State private descriptorWrapper: FastImageView.DescriptorWrapper = {} as FastImageView.DescriptorWrapper
  private eventEmitter: FastImageView.EventEmitter | undefined = undefined
  private cleanUpCallbacks: (() => void)[] = []
  defaultColorFilter: number[] = [
    1, 0, 0, 0, 0,
    0, 1, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 0, 1, 0,
  ]

  aboutToAppear() {
    this.eventEmitter = new FastImageView.EventEmitter(this.ctx.rnInstance, this.tag)
    this.onDescriptorWrapperChange(this.ctx.descriptorRegistry.findDescriptorWrapperByTag<FastImageView.DescriptorWrapper>(this.tag)!)
    Logger.info(`RNOH in RNFastImage descriptor: ${JSON.stringify(this.descriptorWrapper.props.source?.uri)}`);
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (_descriptor, newDescriptorWrapper) => {
        this.onDescriptorWrapperChange(newDescriptorWrapper! as FastImageView.DescriptorWrapper)
      }
    ))
  }

  private onDescriptorWrapperChange(descriptorWrapper: FastImageView.DescriptorWrapper) {
    let uriChanged = false;
    if (this.descriptorWrapper.props === undefined) {
      uriChanged = true;
    } else {
      uriChanged = descriptorWrapper.props.source?.uri !== this.descriptorWrapper.props.source?.uri;
    }
    this.descriptorWrapper = descriptorWrapper
    if (uriChanged) {
      this.onLoadStart();
      this.updateImageSource();
    }
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  requestWithHeaders() {
    http.createHttp()
      .request(this.descriptorWrapper.props.source?.uri, { header: this.descriptorWrapper.props.source?.headers },
        (error, data) => {
          if (!error) {
            let code = data.responseCode;
            if (ResponseCode.ResponseCode.OK === code) {
              let res: ArrayBuffer = data.result as ArrayBuffer
              let imageSource = image.createImageSource(res);

              if (this.descriptorWrapper) {
                let wValue = Math.round(this.descriptorWrapper.width);
                let hValue = Math.round(this.descriptorWrapper.height);
                let options: image.InitializationOptions = {
                  alphaType: 0,
                  editable: false,
                  pixelFormat: 3,
                  scaleMode: 1,
                  size: {
                    width: wValue,
                    height: hValue
                  }
                }
                imageSource.createPixelMap(options, (err, pixelMap) => {
                  if (err) {
                    Logger.error(`RNOH in createPixelMap err: `);
                    this.onError();
                  } else {
                    this.imageSource = new ImageSourceHolder(pixelMap)
                  }
                })
              }
            }

          } else {
            Logger.error(`RNOH in RNFastImage http reqeust failed with. Code: ${error.code}, message: ${error.message}`);
            this.onError();
          }
        }
      )
  }

  async updateImageSource() {
    const uri = this.descriptorWrapper.props.source?.uri ?? '';
    if (uri.startsWith("asset://")) {
      this.imageSource = new ImageSourceHolder($rawfile(uri.replace("asset://", this.ctx.rnInstance.getAssetsDest())));
      return;
    }
    if (uri.startsWith("data:")) {
      this.imageSource = new ImageSourceHolder(uri);
      return;
    }
    if (this.descriptorWrapper.props.source?.headers && this.descriptorWrapper.props.source?.headers.length > 0) {
      this.requestWithHeaders()
      return;
    }
    const skipImageLoader = true && !this.ctx.rnInstance.isFeatureFlagEnabled("IMAGE_LOADER");
    const imageLoader = this.ctx.rnInstance.getTurboModule<ImageLoaderTurboModule>("ImageLoader");
    const queryCacheResult = await imageLoader.queryCache([uri]) as Record<string, string>;
    const isNotCached = !queryCacheResult[uri];
    if (skipImageLoader && isNotCached) {
      this.imageSource = new ImageSourceHolder(uri);
      return;
    }

    this.imageSource = undefined;
    imageLoader.getRemoteImageSource(uri).then(async (remoteImage) => {
      try {
        const imageSource = remoteImage.getImageSource();
        const frameCounter = await imageSource.getFrameCount();
        if (frameCounter === 1) {
          // use the downloaded or cached image source
          this.imageSource = new ImageSourceHolder(await imageLoader.getPixelMap(uri));
        } else {
          // an animated GIF
          this.imageSource = new ImageSourceHolder(remoteImage.getLocation())
        }
      } catch (error) {
        this.onError();
        this.onLoadEnd()
      }
    }).catch((error: RemoteImageLoaderError) => {
      this.onError()
      this.onLoadEnd()
    })
  }

  getResizeMode(resizeMode: string) {
    switch (resizeMode) {
      case 'contain':
        return ImageFit.Contain;
      case 'cover':
        return ImageFit.Cover;
      case 'stretch':
        return ImageFit.Fill;
      case 'center':
        return ImageFit.None;
      default:
        return ImageFit.Cover;
    }
  }

  build() {
    RNViewBase({
      ctx: this.ctx,
      tag: this.tag,
      controlsFocus: false
    }) {
      // NOTE: this ensures we switch the image source, even if the new source is bad.
      // ArkUI keeps displaying the last "good" image when the passed source parameter changes
      if (this.imageSource?.source) {
        Image(this.imageSource.source)
          .interpolation(ImageInterpolation.High)
          .draggable(false)
          .alt(this.getDefaultSource())
          .width(this.descriptorWrapper?.width)
          .height(this.descriptorWrapper?.height)
          .position({
            x: 0,
            y: 0,
          })
          .objectFit(this.getResizeMode(this.descriptorWrapper.props.resizeMode))
          .colorFilter(this.descriptorWrapper.props.tintColor)
          .focusable(this.descriptorWrapper?.focusable ?? false)
          .hitTestBehavior(HitTestMode.None)
          .onComplete((event) => {
            if (event) {
              if (event.loadingStatus) {
                this.onLoadEnd();
                this.onProgress(1, 1);
              } else {
                this.onLoad(event.width, event.height);
              }
            }
          })
          .onError((event) => {
            Logger.error(`RNOH in RNFastImageonError 0`);
            this.onError();
          })
          .id(this.descriptorWrapper.tag.toString())
      }
    }
  }

  getDefaultSource(): string | Resource | undefined {
    const defaultSource = this.descriptorWrapper.props.defaultSource;
    if (defaultSource === undefined) {
      return undefined;
    }
    if (defaultSource.startsWith("asset://")) {
      return $rawfile(defaultSource.replace("asset://", this.ctx.rnInstance.getAssetsDest()));
    }
    return defaultSource;
  }

  onLoadStart() {
    this.eventEmitter!.emit("fastImageLoadStart", {})
  }

  onProgress(loaded: number, total: number) {
    this.eventEmitter!.emit("fastImageProgress", { loaded: loaded, total: total })
  }

  onLoad(width: number, height: number) {
    Logger.debug(`RNOH in RNFastImage onLoad,${this.tag}`);
    this.eventEmitter!.emit("fastImageLoad", { width: width, height: height })
  }

  onError() {
    Logger.error(`RNOH in RNFastImage onError,${this.tag}`);
    this.eventEmitter!.emit("fastImageError", {})
    this.onProgress(0, 1);
  }

  onLoadEnd() {
    this.eventEmitter!.emit("fastImageLoadEnd", {})
  }
}