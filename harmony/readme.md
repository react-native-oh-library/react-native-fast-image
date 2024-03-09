# react-native-fast-image（imageKnife）

### 简介

该版本使用了imageKnife的网络请求和缓存策略，为了与原版本做区分源码请访问本库imageKnife分支，库tgz压缩包请使用带imageKnife后缀的版本。

### 快速使用

##### 下载

使用本版本需要额外在项目中下载imageknife

```
ohpm install @ohos/imageknife
```

##### 依赖配置

在`SampleApp/entry/src/main/ets/entryability/EntryAbility.ets` （UIAbility）中初始化全局ImageKnife.

```
  onWindowStageCreate(windowStage: window.WindowStage) {
    super.onWindowStageCreate(windowStage)
    // 初始化全局ImageKnife
    ImageKnife.with(this.context);
  }
```

##### link

原生配置参考 [react-native-fast-image使用文档](https://react-native-oh-library.gitee.io/usage-docs/#/zh-cn/react-native-fast-image).

