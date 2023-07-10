#ifndef MYFIRSTPCIDRIVER_HPP
#define MYFIRSTPCIDRIVER_HPP
#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <PCIDriverKit/IOPCIDevice.h>
#define MyFirstPCIDriver com_bugratest_MyFirstPCIDriver

class MyFirstPCIDriver : public IOService {
    OSDeclareDefaultStructors(MyFirstPCIDriver);
private:
    IOPCIDevice *fIOPCIDevice;
    
public:
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
};


#endif

