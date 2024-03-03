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

import {
  Descriptor,
  ViewDescriptorWrapperBase,
  ViewBaseDescriptor,
  ViewRawProps,
  RNViewManager,
  RNOHContext,
  ViewBaseProps,
  ColorSegments,
} from 'rnoh'

import http from '@ohos.net.http';
import ResponseCode from '@ohos.net.http';
import image from '@ohos.multimedia.image';
import { ImageLoaderTurboModule } from 'rnoh/src/main/ets/RNOHCorePackage/turboModules';
import Logger from './Logger'
import {FastImageModifier} from './FastImageModifier'
import { RemoteImageLoaderError } from 'rnoh/src/main/ets/RemoteImageLoader/RemoteImageLoaderError';
export const FAST_IMAGE_TYPE = "FastImageView"

export interface FastImageProps extends ViewBaseProps {
  source: string
  resizeMode: number
  headers: string
  priority: string
  cache: string
  tintColor: ColorSegments
  defaultSource: string
}

interface FastImageState {}
interface FastImageRawProps extends ViewRawProps {
  alt: string
}
export type FastImageViewDescriptor = Descriptor<"FastImageView", FastImageProps, FastImageState, FastImageRawProps>

export class FastImageDescriptorWrapper extends ViewDescriptorWrapperBase<"FastImageView", FastImageProps, FastImageState, FastImageRawProps>{
}

class ImageSourceHolder {
  public source: string | Resource | PixelMap

  constructor(source: string | Resource | PixelMap) {
    this.source = source
  }
}

@Component
export struct RNFastImage {
  ctx!: RNOHContext
  tag: number = 0
  @State descriptor: FastImageViewDescriptor = {} as FastImageViewDescriptor
  private unregisterDescriptorChangesListener?: () => void = undefined
  public componentManager!:RNViewManager;
  @State descriptorWrapper :FastImageDescriptorWrapper | undefined = undefined
  private unregisterComponentManager?: () => void = undefined
  private unregisterCommandCallback?: () => void = undefined

  @State pixelMap: PixelMap | undefined = undefined
  @State imageSource: ImageSourceHolder | undefined = undefined
  @State defaultImageSource: ImageSourceHolder | undefined = undefined

  requestWithHeaders() {
    http.createHttp().request(this.descriptor.props.source, { header:JSON.parse(this.descriptor.props.headers) },
      (error, data) => {
        if (!error) {
          let code = data.responseCode;
          if (ResponseCode.ResponseCode.OK === code) {
            let res: ArrayBuffer = data.result as ArrayBuffer
            let imageSource = image.createImageSource(res);
            imageSource.getImageInfo((err, value) => {
              if (err) {
                Logger.error("RNOH in getImageInfo err:")
                this.onError();
                return;
              }
              let hValue = Math.round(value.size.height );
              let wValue = Math.round(value.size.width);

              let options: image.InitializationOptions = {
                alphaType: 0,
                editable: false,
                pixelFormat: 3,
                scaleMode: 1,
                size: {width: wValue, height: hValue}
              }
              imageSource.createPixelMap(options, (err, pixelMap) => {
                if (err) {
                  Logger.error(`RNOH in createPixelMap err: `);
                  this.onError();
                } else {
                  this.pixelMap = pixelMap
                }
              })
            })
          }
        } else {
          Logger.error(`RNOH in RNFastImage http reqeust failed with. Code: ${error.code}, message: ${error.message}`);
          this.onError();
        }
      }
    )
  }

  aboutToAppear() {
    this.initComponent()
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<FastImageViewDescriptor>(this.tag)
    this.updateDefaultImageSource();
    this.updateImageSource();
    Logger.info(`this.defaultImageSource${this.defaultImageSource?.source.toString()} }`);

    this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.descriptor = (newDescriptor as FastImageViewDescriptor)
        this.setDescriptor(this.descriptor)
        this.updateDefaultImageSource();
        this.updateImageSource();
        Logger.debug(`RNOH in RNFastImage newDescriptor props tag,${this.tag} ${JSON.stringify(this.descriptor.props)}`);
      }
    )
    Logger.debug(`RNOH in RNFastImage props tag,${this.tag} ${JSON.stringify(this.descriptor.props)}`);
    if (this.descriptor.props.headers.length > 0) {
      this.requestWithHeaders()
    }
    this.onLoadStart();
  }

  aboutToDisappear() {
    this.unregisterDescriptorChangesListener?.()
    this.unregisterComponentManager?.()
    this.unregisterCommandCallback?.()
  }

  initComponent() {
    let descriptor = this.ctx.descriptorRegistry.getDescriptor<ViewBaseDescriptor>(this.tag)
    if (!this.componentManager) {
      this.componentManager = new RNViewManager(this.tag, this.ctx)
    }
    this.unregisterComponentManager = this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager)
    this.setDescriptor(descriptor)
    const parentTag = descriptor.parentTag
    this.componentManager.setParentTag(parentTag)
    this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.setDescriptor(newDescriptor)
      }
    )
  }

  updateImageSource() {
    const uri = this.descriptor.props.source;
    if (uri.startsWith("asset://")) {
      this.imageSource = new ImageSourceHolder($rawfile(uri.replace("asset://", "assets/")));
      return;
    }
    if (uri.startsWith("data:")) {
      this.imageSource = new ImageSourceHolder(uri);
      return;
    }
    const imageLoader = this.ctx.rnInstance.getTurboModule<ImageLoaderTurboModule>("ImageLoader");
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
        this.dispatchOnError(error.message);
      }
    }).catch((error: RemoteImageLoaderError) => {
      this.dispatchOnError(error.message)
    })
  }

  updateDefaultImageSource() {
    const uri = this.descriptor.props.defaultSource;
    if (uri.startsWith("asset://")) {
      this.defaultImageSource = new ImageSourceHolder($rawfile(uri.replace("asset://", "assets/")));
      return;
    }
    if (uri.startsWith("data:")) {
      this.defaultImageSource = new ImageSourceHolder(uri);
      return;
    }
    const imageLoader = this.ctx.rnInstance.getTurboModule<ImageLoaderTurboModule>("ImageLoader");
    this.defaultImageSource = undefined;
    imageLoader.getRemoteImageSource(uri).then(async (remoteImage) => {
      try {
        const imageSource = remoteImage.getImageSource();
        const frameCounter = await imageSource.getFrameCount();
        if (frameCounter === 1) {
          // use the downloaded or cached image source
          this.defaultImageSource = new ImageSourceHolder(await imageLoader.getPixelMap(uri));
        } else {
          // an animated GIF
          this.defaultImageSource = new ImageSourceHolder(remoteImage.getLocation())
        }
      } catch (error) {
        this.dispatchOnError(error.message);
      }
    }).catch((error: RemoteImageLoaderError) => {
      this.dispatchOnError(error.message)
    })
  }

  dispatchOnError(message?: string) {
    const errorMessage = message ? message : "Failed to load the image";
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, "error", errorMessage)
  }

  setDescriptor(descriptor:ViewBaseDescriptor) {
    this.descriptorWrapper = new FastImageDescriptorWrapper(descriptor)
  }

  build() {
    Image(this.descriptor.props.headers.length > 0 ? this.pixelMap : this.imageSource?.source)
      .attributeModifier(FastImageModifier.getInstance().setDescriptor(this.descriptor, this.descriptorWrapper as FastImageDescriptorWrapper))
      .interpolation(ImageInterpolation.High)
      .draggable(false)
      .alt(this.defaultImageSource?.source as string | Resource | undefined)
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
        if (this.descriptor.props.headers.length == 0) {
          this.onError();
        }
      })
      .id(this.tag.toString())
  }

  onLoadStart() {
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, FAST_IMAGE_TYPE, { type: "onLoadStart" });
  }

  onProgress(loaded: number, total: number) {
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, FAST_IMAGE_TYPE, {
      type: "onProgress",
      loaded: loaded,
      total: total,
    })
  }

  onLoad(width: number, height: number) {
    Logger.debug(`RNOH in RNFastImage onLoad,${this.tag}`);
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, FAST_IMAGE_TYPE, {
      type: "onLoad",
      width: width,
      height: height,
    })
  }

  onError() {
    Logger.error(`RNOH in RNFastImage onError,${this.tag}`);
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, FAST_IMAGE_TYPE, { type: "onError" })
    this.onProgress(0, 1);
  }

  onLoadEnd() {
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, FAST_IMAGE_TYPE, { type: "onLoadEnd" })
  }
}