#ifndef HELLO_CONSOLE_HPP
#define HELLO_CONSOLE_HPP
#include <libkern/libkern.h>
#include <mach/mach_types.h>
#include <IOKit/IOLib.h>

kern_return_t HelloWorld_start(kmod_info_t *ki, void *d);

kern_return_t HellowWorld_stop(kmod_info_t *ki, void *d);

#endif
