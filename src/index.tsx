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

import { Image, View, StyleSheet } from "react-native";
import type { FastImageProps } from "./FastImageViewNativeComponent";
import { forwardRef, memo } from "react";
import React from "react";
import type { Source } from "./NativeFastImage";
import RNCFastImageView from "./NativeFastImage";
import FastImageView from "./FastImageViewNativeComponent"


function FastImageBase({
  source,
  tintColor,
  defaultSource,
  onLoadStart,
  onProgress,
  onLoad,
  onError,
  onLoadEnd,
  style,
  fallback,
  children,
  // eslint-disable-next-line no-shadow
  resizeMode = 'cover',
  forwardedRef,
  ...props
}: FastImageProps & { forwardedRef: React.Ref<any> }) {
  if (fallback) {
      const cleanedSource = { ...(source as any) }
      delete cleanedSource.cache
      const resolvedSource = Image.resolveAssetSource(cleanedSource)

      return (
          <View style={[styles.imageContainer, style]} ref={forwardedRef}>
              <Image
                  {...props}
                  style={StyleSheet.absoluteFill}
                  source={resolvedSource}
                  onLoadStart={onLoadStart as any}
                  onProgress={onProgress}
                  onLoad={onLoad as any}
                  onError={onError}
                  onLoadEnd={onLoadEnd as any}
                  resizeMode={resizeMode as any}
              />
              {children}
          </View>
      )
  }

  const resolvedSource = Image.resolveAssetSource(source as any)
  const defaultResolveSource = Image.resolveAssetSource(defaultSource as any)
  const defaultResolveSourceAsString = defaultResolveSource !== null ? String(defaultResolveSource) : null;

  return (
      <View style={[styles.imageContainer, style]} ref={forwardedRef}>
          <FastImageView
              {...props}
              tintColor={tintColor}
              defaultSource={defaultResolveSourceAsString}
              style={StyleSheet.absoluteFill}
              source={resolvedSource}
              onLoadStart={onLoadStart}
              onProgress={onProgress}
              onLoad={onLoad}
              onError={onError}
              onLoadEnd={onLoadEnd}
              resizeMode={resizeMode}
          />
          {children}
      </View>
  )
}

const FastImageMemo = memo(FastImageBase)

const FastImageComponent: React.ComponentType<FastImageProps> = forwardRef(
  (props: FastImageProps, ref: React.Ref<any>) => (
      <FastImageMemo forwardedRef={ref} {...props} />
  ),
)

FastImageComponent.displayName = 'FastImage'

const resizeMode = {
  contain: 'contain',
  cover: 'cover',
  stretch: 'stretch',
  center: 'center',
} as const


const priority = {
  low: 'low',
  normal: 'normal',
  high: 'high',
} as const


const cacheControl = {
  // Ignore headers, use uri as cache key, fetch only if not in cache.
  immutable: 'immutable',
  // Respect http headers, no aggressive caching.
  web: 'web',
  // Only load from cache.
  cacheOnly: 'cacheOnly',
} as const


export interface FastImageStaticProperties {
  resizeMode: typeof resizeMode
  priority: typeof priority
  cacheControl: typeof cacheControl
  preload: (sources: Source[]) => void
  clearMemoryCache: () => Promise<void>
  clearDiskCache: () => Promise<void>
}

const FastImage: React.ComponentType<FastImageProps> &
  FastImageStaticProperties = FastImageComponent as any

FastImage.resizeMode = resizeMode

FastImage.cacheControl = cacheControl

FastImage.priority = priority

FastImage.preload = (sources: Source[]) => RNCFastImageView.preload(sources)

FastImage.clearMemoryCache = () => RNCFastImageView.clearMemoryCache()

FastImage.clearDiskCache = () => RNCFastImageView.clearDiskCache()

const styles = StyleSheet.create({
  imageContainer: {
      overflow: 'hidden',
  },
})

export default FastImage
