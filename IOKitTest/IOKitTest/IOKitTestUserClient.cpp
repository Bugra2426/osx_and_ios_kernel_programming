//
//  IOKitTestUserClient.cpp
//  IOKitTest
//
//  Created by Bugra Karabudak on 23.06.2023.
//
#include <IOKit/IOLib.h>
#include "IOKitTestUserClient.hpp"

#define super IOUserClient

OSDefineMetaClassAndStructors(IOKitTestUserClient, IOUserClient)

const IOExternalMethodDispatch
IOKitTestUserClient::sMethods[kTestUserClientMethodCount] =
{
    { sStartTimer, 0, 0, 0, 0 },
    { sStopTimer, 0, 0, 0, 0  },
    { sGetElapsedTimerTime, 0, 0, 1, 0 },
    { sGetElapsedTimerValue, 0, 0, 0, sizeof(TimerValue) },
    { sDelayForMs, 1, 0, 0, 0 },
    { sDelayForTime, 0, sizeof(TimerValue), 0, 0 }
};


bool IOKitTestUserClient::initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties)
{
    if (!owningTask){
        return false;
    }
    
    if (!super::initWithTask(owningTask, securityToken, type, properties)){
        return false;
    }
    m_task = owningTask;
    IOReturn ret = clientHasPrivilege(securityToken, kIOClientPrivilegeAdministrator);
    if (ret == kIOReturnSuccess){
    
    }
    return true;
    
}

void IOKitTestUserClient::free(void)
{
    super::free();
}


bool IOKitTestUserClient::start(IOService *provider)
{
    if (!super::start(provider)) {
           return false;
       }
    
    m_driver = OSDynamicCast(IOKitTest, provider);

    if (!m_driver){
        return false;
    }
    
    return true;
}


IOReturn IOKitTestUserClient::clientClose(void)
{
    terminate();
    return kIOReturnSuccess;
}

void IOKitTestUserClient::stop(IOService *provider)
{
    super::stop(provider);
}

IOReturn IOKitTestUserClient::externalMethod(uint32_t selector, IOExternalMethodArguments *args, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference)
{
    if (selector >= kTestUserClientMethodCount){
        return kIOReturnUnsupported;
    }
    dispatch = (IOExternalMethodDispatch*)&sMethods[selector];
    target = this;
    reference = NULL;
    return super::externalMethod(selector, args, dispatch, target, reference);
    
}

IOReturn IOKitTestUserClient::sStartTimer(OSObject *targer, void *refrence, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::SStartTimer\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sStopTimer(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sStopTimer\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sGetElapsedTimerTime(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sGetElapsedTimerTime\n");
    return KERN_SUCCESS;
}


IOReturn IOKitTestUserClient::sGetElapsedTimerValue(OSObject *target, void *reference, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sGetElapsedTimerValue\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sDelayForMs(OSObject *targer, void *refrence, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sDelayForMs\n");
    return KERN_SUCCESS;
}

IOReturn IOKitTestUserClient::sDelayForTime(OSObject *targer, void *refrence, IOExternalMethodArguments *args)
{
    IOLog("IOKitTestUserClient::sDelayForTime\n");
    return KERN_SUCCESS;
}


