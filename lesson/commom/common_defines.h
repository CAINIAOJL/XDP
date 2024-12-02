#pragma once

#include <net/if.h>
#include <linux/types.h>
#include <stdbool.h>
#include <xdp/libxdp.h>

#define DIR_FILENAME_LEN 512
#define PROG_NAME_LEN 32
#define MAC_ADDR_LEN 18


struct config {
    enum xdp_attach_mode attach_mode;
    __u32 xdp_flags;

    int ifindex;
    char *ifname;
    char ifname_buf[IF_NAMESIZE];
    int redirect_ifindex;
    char *redirct_ifname;
    char redirct_ifname_buf[IF_NAMESIZE];

    bool do_unload;

    __u32 prog_id;
    bool reuse_maps;

    char pin_dir[DIR_FILENAME_LEN];
    char filename[DIR_FILENAME_LEN];
    char progname[PROG_NAME_LEN];
    char src_mac[MAC_ADDR_LEN];
    char dst_mac[MAC_ADDR_LEN];

    __u16 xsk_bind_flags;
    int xsk_if_queue;
    bool xsk_poll_mode;
    bool unload_all;
};

extern int verbose;

#define EXIT_OK             0
#define EXIT_FAIL           1
#define EXIT_FAIL_OPTION    2
#define EXIT_FAIL_XDP       3
#define EXIT_FAIL_BPF       4