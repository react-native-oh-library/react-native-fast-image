import type {RNPackageContext, RNPackage} from '@rnoh/react-native-openharmony/ts';
import {FastImagePackage} from '@react-native-oh-tpl/react-native-fast-image/ts';

export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [new FastImagePackage(ctx)];
}