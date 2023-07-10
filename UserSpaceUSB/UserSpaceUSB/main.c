//
//  main.c
//  UserSpaceUSB
//
//  Created by Bugra Karabudak on 9.07.2023.
//

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/usb/USBSpec.h>


CFDictionaryRef MyCrateUSBMatchingDictionary(SInt32 idVendor, SInt32 idProduct);
void MyFindMatchingDevices(CFDictionaryRef matchingDictionary);
IOUSBDeviceInterface942 **MyStartDriver(io_service_t usbDeviceRef);
IOReturn PrintDeviceManufacturer(IOUSBDeviceInterface942 **usbDevice);
IOReturn MyConfigureDevice(IOUSBDeviceInterface942 **usbDevice);
IOUSBInterfaceInterface942 **MyCreateInterfaceClass(io_service_t usbInterfaceRef);
IOReturn MyListEndpoints(IOUSBInterfaceInterface942 **usbInterface);

int main(int argc, const char * argv[]) {
    
    CFDictionaryRef matchingDictionary = NULL;
    matchingDictionary = MyCrateUSBMatchingDictionary(0x05AC, 0x8600);
    if(matchingDictionary) {
        MyFindMatchingDevices(matchingDictionary);
    }
    return 0;
}
CFDictionaryRef MyCreateUSBMatchingDictionary(SInt32 idVendor, SInt32 idProduct)
{
    CFMutableDictionaryRef matchingDictionary = NULL;
    CFNumberRef numberRef;
    matchingDictionary = IOServiceMatching(kIOUSBDeviceClassName);
    if(!matchingDictionary) {
        goto bail;
    }
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &idVendor);
    if(!numberRef) {
        goto bail;
    }
    CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBVendorID), numberRef);
    CFRelease(numberRef);
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &idProduct);
    if(!numberRef) {
        goto bail;
    }
    CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBProductID), numberRef);
    CFRelease(numberRef);
    return matchingDictionary;
bail:
    // Failure - release resources and return NULL
    if (matchingDictionary) {
        CFRelease(matchingDictionary);
    }
    
    return NULL;
};

void MyFindMatchingDevices(CFDictionaryRef matchingDictionary)
{
    io_iterator_t iterator = 0;
    io_service_t usbDeviceRef;
    kern_return_t err;
    err = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDictionary, &iterator);
    if(err == KERN_SUCCESS) {
        while ((usbDeviceRef = IOIteratorNext(iterator)) != 0) {
            MyStartDriver(usbDeviceRef);
            IOObjectRelease(usbDeviceRef);
        }
        IOObjectRelease(iterator);
    }

}

IOUSBDeviceInterface942 **MyStartDriver(io_service_t usbDeviceRef)
{
    SInt32 score;
    IOCFPlugInInterface **iodev;
    IOUSBDeviceInterface942 **usbDevice = NULL;
    kern_return_t err;
    
    err = IOCreatePlugInInterfaceForService(usbDeviceRef, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &iodev, &score);
    if ((err != KERN_SUCCESS || !iodev)) {
        printf("Unable to create a plug-in (%08x)\n", err);
        return NULL;
    }
    err = (*iodev)->QueryInterface(iodev, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID942), (LPVOID)&usbDevice);
    IODestroyPlugInInterface(iodev);
    if (err != KERN_SUCCESS || !usbDevice) {
        printf("Couldn’t create a device interface (%08x)\n", (int) err);
        return NULL;
    }
    if ((*usbDevice)->USBDeviceOpen(usbDevice) == kIOReturnSuccess) {
        (*usbDevice)->ResetDevice(usbDevice);
        PrintDeviceManufacturer(usbDevice);
        MyConfigureDevice(usbDevice);
        (*usbDevice)->USBDeviceClose(usbDevice);

    }
    return usbDevice;
    
    
}

IOReturn PrintDeviceManufacturer(IOUSBDeviceInterface942 **usbDevice)
{
    UInt8 stringIndex;
    IOUSBDeviceRequest devRequest;
    UInt8 buffer[256];
    CFStringRef manufString;
    IOReturn error;
    error = (*usbDevice)->USBGetManufacturerStringIndex(usbDevice, &stringIndex);
    if(error != kIOReturnSuccess) {
        return error;
    }
    devRequest.bmRequestType = USBmakebmRequestType(kUSBIn, kUSBStandard, kUSBDevice);
    devRequest.bRequest = kUSBRqGetDescriptor;
    devRequest.wValue = (kUSBStringDesc << 8) | stringIndex;
    devRequest.wIndex = 0x409;
    devRequest.wLength = sizeof(buffer);
    devRequest.pData = &buffer[0];
    bzero(&buffer[0], sizeof(buffer));
    error = (*usbDevice)->DeviceRequest(usbDevice, &devRequest);
    if(error != kIOReturnSuccess)
        return error;
    int strLength;
    strLength = buffer[0] - 2;
    manufString = CFStringCreateWithBytes(kCFAllocatorDefault, &buffer[2], strLength, kCFStringEncodingUTF16LE, false);
    CFShow(manufString);
    CFRelease(manufString);
    return error;
}
IOReturn MyConfigureDevice(IOUSBDeviceInterface942 **usbDevice)
{
    UInt8 numConfigrations;
    IOUSBConfigurationDescriptorPtr configDesc;
    IOUSBFindInterfaceRequest interfaceRequest;
    io_iterator_t interfaceIterator;
    io_service_t usbInterfaceRef;
    IOReturn error;
    error = (*usbDevice)->GetNumberOfConfigurations(usbDevice, &numConfigrations);
    if((error != kIOReturnSuccess) || (numConfigrations == 0)) {
        return error;
    }
    error = (*usbDevice)->GetConfigurationDescriptorPtr(usbDevice, 0,&configDesc);
    if(error != kIOReturnSuccess) {
        return error;
    }
    error = (*usbDevice)->SetConfiguration(usbDevice,configDesc->bConfigurationValue);
    if(error != kIOReturnSuccess) {
        return error;
    }
    interfaceRequest.bInterfaceClass = kIOUSBFindInterfaceDontCare;
    interfaceRequest.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
    interfaceRequest.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
    interfaceRequest.bAlternateSetting = kIOUSBFindInterfaceDontCare;
    error = (*usbDevice)->CreateInterfaceIterator(usbDevice, &interfaceRequest, &interfaceIterator);
    if (error != kIOReturnSuccess) {
        return error;
    }
    while ((usbInterfaceRef = IOIteratorNext(interfaceIterator)) != 0) {
        IOUSBInterfaceInterface942 **usbInterface;
        
        usbInterface = MyCreateInterfaceClass(usbInterfaceRef);
        IOObjectRelease(usbInterfaceRef);
        
        if ((*usbInterface)->USBInterfaceOpen(usbInterface) == kIOReturnSuccess) {
            // Use the interface to iterate the endpoints
            error = MyListEndpoints(usbInterface);
            
            (*usbInterface)->USBInterfaceClose(usbInterface);
        }
    }
    IOObjectRelease(interfaceIterator);
    
    return kIOReturnSuccess;
}
IOUSBInterfaceInterface942 **MyCreateInterfaceClass(io_service_t usbInterfaceRef)
{
    SInt32 score;
    IOCFPlugInInterface **plugin;
    IOUSBInterfaceInterface942 **usbInterface = NULL;
    kern_return_t err;
    err = IOCreatePlugInInterfaceForService(usbInterfaceRef, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID, &plugin, &score);
    if(err == 0) {
        err = (*plugin)->QueryInterface(plugin,
                                                CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID942),
                                                (LPVOID)&usbInterface);
        IODestroyPlugInInterface(plugin);
    }
    return usbInterface;
}

IOReturn MyListEndpoints(IOUSBInterfaceInterface942 ** usbInterface)
{
    UInt8 numEndpoints;
    UInt8 i;
    IOReturn error;
    error = (*usbInterface)->GetNumEndpoints(usbInterface, &kUSBNumEndpoints);
    if(error != kIOReturnSuccess) {
        return error;
    }
    
    for (i = 1; i <= numEndpoints; i++) {
        UInt8 direction, number, transferType;
        UInt16 maxPacketSize;
        UInt8 interval;
        
        error = (*usbInterface)->GetPipeProperties(usbInterface, i, &direction, &number, &transferType, &maxPacketSize, &interval);
        if (error != kIOReturnSuccess) {
            return error;
        }
        
        // Print a description of the current endpoint
        if ((transferType == kUSBControl) && (direction == kUSBOut)) {
            printf("Pipe ref %d: Control OUT\n", i);
        }
        else if ((transferType == kUSBControl) && (direction == kUSBIn)) {
            printf("Pipe ref %d: Control IN\n", i);
        }
        else if ((transferType == kUSBIsoc) && (direction == kUSBOut)) {
            printf("Pipe ref %d: Isoc OUT\n", i);
        }
        else if ((transferType == kUSBIsoc) && (direction == kUSBIn)) {
            printf("Pipe ref %d: Isoc IN\n", i);
        }
        else if ((transferType == kUSBBulk) && (direction == kUSBOut)) {
            printf("Pipe ref %d: Bulk OUT\n", i);
        }
        else if ((transferType == kUSBBulk) && (direction == kUSBIn)) {
            printf("Pipe ref %d: Bulk IN\n", i);
        }
        else if ((transferType == kUSBInterrupt) && (direction == kUSBOut)) {
            printf("Pipe ref %d: Interrupt OUT\n", i);
        }
        else if ((transferType == kUSBInterrupt) && (direction == kUSBIn)) {
            printf("Pipe ref %d: Interrupt IN\n", i);
        }
    }
    
    return kIOReturnSuccess;
}
