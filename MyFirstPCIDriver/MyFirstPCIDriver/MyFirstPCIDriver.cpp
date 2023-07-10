#include <IOKit/IOLib.h>

#include "MyFirstPCIDriver.hpp"

#define super IOService

OSDefineMetaClassAndStructors(MyFirstPCIDriver, IOService);

bool MyFirstPCIDriver::start(IOService *provider)
{
    IOLog("%s::start\n", getName());
    if (!super::start(provider)) {
        return false;
    }
    
    fIOPCIDevice = OSDynamicCast(IOPCIDevice , provider);
    if (!fIOPCIDevice) {
        return false;
    }
    
    fIOPCIDevice->setMemoryEnable(true);
    UInt16 vendorID = fIOPCIDevice->configRead16(kIOPCIConfigVendorID);
    UInt16 deviceID = fIOPCIDevice->configRead16(kIOPCIConfigDeviceID);
    IOLog("vendor ID = 0x%04x device ID = 0x%04x\n", vendorID, deviceID);
    IOByteCount offset = 0;
    if (fIOPCIDevice->extendedFindPCICapability(kIOPCIPCIExpressCapability, &offset)) {
        UInt16 linkStatusRegister = fIOPCIDevice->extendedConfigRead16(offset + 0x12);
        IOLog("link status register = 0x%04x\n", linkStatusRegister);
        return 0;
        }

    for (UInt32 i = 0; i < fIOPCIDevice->getDeviceMemoryCount(); i++) {
        IODeviceMemory* memoryDesc = fIOPCIDevice->getDeviceMemoryWithIndex(i);
        if (!memoryDesc){
            continue;
        }
        IOLog("region%u: length=%llu bytes\n", i, memoryDesc->getLength());

    }
    RegisterService();
    return true;
    
    
};


void MyFirstPCIDriver::stop(IOService *provider)
{
    IOLog("%s::stop\n",getName());
    super::stop(provider);
}
