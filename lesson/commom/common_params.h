#pragma once

#include <getopt.h>
#include "common_defines.h"

struct option_wrapper {
    struct option option;
    char *help;
    char *metaver;
    bool required;
};

void usage(const char *prog_name, const char *doc, const struct option_wrapper *long_options, bool full);

void parse_cmdline_args(int argc, char **argv, const struct option_wrapper *long_options, struct config *cfg, const char *doc);