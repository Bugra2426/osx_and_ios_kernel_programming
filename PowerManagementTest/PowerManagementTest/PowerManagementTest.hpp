#ifndef POWERMANAGEGEMENTTEST_HPP
#define POWERMANAGEGEMENTTEST_HPP
#include <IOKit/IOService.h>

#define PowerManagementTest com_bugratest_PowerManagementTest

class PowerManagementTest : public IOService
{
  OSDeclareDefaultStructors(PowerManagementTest)
private:
    IOLock *m_lock;
    unsigned long m_devicePowerState;
    SInt32 m_outstandIQ;
protected:
    virtual void powerChangeDone(unsigned long stateNumber) APPLE_KEXT_OVERRIDE;
    virtual IOReturn powerStateWillChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *WhatDevice) APPLE_KEXT_OVERRIDE;
    virtual IOReturn powerStateDidChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *WhatDevice) APPLE_KEXT_OVERRIDE;
    
public:
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual IOReturn setPowerState(unsigned long powerStateOrdinal, IOService *whatDevice) APPLE_KEXT_OVERRIDE;
    
    IOReturn myReadDataFromDevice();
    
    
};


#endif

