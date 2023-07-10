#ifndef RAMDISK_HPP
#define RAMDISK_HPP
#include <IOKit/IOService.h>
#include <IOKit/IOBufferMemoryDescriptor.h>

#define RAMDisk com_bugratest_driver_RAMDisk

class RAMDisk : public IOService
{
    OSDeclareDefaultStructors(RAMDisk)
private:
    IOBufferMemoryDescriptor *m_memoryDesc;
    void *m_buffer;
protected:
    bool createBlockStorageDevice();
public:
    virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
    virtual void free(void) APPLE_KEXT_OVERRIDE;
    
    virtual IOByteCount performRead(IOMemoryDescriptor *dstDesc, UInt64 byteOffset, UInt64 byteCount);
    virtual IOByteCount performWrite(IOMemoryDescriptor *srcDesc, UInt64 byteOffset, UInt64 byteCount);
};
#endif
