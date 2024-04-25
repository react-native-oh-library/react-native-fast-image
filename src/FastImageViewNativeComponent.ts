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

import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type { 
    ViewProps, 
    ColorValue,
} from 'react-native';
import type {
  Float,
  WithDefault,
  BubblingEventHandler,
  Int32,
} from 'react-native/Libraries/Types/CodegenTypes';

type Headers = ReadonlyArray<Readonly<{name: string, value: string}>>;
type Priority = WithDefault< 'low' | 'normal' | 'high', 'normal'>
type CacheControl = WithDefault< 'immutable' | 'web' | 'cacheOnly', 'web'>

type FastImageSource = Readonly<{
    uri?: string,
    headers?: Headers,
    priority?: Priority,
    cache?: CacheControl,
}>

type OnLoadEvent = Readonly<{
        width: Float,
        height: Float,
      }>

type OnProgressEvent = Readonly<{
        loaded: Int32,
        total: Int32,
      }>

interface NativeProps extends ViewProps {
  onFastImageError?: BubblingEventHandler<Readonly<{}>>,
  onFastImageLoad?: BubblingEventHandler<OnLoadEvent>,
  onFastImageLoadEnd?: BubblingEventHandler<Readonly<{}>>,
  onFastImageLoadStart?: BubblingEventHandler<Readonly<{}>>,
  onFastImageProgress?: BubblingEventHandler<OnProgressEvent>,
  source?: FastImageSource,
  defaultSource?: string | null,
  resizeMode?:  WithDefault<'contain' | 'cover' | 'stretch' | 'center', 'cover'>,
  tintColor?: ColorValue,
}

export default codegenNativeComponent<NativeProps>('FastImageView');