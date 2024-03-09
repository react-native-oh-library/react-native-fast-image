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
    ColorSegments
} from 'rnoh'
import {
    ImageKnifeData,
    RequestOption,
    Size,
    ImageKnife,
    ImageKnifeGlobal,
    ImageKnifeComponent,
    ObjectKey
} from '@ohos/imageknife'
import http from '@ohos.net.http';
import ResponseCode from '@ohos.net.http';
import image from '@ohos.multimedia.image';
import { ImageLoaderTurboModule } from 'rnoh/src/main/ets/RNOHCorePackage/turboModules';
import Logger from './Logger'
import { FastImageModifier } from './FastImageModifier'
import { BusinessError } from '@ohos.base'

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

export class FastImageDescriptorWrapper extends ViewDescriptorWrapperBase<"FastImageView", FastImageProps, FastImageState, FastImageRawProps> {
}

@Component
export struct RNFastImage {
    ctx!: RNOHContext
    tag: number = 0
    @State descriptor: FastImageViewDescriptor = {} as FastImageViewDescriptor
    @State descriptorWrapper: FastImageDescriptorWrapper | undefined = undefined
    @State pixelMap: PixelMap | undefined = undefined
    @State rawfilePath: Resource = $rawfile(undefined)
    private unregisterDescriptorChangesListener?: () => void = undefined
    public componentManager!: RNViewManager;
    private unregisterComponentManager?: () => void = undefined
    private unregisterCommandCallback?: () => void = undefined
    private timeId?: number = -1

    aboutToAppear() {
        this.initComponent()
        this.descriptor = this.ctx.descriptorRegistry.getDescriptor<FastImageViewDescriptor>(this.tag)
        this.updatePixelMap()
        this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
            (newDescriptor) => {
                this.descriptor = (newDescriptor as FastImageViewDescriptor)
                this.setDescriptor(this.descriptor)
                this.updatePixelMap()
                Logger.debug(`RNOH in RNFastImage newDescriptor props tag,${this.tag} ${JSON.stringify(this.descriptor.props)}`);
            }
        )
        Logger.debug(`RNOH in RNFastImage props tag,${this.tag} ${JSON.stringify(this.descriptor.props)}`);
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

    setDescriptor(descriptor: ViewBaseDescriptor) {
        this.descriptorWrapper = new FastImageDescriptorWrapper(descriptor)
    }

    updatePixelMap(){
        let source = this.getSource(this.descriptor.props.source)
        if (source instanceof String) {
            this.load(source)
        }else {
            this.rawfilePath = source as Resource
        }
    }

    load(src: string | image.PixelMap | Resource) {
        clearTimeout(this.timeId)
        let request = new RequestOption()
        //*requestOption调用*
        if (this.descriptor.props.headers.length > 0) {
            let header:Record<string,string> = JSON.parse(this.descriptor.props.headers)
            const keys = Object.keys(header);
            keys.forEach(key => {
                request.addHeader(key, header[key]);
            });
        }
        //通过时间戳去清除缓存
        request.signature = new ObjectKey(new Date().getTime().toString())
        request.load(src)
            .addListener({
                callback: (err: BusinessError | string, data: ImageKnifeData) => {
                    if (data.isPixelMap()) {
                        if (data.drawPixelMap) {
                            let pixelmap = data.drawPixelMap.imagePixelMap
                            if (pixelmap) {
                                this.pixelMap = pixelmap
                            }
                        }
                    }
                    if (data.isGIFFrame()) {
                        let index: number = 0
                        if (data.drawGIFFrame) {
                            if (data.drawGIFFrame.imageGIFFrames) {
                                let renderGif = () => {
                                    if (data.drawGIFFrame) {
                                        if (data.drawGIFFrame.imageGIFFrames) {
                                            let pixelmap = data.drawGIFFrame.imageGIFFrames[index].drawPixelMap
                                            let delay = data.drawGIFFrame.imageGIFFrames[index].delay
                                            if (pixelmap) {
                                                this.pixelMap = pixelmap
                                            }
                                            index++;
                                            if (index == data.drawGIFFrame.imageGIFFrames.length - 1) {
                                                index = 0
                                            }
                                            this.timeId = setTimeout(renderGif, data!.drawGIFFrame!.imageGIFFrames![index].delay)
                                        }
                                    }
                                }
                                renderGif()
                            }
                        }
                    }
                    if (err) {
                        Logger.error("fastimage error:" + JSON.stringify(err));
                    }
                    return false
                }
            })
        let compSize: Size = {
            width: 300,
            height: 300
        }
        request.setImageViewSize(compSize)
        let imageknife: ImageKnife | undefined = ImageKnifeGlobal.getInstance().getImageKnife()
        if (imageknife != undefined) {
            imageknife.call(request)
        }
    }

    build() {
        Image(this.rawfilePath || this.pixelMap)
            .attributeModifier(FastImageModifier.getInstance()
                .setDescriptor(this.descriptor, this.descriptorWrapper as FastImageDescriptorWrapper))
            .interpolation(ImageInterpolation.High)
            .draggable(false)
            .alt(this.getSource(this.descriptor.props.defaultSource))
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