// This file was generated.
import {
  Descriptor as ComponentDescriptor,
  ViewBaseProps,
  ViewRawProps,
  ViewDescriptorWrapperBase,
  ColorValue,
  Color,
  RNInstance,
  Tag,
  RNComponentCommandReceiver,
  ViewPropsSelector,
} from '@rnoh/react-native-openharmony/ts';


export namespace FastImageView {
  export const NAME = "FastImageView" as const

  export interface DirectRawProps {
    source?: {uri?: string, headers?: {name: string, value: string}[], priority?: 'low' | 'normal' | 'high', cache?: 'immutable' | 'web' | 'cacheOnly'};
    defaultSource?: string;
    resizeMode?: 'contain' | 'cover' | 'stretch' | 'center';
    tintColor?: ColorValue;
  }

  export interface Props extends ViewBaseProps {}

  export interface State {}

  export interface RawProps extends ViewRawProps, DirectRawProps {}

  export class PropsSelector extends ViewPropsSelector<Props, RawProps> {
    get source() {
      return this.rawProps.source;
    }

    get defaultSource() {
      return this.rawProps.defaultSource;
    }

    get resizeMode() {
      return this.rawProps.resizeMode ?? 'cover';
    }


    get tintColor() {
      if (this.rawProps.tintColor) {
        return Color.fromColorValue(this.rawProps.tintColor)
      } else {
        return new Color({ r: 0, g: 0, b: 0, a: 255})
      }
    }

  }

  export type Descriptor = ComponentDescriptor<
  typeof NAME,
  Props,
  State,
  RawProps
  >;

  export class DescriptorWrapper extends ViewDescriptorWrapperBase<
  typeof NAME,
  Props,
  State,
  RawProps,
  PropsSelector
  > {
    protected createPropsSelector() {
      return new PropsSelector(this.descriptor.props, this.descriptor.rawProps)
    }
  }

  export interface EventPayloadByName {
    "fastImageError": {}
    "fastImageLoad": {width: number, height: number}
    "fastImageLoadEnd": {}
    "fastImageLoadStart": {}
    "fastImageProgress": {loaded: number, total: number}
  }

  export class EventEmitter {
    constructor(private rnInstance: RNInstance, private tag: Tag) {}

    emit<TEventName extends keyof EventPayloadByName>(eventName: TEventName, payload: EventPayloadByName[TEventName]) {
      this.rnInstance.emitComponentEvent(this.tag, eventName, payload)
    }
  }

  export interface CommandArgvByName {
  }

  export class CommandReceiver {
    private listenersByCommandName = new Map<string, Set<(...args: any[]) => void>>()
    private cleanUp: (() => void) | undefined = undefined

    constructor(private componentCommandReceiver: RNComponentCommandReceiver, private tag: Tag) {
    }

    subscribe<TCommandName extends keyof CommandArgvByName>(commandName: TCommandName, listener: (argv: CommandArgvByName[TCommandName]) => void) {
      if (!this.listenersByCommandName.has(commandName)) {
        this.listenersByCommandName.set(commandName, new Set())
      }
      this.listenersByCommandName.get(commandName)!.add(listener)
      const hasRegisteredCommandReceiver = !!this.cleanUp
      if (!hasRegisteredCommandReceiver) {
        this.cleanUp = this.componentCommandReceiver.registerCommandCallback(this.tag, (commandName: string, argv: any[]) => {
          if (this.listenersByCommandName.has(commandName)) {
            const listeners = this.listenersByCommandName.get(commandName)!
            listeners.forEach(listener => {
              listener(argv)
            })
          }
        })
      }

      return () => {
        this.listenersByCommandName.get(commandName)?.delete(listener)
        if (this.listenersByCommandName.get(commandName)?.size ?? 0 === 0) {
          this.listenersByCommandName.delete(commandName)
        }
        if (this.listenersByCommandName.size === 0) {
          this.cleanUp?.()
        }
      }
    }
  }

}
