//
//  MyInterfaceFilter.c
//  MyInterfaceFilter
//
//  Created by Bugra Karabudak on 8.07.2023.
//

#include <libkern/libkern.h>
#include <sys/errno.h>
#include <sys/kpi_mbuf.h>
#include <mach/mach_types.h>
#include <net/kpi_interfacefilter.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>

static boolean_t g_filter_registered = TRUE;
static boolean_t g_filter_detached = FALSE;
static interface_filter_t g_filter_ref;

static errno_t
myif_filter_input(void *cookie, ifnet_t interface, protocol_family_t protocol, mbuf_t *data, char **frame_ptr)
{
    printf("incoming packet: %lu bytes\n", mbuf_pkthdr_len(*data));
    return 0;
}

static errno_t
myif_filter_output(void *cookie, ifnet_t interface, protocol_family_t protocol, mbuf_t *data)
{
    printf("outgoing packet: %lu bytes\n", mbuf_pkthdr_len(*data));
    return 0;
}
static void
myif_filter_detached(void *cookie, ifnet_t interface)
{
    g_filter_detached = TRUE;
}
static struct iff_filter g_my_iff_filter = {
    NULL,
    "com.bugratest.MyInterfaceFilter",
    0,
    myif_filter_input,
    myif_filter_output,
    NULL,
    NULL,
    myif_filter_detached,
};
kern_return_t
MyInterfaceFilter_start(kmod_info_t *ki, void *d)
{
    ifnet_t interface;
    
    // change to your own interface
    if (ifnet_find_by_name("en1", &interface) != KERN_SUCCESS) {
        return KERN_FAILURE;
    }
    
    if (iflt_attach(interface, &g_my_iff_filter, &g_filter_ref) == KERN_SUCCESS) {
        g_filter_registered = TRUE;
    }
    
    ifnet_release(interface);
    
    return KERN_SUCCESS;
}


kern_return_t
MyInterfaceFilter_stop(kmod_info_t *ki, void *d)
{
    if (g_filter_registered) {
        iflt_detach(g_filter_ref);
        g_filter_registered = FALSE;
    }
    
    // Don't allow unload until filter is detached.
    if (!g_filter_detached) {
        return KERN_NO_ACCESS;
    }
    
    return KERN_SUCCESS;
}
