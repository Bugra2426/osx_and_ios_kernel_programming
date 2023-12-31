//
//  main.m
//  IOKitClient
//
//  Created by Bugra Karabudak on 24.06.2023.
//

#import <Foundation/Foundation.h>
#include <IOKit/IOKitLib.h>
#include "TestDriverInterface.h"

kern_return_t StartTimer(io_connect_t connection)
{
    return IOConnectCallMethod(connection, kTestUserClientStartTimer,
                               NULL, 0, NULL, 0, NULL, NULL, NULL, NULL);
    
}

kern_return_t StopTimer(io_connect_t connection)
{
    return IOConnectCallMethod(connection, kTestUserClientStopTimer,
                               NULL, 0, NULL, 0, NULL, NULL, NULL, NULL);
}

kern_return_t GetElapsedTimerTime(io_connect_t connection ,uint32_t *timerTime)
{
    uint64_t scalarOut[1];
    uint32_t scalarOutCount;
    kern_return_t result;
    
    scalarOutCount = 1;
    result = IOConnectCallScalarMethod(connection, kTestUserClientGetElapsedTimerTime, NULL, 0, scalarOut, &scalarOutCount);
    
    if (result == kIOReturnSuccess) {
        *timerTime = (uint32_t)scalarOut[0];
    }
    
    return result;
}

kern_return_t DelayForMs(io_connect_t connection, const TimerValue *timerValue)
{
    return IOConnectCallStructMethod(connection, kTestUserClientDelayForTime,
                                         timerValue, sizeof(TimerValue), NULL, 0);
}



int main(int argc, const char * argv[]) {
    @autoreleasepool {
        CFDictionaryRef machingDict = NULL;
        io_service_t service = 0;
        machingDict = IOServiceMatching("com_bugratest_driver_IOKitTest");
        service = IOServiceGetMatchingService(kIOMainPortDefault, machingDict);
        if (!service) {
            return -1;
        }
        
        task_port_t owningTask = mach_task_self();
        uint32_t type = 0;
        io_connect_t driverConnection;
        kern_return_t kr;
        kr = IOServiceOpen(service, owningTask, type, &driverConnection);
        if (kr != KERN_SUCCESS) {
            return -1;
        }
        
        StartTimer(driverConnection);
        StopTimer(driverConnection);
        IOServiceClose(service);
        IOObjectRelease(service);
    }
    return 0;
}
