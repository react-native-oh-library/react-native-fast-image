#include "FastImageLoaderTurboModule.h"
#include "RNOH/ArkTSTurboModule.h"

using namespace rnoh;
using namespace facebook;

rnoh::FastImageLoaderTurboModule::FastImageLoaderTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : rnoh::ArkTSTurboModule(ctx, name){
  methodMap_ = {
      ARK_METHOD_METADATA(getConstants, 0),
      ARK_ASYNC_METHOD_METADATA(getSize, 1),
      ARK_ASYNC_METHOD_METADATA(getCacheFilePath, 1),
      ARK_ASYNC_METHOD_METADATA(getSizeWithHeaders, 2),
      ARK_ASYNC_METHOD_METADATA(prefetchImage, 2),
      ARK_ASYNC_METHOD_METADATA(prefetchImageWithMetadata, 3),
      ARK_ASYNC_METHOD_METADATA(queryCache, 1)};
    
    m_ctx.taskExecutor->runTask(
      TaskThread::MAIN, [weakInstance = m_ctx] {
        auto instance = weakInstance.instance.lock();
        if (!instance) {
          return;
        }
        auto self = instance->getTurboModule<FastImageLoaderTurboModule>("FastImageLoader");
        if (!self) {
          return;
        }
        if (self->m_FastImageSourceResolver) {
          return;
        }
        
        self->m_FastImageSourceResolver = std::make_shared<FastImageSourceResolver>(
            weakInstance.arkTSMessageHub, instance);
    });
}
