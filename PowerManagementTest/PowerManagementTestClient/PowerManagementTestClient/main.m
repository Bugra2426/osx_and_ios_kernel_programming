//
//  main.m
//  PowerManagementTestClient
//
//  Created by Bugra Karabudak on 28.06.2023.
//

#import <Foundation/Foundation.h>
#include <IOKit/IOKitLib.h>
#include "TestDriverInterface.h"


int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        CFDictionaryRef matchingDict = NULL;
        io_service_t service = 0;
        matchingDict = IOServiceMatching("com_bugratest_PowerManagementTest");
        service = IOServiceGetMatchingService(kIOMainPortDefault, matchingDict);
        if (!service) {
            return false;
        }
        task_port_t owningTask = mach_task_self();
        uint32_t type = 0;
        io_connect_t driverConnection;
        kern_return_t kr;
        kr = IOServiceOpen(service, owningTask, type, &driverConnection);
        if (kr != KERN_SUCCESS) {
            return -1;
        }
        IOConnectCallMethod(driverConnection, kPowerManagementTestClientReadData, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL);
        IOServiceClose(service);
        IOObjectRelease(service);
    }
    return 0;
}
