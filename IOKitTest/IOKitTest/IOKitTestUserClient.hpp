//
//  IOKitTestUserClient.hpp
//  IOKitTest
//
//  Created by Bugra Karabudak on 23.06.2023.
//

#ifndef IOKitTestUserClient_hpp
#define IOKitTestUserClient_hpp
#include <IOKit/IOUserClient.h>
#include "IOKitTest.hpp"
#include "TestDriverInterface.h"

#define IOkitTestUserClient com_bugra_IOKitTestUserClient

class IOKitTestUserClient : public IOUserClient
{
    OSDeclareDefaultStructors(IOKitTestUserClient)
    
private:
    task_t m_task;
    IOKitTest *m_driver;
    
public:
    virtual bool initWithTask(task_t owningTask, void *securityToken ,UInt32 type, OSDictionary *properties) APPLE_KEXT_OVERRIDE;
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual IOReturn clientClose(void) APPLE_KEXT_OVERRIDE;
    virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments *args, IOExternalMethodDispatch *dispatch,
                                OSObject *target, void *reference) APPLE_KEXT_OVERRIDE;
protected:
    static const IOExternalMethodDispatch sMethods[kTestUserClientMethodCount];
    static IOReturn sStartTimer(OSObject *targer,void *refrence,IOExternalMethodArguments *args);
    static IOReturn sStopTimer(OSObject *targer,void *refrence,IOExternalMethodArguments *args);
    static IOReturn sGetElapsedTimerTime(OSObject *targer,void *refrence,IOExternalMethodArguments *args);
    static IOReturn sGetElapsedTimerValue(OSObject *target, void *reference, IOExternalMethodArguments *args);
    static IOReturn sDelayForMs(OSObject *targer,void *refrence,IOExternalMethodArguments *args);
    static IOReturn sDelayForTime(OSObject *targer,void *refrence,IOExternalMethodArguments *args);
    
};


#endif /* IOKitTestUserClient_hpp */
