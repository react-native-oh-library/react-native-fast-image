import React from 'react';
import { View, StyleSheet, SafeAreaView } from 'react-native';
import { FastImageLoadingDelay } from './src/FastImageLoadingDelay';
import { FastImageLoadingDelayNatives } from './src/FastImageLoadingDelayNatives';
import { FastImageMothodDemo } from './src/FastImageMothodDemo';
import { FastImageStyleDemo } from './src/FastImageStyleDemo';
import { FastImageSourceDemo } from './src/FastImageSourceDemo';
import {NavigationContainer, Page} from './src/Navigation';

export default function FastImageDemo() {
  // renders
  return (
    <View style={styles.container}>
      <SafeAreaView>
        <NavigationContainer>
          <Page name='FastImageMothodDemo'><FastImageMothodDemo></FastImageMothodDemo></Page>
          <Page name='FastImageStyleDemo'><FastImageStyleDemo></FastImageStyleDemo></Page>
          <Page name='FastImageSourceDemo'><FastImageSourceDemo></FastImageSourceDemo></Page>
          <Page name='FastImageLoadingDelayNatives'><FastImageLoadingDelayNatives></FastImageLoadingDelayNatives></Page>
          <Page name='FastImageLoadingDelay'><FastImageLoadingDelay></FastImageLoadingDelay></Page>
        </NavigationContainer>
      </SafeAreaView>
    </View>
  );
}
const styles = StyleSheet.create({
  container: {
    flex:1,
    backgroundColor: 'grey',
  },
});