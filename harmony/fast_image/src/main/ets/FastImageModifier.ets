import {
  TransformMatrix,
  getTintColorMatrix
} from 'rnoh';
import {FastImageViewDescriptor, FastImageDescriptorWrapper } from './RNFastImage';
import matrix4 from '@ohos.matrix4';

export class FastImageModifier implements AttributeModifier<ImageAttribute>{
  private constructor() {}
  private static instance: FastImageModifier;
  protected descriptor: FastImageViewDescriptor = Object() as FastImageViewDescriptor
  protected descriptorWrapper :FastImageDescriptorWrapper | undefined = undefined

  public static getInstance(): FastImageModifier {
    if (!FastImageModifier.instance) {
      FastImageModifier.instance = new FastImageModifier();
    }
    return FastImageModifier.instance;
  }

  setDescriptor(descriptor:FastImageViewDescriptor, descriptorWrapper:FastImageDescriptorWrapper) :FastImageModifier{
    this.descriptor = descriptor;
    this.descriptorWrapper = descriptorWrapper;
    return FastImageModifier.instance;
  }

  getHitTestMode() {
    switch(this.descriptorWrapper?.pointerEvents) {
      case "none":
        return HitTestMode.None
      case "box-none":
        return HitTestMode.None
      case "box-only":
        return HitTestMode.Block
      default :
        return HitTestMode.Default
    }
  }

  getBorderStyle(): BorderStyle {
    switch (this.descriptorWrapper?.borderStyle) {
      case "dashed":
        return BorderStyle.Dashed
      case "dotted":
        return BorderStyle.Dotted
      default :
        return BorderStyle.Solid
    }
  }

  getTransform(): undefined | matrix4.Matrix4Transit {
    const descriptorWrapper = this.descriptorWrapper
    if (!descriptorWrapper) {
      return undefined
    }
    const rawTransformationMatrix = descriptorWrapper.rawTransformationMatrix.slice()
    rawTransformationMatrix[12] = vp2px(rawTransformationMatrix[12]);
    rawTransformationMatrix[13] = vp2px(rawTransformationMatrix[13]);
    rawTransformationMatrix[14] = vp2px(rawTransformationMatrix[14]);
    return matrix4.init(rawTransformationMatrix as TransformMatrix)
  }

  getResizeMode(resizeMode: number) {
    switch (resizeMode) {
      case 1:
        return ImageFit.Contain;
      case 2:
        return ImageFit.Fill;
      case 3:
        return ImageFit.None;
      default:
        return ImageFit.Cover;
    }
  }

  applyNormalAttribute(instance: ImageAttribute): void {
    // instance.width(this.descriptorWrapper?.width);
    // instance.height(this.descriptorWrapper?.height);
    instance.position(this.descriptorWrapper?.positionRelativeToParent);

    if(this.descriptorWrapper?.backgroundColor) {
      instance.backgroundColor(this.descriptorWrapper?.backgroundColor)
    }
    if(this.descriptorWrapper?.borderWidth) {
      instance.borderWidth(this.descriptorWrapper?.borderWidth)
    }
    if(this.descriptorWrapper?.borderColor) {
      instance.borderColor(this.descriptorWrapper?.borderColor)
    }
    if(this.descriptorWrapper?.borderRadius) {
      instance.borderRadius(this.descriptorWrapper?.borderRadius)
    }
    if(this.descriptorWrapper?.borderStyle === 'dashed' || this.descriptorWrapper?.borderStyle === 'dotted') {
      instance.borderStyle(this.getBorderStyle())
    }
    if(this.descriptorWrapper?.opacity) {
      instance.opacity(this.descriptorWrapper?.opacity)
    }
    // if(this.descriptorWrapper?.transformStringify !== '[1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]') {
    //   instance.transition(this.getTransform())
    // }
    if(this.descriptorWrapper?.isClipping) {
      instance.clip(this.descriptorWrapper?.isClipping)
    }
    let hitTestBehavior :HitTestMode = this.getHitTestMode();
    if(hitTestBehavior !== HitTestMode.Default) {
      instance.hitTestBehavior(hitTestBehavior)
    }
    if(this.descriptorWrapper?.shadow) {
      instance.shadow(this.descriptorWrapper?.shadow)
    }

    if(this.descriptor.rawProps.alt) {
      instance.accessibilityText(this.descriptor.rawProps.alt)
    }
    if(this.getResizeMode(this.descriptor.props.resizeMode) !== ImageFit.Cover) {
      instance.objectFit(this.getResizeMode(this.descriptor.props.resizeMode))
    }
    if(this.descriptor.props.resizeMode === 4) {
      instance.objectRepeat(ImageRepeat.XY)
    }
    if (!this.descriptor.props.tintColor ||  this.descriptor.props.tintColor.every((element) => element ===0)) {
    } else {
      instance.colorFilter(getTintColorMatrix(this.descriptor.props.tintColor))
    }
  }
}