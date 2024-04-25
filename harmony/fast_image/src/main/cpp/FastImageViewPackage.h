#include "RNOH/Package.h"
#include "ComponentDescriptors.h"
#include "FastImageViewJSIBinder.h"
#include "RNCFastImageViewTurboModule.h"
#include "FastImageViewComponentInstance.h"

namespace rnoh {

class FastImageViewComponentInstanceFactoryDelegate : public ComponentInstanceFactoryDelegate {
public:
    using ComponentInstanceFactoryDelegate::ComponentInstanceFactoryDelegate;

    ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
        if (ctx.componentName == "FastImageView") {
            return std::make_shared<FastImageViewComponentInstance>(std::move(ctx));
        }
        return nullptr;
    }
};

class FastImageTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "RNCFastImageView") {
            return std::make_shared<RNCFastImageViewTurboModule>(ctx, name);
        }
        return nullptr;
    };
};

class FastImageViewPackage : public Package {
public:
    FastImageViewPackage(Package::Context ctx) : Package(ctx) {}

    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override {
        return std::make_unique<FastImageTurboModuleFactoryDelegate>();
    }

    ComponentInstanceFactoryDelegate::Shared createComponentInstanceFactoryDelegate() override {
        return std::make_shared<FastImageViewComponentInstanceFactoryDelegate>();
    }

    std::vector<facebook::react::ComponentDescriptorProvider> createComponentDescriptorProviders() override {
        return {
            facebook::react::concreteComponentDescriptorProvider<facebook::react::FastImageViewComponentDescriptor>()};
    }

    ComponentJSIBinderByString createComponentJSIBinderByName() override {
        return {{"FastImageView", std::make_shared<FastImageViewJSIBinder>()}};
    }
};
} // namespace rnoh