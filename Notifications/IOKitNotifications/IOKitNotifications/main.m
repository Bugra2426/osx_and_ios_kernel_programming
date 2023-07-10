//
//  main.m
//  IOKitNotifications
//
//  Created by Bugra Karabudak on 24.06.2023.
//

#import <Foundation/Foundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>


typedef struct {
    io_service_t service;
    io_object_t notification;
} MyDriverData;

IONotificationPortRef gNotificationPort = NULL;

void DeviceNotification(void *refCon, io_service_t service, natural_t messageType,
                        void *messageArgument)
{
    MyDriverData *myDriverData = (MyDriverData *)refCon;
    kern_return_t kr;
    
    
    if (messageType == kIOMessageServiceIsTerminated) {
        io_name_t name;
        
        IORegistryEntryGetName(service, name);
        printf("Device Removed %s",name);
        kr = IOObjectRetain(myDriverData->notification);
        IOObjectRelease(myDriverData->service);
        free(myDriverData);
    }
    
}
    

void DeviceAdded(void *refCon , io_iterator_t iterator) {
    io_service_t service = 0;
    
    
    while ((service = IOIteratorNext(iterator)) != 0) {
        CFStringRef ClassName;
        io_name_t name;
        
        ClassName = IOObjectCopyClass(service);
        if (CFEqual(ClassName, CFSTR("IOUSBDevice"))) {
           
            IORegistryEntryGetName(service, name);
            printf("Found device with name: %s\n", name);
            
            CFTypeRef vendorName;
            vendorName = IORegistryEntryCreateCFProperty(service, CFSTR("TestUSB"), kCFAllocatorDefault, 0);
            CFShow(vendorName);
            MyDriverData *myDriverData;
            kern_return_t kr;
            myDriverData = (MyDriverData *)malloc(sizeof(MyDriverData));
            myDriverData->service = service;
            kr = IOServiceAddInterestNotification(gNotificationPort, service, kIOGeneralInterest, DeviceNotification, myDriverData, &myDriverData->notification);
            
            

        }
        
        CFRelease(ClassName);
        IOObjectRelease(service);
        
    };
    
}


int main(int argc, const char * argv[])
{
    @autoreleasepool {
        CFDictionaryRef machingDict  = NULL;
        io_iterator_t iter = 0;
        CFRunLoopSourceRef runLoopSource;
        kern_return_t kr;
        machingDict = IOServiceMatching("IOUSBDevice");
        gNotificationPort = IONotificationPortCreate(kIOMainPortDefault);
        runLoopSource = IONotificationPortGetRunLoopSource(gNotificationPort);
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
        kr = IOServiceAddMatchingNotification(gNotificationPort, kIOFirstMatchNotification, machingDict, DeviceAdded, NULL, &iter);
        DeviceAdded(NULL, iter);
        CFRunLoopRun();
        IONotificationPortDestroy(gNotificationPort);
        IOObjectRelease(iter);
    }
    return 0;
}
