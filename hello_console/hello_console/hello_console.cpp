#include "hello_console.hpp"

kern_return_t HelloWorld_start(kmod_info_t *ki, void *d)
{
    printf("Hello_cosole::İnitializing");
    printf("First Driver:)");
    return KERN_SUCCESS;
}

kern_return_t HellowWorld_stop(kmod_info_t *ki, void *d)
{
    printf("Hello_cosole::Stoping");
    printf(":(");
    return KERN_SUCCESS;
}
