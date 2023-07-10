//
//  PowerManagementTestUserClient.hpp
//  PowerManagementTest
//
//  Created by Bugra Karabudak on 28.06.2023.
//

#ifndef POWERMANAGEMENTTESTUSERCLIENT_HPP
#define POWERMANAGEMENTTESTUSERCLIENT_HPP

#include <IOKit/IOUserClient.h>
#include "PowerManagementTest.hpp"
#include "TestDriverInterface.h"

#define PowerManagementTestUserClient com_bugratest_PowerManagementTestUserClient
class PowerManagementTestUserClient : public IOUserClient
{
    OSDeclareDefaultStructors(PowerManagementTestUserClient)
private:
    task_t task;
    PowerManagementTest *m_driver;
public:
    virtual bool initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual IOReturn clientClose(void) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments *args, IOExternalMethodDispatch *dispatch,
                                OSObject *target, void *reference) APPLE_KEXT_OVERRIDE;
        protected:
    static const IOExternalMethodDispatch sMethods[kPowerManagementTestClientMethodCount];
    static IOReturn sReadData(OSObject *target, void *refrence, IOExternalMethodArguments *args);
};
#endif /* POWERMANAGEMENTTESTUSERCLIENT_HPP */
