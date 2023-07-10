#include "PowerManagementTest.hpp"
#include <IOKit/IOLib.h>

#define super IOService

OSDefineMetaClassAndStructors(PowerManagementTest, IOService)

enum {
    kOffPowerState,
    kStandbyPowerState,
    kIdlePowerState,
    kOnPowerState,
    kNumPowerStates
};

static IOPMPowerState gPowerStates[kNumPowerStates] = {
        {kIOPMPowerStateVersion1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {kIOPMPowerStateVersion1, kIOPMPowerOn, kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0},
        {kIOPMPowerStateVersion1, kIOPMPowerOn, kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0},
        {kIOPMPowerStateVersion1, (kIOPMPowerOn | kIOPMDeviceUsable), kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0}
    
};

bool PowerManagementTest::start(IOService *provider)
{
    IOLog("PowerManagementTest::start\n");
    if (!super::start(provider)) {
        return false;
    }
    m_lock = IOLockAlloc();
    if (!m_lock) {
        return false;
    }
    PMinit();
    provider->joinPMtree(this);
    makeUsable();
    ChangePowerState(kOffPowerState);
    registerPowerDriver(this, gPowerStates, kNumPowerStates);
    setIdleTimerPeriod(60);
    registerService();
    return true;

}

void PowerManagementTest::stop(IOService *provider)
{
    IOLog("PowerManagementTest::stop\n");
    PMstop();
    super::stop(provider);
}

void PowerManagementTest::free()
{
    IOLog("PowerManagementTest::free\n");
    if (m_lock) {
        IOLockFree(m_lock);
    }
    super::free();
}

IOReturn PowerManagementTest::setPowerState(unsigned long powerStateOrdinal, IOService *whatDevice)
{
    IOLog("PowerManagementTest::setPowerState(0x%lx)\n", powerStateOrdinal);
    if (powerStateOrdinal < m_devicePowerState) {
        m_devicePowerState = powerStateOrdinal;
    }
    switch (powerStateOrdinal) {
        case kOffPowerState:
        case kStandbyPowerState:
        case kIdlePowerState:
            
            IOLockLock(m_lock);
            while (m_outstandIQ != 0) {
                IOLockSleep(m_lock, &m_outstandIQ, THREAD_UNINT);
            }
            IOLockUnlock(m_lock);
            
        
            break;
    }
    if (powerStateOrdinal > m_devicePowerState) {
        m_devicePowerState = powerStateOrdinal;
    }
    return  kIOPMAckImplied;
}

IOReturn PowerManagementTest::powerStateWillChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *whatDevice)
{
    IOLog("PowerManagementTest::powerStateWillChangeTo(0x%lx)\n",stateNumber);
    return  IOPMAckImplied;
}

IOReturn PowerManagementTest::powerStateDidChangeTo(IOPMPowerFlags capabilities, unsigned long stateNumber, IOService *whatDevice)
{
    IOLog("PowerManagementTest::powerStateDidChangeTo(0x%lx)\n", stateNumber);
    return IOPMAckImplied;
}

void PowerManagementTest::powerChangeDone(unsigned long stateNumber)
{
    IOLog("PowerManagementTest::powerChangeDone(0x%lx)\n",stateNumber);
    IOLockWakeup(m_lock, &m_devicePowerState, false);
}

IOReturn PowerManagementTest::myReadDataFromDevice()
{
    IOLockLock(m_lock);
    if (!activityTickle(kIOPMSuperclassPolicy1, kOnPowerState)) {
        while (m_devicePowerState != kOnPowerState) {
            IOLog("PowerManagementTest waiting on power transition\n");
            IOLockSleep(m_lock, &m_devicePowerState, THREAD_UNINT);
        }
        
    }
    m_outstandIQ += 1;
    IOLockUnlock(m_lock);
    IOLog("PowerManagementTest reading some data\n");
    IOLockLock(m_lock);
    m_outstandIQ -= 1;
    IOLockWakeup(m_lock, &m_devicePowerState, false);
    IOLockUnlock(m_lock);
    return kIOReturnSuccess;
}
