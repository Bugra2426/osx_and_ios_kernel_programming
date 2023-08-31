//
//  PowerManagementTestUserClient.cpp
//  PowerManagementTest
//
//  Created by Bugra Karabudak on 28.06.2023.
//

#include "PowerManagementTestUserClient.hpp"
#include <IOKit/IOLib.h>

#define super IOUserClient
OSDefineMetaClassAndStructors(PowerManagementTestUserClient, IOUserClient)
const IOExternalMethodDispatch PowerManagementTestUserClient::sMethods[kPowerManagementTestClientMethodCount] =
{
    { sReadData, 0, 0, 0, 0 }
};
bool PowerManagementTestUserClient::initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties)
{
    if (!owningTask){
        return false;
    }
    if (!super::initWithTask(owningTask, securityToken , type, properties)) {
            return false;
    }
        
    task = owningTask;
    IOReturn ret = clientHasPrivilege(securityToken, kIOClientPrivilegeAdministrator);
    if (ret == kIOReturnSuccess) {
        
    }
    return true;
}
void PowerManagementTestUserClient::free(void)
{
    super::free();
}

bool PowerManagementTestUserClient::start(IOService *provider)
{
    if (!super::start(provider)) {
        return false;
    }
    m_driver = OSDynamicCast(PowerManagementTest, provider);
    if (!m_driver) {
        return false;
    }
    return true;
}

IOReturn PowerManagementTestUserClient::clientClose()
{
    terminate();
    return kIOReturnSuccess;
}

void PowerManagementTestUserClient::stop(IOService *provider)
{
    super::stop(provider);
}

IOReturn PowerManagementTestUserClient::externalMethod(uint32_t selector, IOExternalMethodArguments *args, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference)
{
    if (selector >= kPowerManagementTestClientMethodCount) {
        return kIOReturnUnsupported;
    }
    dispatch = (IOExternalMethodDispatch*)&sMethods[selector];
    target = this;
    reference = NULL;
    return super::externalMethod(selector, args, dispatch, target, reference);
}

IOReturn PowerManagementTestUserClient::sReadData(OSObject *target, void *refrence, IOExternalMethodArguments *args)
{
    IOLog("PowerManagementTestUserClient::sReadData\n");
    PowerManagementTestUserClient *me;
    me = (PowerManagementTestUserClient *) target;
    me->m_driver->myReadDataFromDevice();
    return kIOReturnSuccess;
}
