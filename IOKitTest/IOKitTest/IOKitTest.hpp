#ifndef IOKITTEST_HPP
#define IOKITTEST_HPP
#include <IOKit/IOService.h>

#define IOKitTest com_bugra_IOKitTest

class IOKitTest : public IOService
{
    OSDeclareDefaultStructors(IOKitTest)
    
public:
    virtual bool init(OSDictionary *dictionary = NULL) APPLE_KEXT_OVERRIDE;
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    virtual IOService *probe(IOService *provider, SInt32 *score) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
};
#endif
