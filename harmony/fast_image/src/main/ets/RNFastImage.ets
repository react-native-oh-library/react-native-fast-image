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

import { Descriptor, ComponentBuilderContext, RNOHContext, ViewBaseProps, ColorSegments } from 'rnoh'
import http from '@ohos.net.http';
import ResponseCode from '@ohos.net.http';
import image from '@ohos.multimedia.image';
import { ImageLoaderTurboModule } from 'rnoh/src/main/ets/RNOHCorePackage/turboModules';
import Logger from './Logger'

export const FAST_IMAGE_TYPE = "FastImageView"

export interface FastImageProps extends ViewBaseProps {
  source: string
  isWebpGif: boolean
  resizeWidth: number // 图片缩放宽
  imgStyleHeight: number // 图片缩放高
  resizeMode: number
  headers: string
  priority: string
  cache: string
  tintColor: ColorSegments
  defaultSource: string
}

export type FastImageViewDescriptor = Descriptor<"FastImageView", FastImageProps>

@Component
export struct RNFastImage {
  ctx!: RNOHContext
  tag: number = 0
  @BuilderParam buildCustomComponent: (componentBuilderContext: ComponentBuilderContext) => void
  @State descriptor: FastImageViewDescriptor = {} as FastImageViewDescriptor
  private unregisterDescriptorChangesListener?: () => void = undefined
  @State redirectUri: string | undefined = undefined
  @State redirect: boolean = false
  @State pixelMap: PixelMap | undefined = undefined

  /**
   * 处理重定向图片
   */
  request() {
    let httpRequest = http.createHttp();
    httpRequest.request(this.descriptor.props.source, {
      header: { 'Content-Type': 'application/json' },
    }, (err, data) => {
      if (!err) {
        let header: string = JSON.stringify(data?.header);
        JSON.parse(header, (key: string, value: string) => {
          if (key == "location") {
            this.redirectUri = value;
          }
        })
        if (this.redirectUri && this.redirectUri != this.descriptor.props.source) {
          this.redirect = true
        }
      }
    });
  }

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
                } else {
                  this.pixelMap = pixelMap
                }
              })
            })
          }
        } else {
          Logger.error(`RNOH in RNFastImage http reqeust failed with. Code: ${error.code}, message: ${error.message}`);
        }
      }
    )
  }

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<FastImageViewDescriptor>(this.tag)
    this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.descriptor = (newDescriptor as FastImageViewDescriptor)
        if (this.redirectUri) {
          this.redirect = false
          this.redirectUri = undefined
        }
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
  }

  getColorArr(color: ColorSegments) {
    if (color == undefined) {
      return [
        1, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
      ]
    }
    return [
      0, 0, 0, color[0], 0,
      0, 0, 0, color[1], 0,
      0, 0, 0, color[2], 0,
      0, 0, 0, color[3], 0,
    ]
  }

  build() {
    Image(this.descriptor.props.headers.length > 0 ? this.pixelMap : (this.redirect ? this.redirectUri : this.getSource(this.descriptor.props.source)))
      .interpolation(ImageInterpolation.High)
      .width("100%")
      .height("100%")
      .sourceSize(this.descriptor.props.resizeMode === 2 ? {
        width: vp2px(this.descriptor.props.resizeWidth),
        height: vp2px(this.descriptor.props.imgStyleHeight)
      } : { width: 0, height: 0 })
      .alt(this.getSource(this.descriptor.props.defaultSource))
      .onComplete((event) => {
        if (event) {
          this.onLoad(event.width, event.height);
          if (event.loadingStatus) {
            this.onLoadEnd();
            this.onProgress(1, 1);
          }
        }
      })
      .onError((event) => {
        this.onError();
        this.onProgress(0, 1);
      })
      .objectFit(this.transResizeModeForImage(this.descriptor.props.resizeMode))
      .colorFilter(this.getColorArr(this.descriptor.props.tintColor))
  }

  transResizeModeForImage(resizeMode: number) {
    if (resizeMode === 0) {
      return ImageFit.Cover;
    } else if (resizeMode === 1) {
      return ImageFit.Contain;
    } else if (resizeMode === 2) {
      return ImageFit.Fill;
    } else if (resizeMode === 3) {
      return ImageFit.None;
    }
    return ImageFit.Cover;
  }

  getSource(source: string) {
    if (source.startsWith("asset://")) {
      return $rawfile(source.replace("asset://", "assets/"));
    }
    if (source.startsWith("file://assets/src/assets/")) {
      return $rawfile(source.replace("file://assets/src/assets/", "assets/"));
    }
    const filePath = this.ctx.rnInstance.getTurboModule<ImageLoaderTurboModule>("ImageLoader").getCachedImage(source)
    return filePath ?? source
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
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, FAST_IMAGE_TYPE, {
      type: "onLoad",
      width: width,
      height: height,
    })
  }

  onError() {
    if (!this.descriptor.props.source.startsWith("file://") && !this.descriptor.props.source.startsWith("asset://")) {
      this.request()
    }
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, FAST_IMAGE_TYPE, { type: "onError" })
  }

  onLoadEnd() {
    this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, FAST_IMAGE_TYPE, { type: "onLoadEnd" })
  }
}