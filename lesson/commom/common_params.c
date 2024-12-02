#include "common_params.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h> //分析命令行参数的头文件 <argp.h>
#include <errno.h>

#include <net/if.h>
#include <linux/if_link.h>
#include <linux/if_xdp.h>

int verbose = 1;

#define BUF_SIZE 30

void __print_options(const struct option_wrapper *long_options, bool rquired) {
    int i, pos;
    char buf[BUF_SIZE];

    for(i = 0; long_options[i].option.name != 0; i++) {
        if (long_options[i].required != rquired) {
            continue;
        }

        if (long_options[i].option.val > 64) {
            printf("  -%c,", long_options[i].option.val);
        } else {
            printf("    ");
        }

        pos = snprintf(buf, BUF_SIZE, " --%s", long_options[i].option.name);
        if(long_options[i].metaver) {
            snprintf(&buf[pos], BUF_SIZE - pos, " %s", long_options[i].metaver);
        }
        printf("%-22s", buf);
        printf("  %s", long_options[i].help);
        printf("\n");
    }
}

void usage(const char *prog_name, const char *doc, const struct option_wrapper *long_options, bool full) {
    printf("Usage: %s [options]\n", prog_name);

    if(!full) {
        printf("Use --help (or -h) to see full options.\n");
        return;
    }

    printf("\nDOCUMENTATION:\n%s\n", doc);
    printf("Required options:\n");
    __print_options(long_options, true);
    printf("\n");
    printf("Other options:\n");
    __print_options(long_options, false);
    printf("\n");
}

int option_wrapper_to_options(const struct option_wrapper *wrapper, struct option **options) {
    int i, num;
    struct option *new_options;
    for(i = 0; wrapper[i].option.name != 0; i++) {

    }
    num = i;
    new_options = malloc(sizeof(struct option) * num);
    if(!new_options) {
        return -1;
    }

    for(i = 0; i < num; i++) {
        memcpy(&new_options[i], &wrapper[i], sizeof(struct option));
    }

    *options = new_options;
    return 0;
}

void parse_cmdline_args(int argc, char **argv, const struct option_wrapper *option_wrapper, struct config *cfg, const char *doc) {
    struct option *long_options;
    bool full_help = false;
    int long_index = 0;
    char *dest;
    int opt;

    if(option_wrapper_to_options(option_wrapper, &long_options)) {
        fprintf(stderr, "Failed to convert option wrapper to options.\n");
        exit(EXIT_FAIL_OPTION);
    }

    while((opt = getopt_long(argc, argv, "hd:r:L:R:ASNFU:MQ:czpq", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'd':
                if(strlen(optarg) >= IF_NAMESIZE) {
                   fprintf(stderr, "ERR: --dev name too long\n");
				    goto error;
                }
                cfg->ifname = (char *)&cfg->ifname_buf;
                strncpy(cfg->ifname, optarg, IF_NAMESIZE);
                cfg->ifindex = if_nametoindex(cfg->ifname);
                if(cfg->ifindex == 0) {
                    fprintf(stderr,"ERR: --dev name unknown err(%d):%s\n",errno, strerror(errno));
				    goto error;
                }
                break;
            case 'r':
                if(strlen(optarg) >= IF_NAMESIZE) {
                    fprintf(stderr, "ERR: --redirect-dev name too long\n");
				    goto error;
                } 
                cfg->redirct_ifname = (char *)&cfg->redirct_ifname_buf;
                strncpy(cfg->redirct_ifname, optarg, IF_NAMESIZE);
                cfg->redirect_ifindex = if_nametoindex(cfg->redirct_ifname);
                if(cfg->redirect_ifindex == 0) {
                    fprintf(stderr,"ERR: --redirect-dev name unknown err(%d):%s\n",errno, strerror(errno));
				    goto error;
                }
                break;
            case 'A':
                cfg->attach_mode = XDP_MODE_UNSPEC;;
                break;
            case 'S':
                cfg->attach_mode = XDP_MODE_SKB;
                cfg->xsk_bind_flags &= ~XDP_ZEROCOPY;
                cfg->xsk_bind_flags |= XDP_COPY;
                break;
            case 'N':
                cfg->attach_mode = XDP_MODE_NATIVE;
                break;
            case 3:
                cfg->attach_mode = XDP_MODE_HW;
                break;
            case 'M':
                cfg->reuse_maps = true;
                break;
            case 'U':
                cfg->do_unload = true;
                cfg->prog_id = atoi(optarg);
                break;
            case 'P':
                cfg->xsk_poll_mode = true;
                break;
            case 'Q':
                cfg->xsk_if_queue = atoi(optarg);
                break;
            case 1:
                dest = (char *)&cfg->filename;
                strncpy(dest, optarg, sizeof(cfg->filename));
                break;
            case 2:
                dest = (char *)&cfg->progname;
                strncpy(dest, optarg, sizeof(cfg->progname));
                break;
            case 'L': /* --src-mac */
			    dest  = (char *)&cfg->src_mac;
			    strncpy(dest, optarg, sizeof(cfg->src_mac));
			    break;
		    case 'R': /* --dest-mac */
			    dest  = (char *)&cfg->dst_mac;
			    strncpy(dest, optarg, sizeof(cfg->dst_mac));
			    break;
		    case 'c':
			    cfg->xsk_bind_flags &= ~XDP_ZEROCOPY;
			    cfg->xsk_bind_flags |= XDP_COPY;
			    break;
		    case 'z':
			    cfg->xsk_bind_flags &= ~XDP_COPY;
			    cfg->xsk_bind_flags |= XDP_ZEROCOPY;
			    break;
		    case 4: /* --unload-all */
			    cfg->unload_all = true;
			    break;
		    case 'h':
			    full_help = true;
			/* fall-through */
		    error:
            default:
                usage(argv[0], doc, long_options, full_help);
                free(long_options);
                exit(EXIT_FAIL_OPTION);
        }
    }
    free(long_options);
}
