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

import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";
import type { 
    ViewProps,
    HostComponent, 
    ColorValue,
} from "react-native";
import type { 
    WithDefault, 
    DirectEventHandler, 
    Float, 
    Int32} from "react-native/Libraries/Types/CodegenTypes";

export type ResizeMode = 'contain' | 'cover' | 'stretch' | 'center';

export type OnProgressEventData = Readonly<{
    loaded: Int32,
    total: Int32
}>;

export type OnLoadEventData = Readonly<{
    width: Int32,
    height: Int32
}>;

export type OnLoadStartEventData = Readonly<{}>;
export type OnLoadEndEventData = Readonly<{}>;
export type OnErrorEventData = Readonly<{}>;
type Priority = 'low' | 'normal' | 'high'

type Cache = 'immutable' | 'web' | 'cacheOnly';

type Source = {
  uri?: string;
  headers?: { [key: string]: string };
  priority?: WithDefault<Priority, 'normal'>;
  cache?: WithDefault<Cache, 'web'>;
}

type DefaultSource = {
    uri?: string;
}

export interface FastImageProps extends ViewProps {
    source?: Source
    resizeMode?: WithDefault<ResizeMode, 'cover'>
    fallback?: boolean
    isWebpGif?: boolean
    resizeWidth?: Float
    imgStyleHeight?: Float
    onLoadStart?: DirectEventHandler<OnLoadStartEventData>;

    onProgress?: DirectEventHandler<OnProgressEventData>;

    onLoad?: DirectEventHandler<OnLoadEventData>;

    onError?: DirectEventHandler<OnErrorEventData>;

    onLoadEnd?: DirectEventHandler<OnLoadEndEventData>;

    tintColor?: ColorValue
    defaultSource?: DefaultSource
}

export default codegenNativeComponent<FastImageProps>(
    'FastImageView'
) as HostComponent<FastImageProps>;