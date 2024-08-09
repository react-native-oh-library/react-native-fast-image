#include "FastImageLoaderTurboModule.h"
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/RNInstanceCAPI.h"

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
    
    if(m_FastImageSourceResolver){
        return;
    }

    if(auto instance = m_ctx.instance.lock(); instance != nullptr){
        m_FastImageSourceResolver = std::make_shared<FastImageSourceResolver>(instance);
        auto rnInstance = dynamic_cast<RNInstanceCAPI*>(instance.get());
        rnInstance->addArkTSMessageHandler(m_FastImageSourceResolver);
    }
    
}
