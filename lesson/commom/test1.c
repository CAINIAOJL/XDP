#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include "common_params.h"

jmp_buf env;

void test_option_wrapper_to_options() {
    struct option_wrapper wrapper[] = {
        {{"dev", 'd', 0, 0}, "Device name", "IFNAME", true},
        {{"redirect-dev", 'r', 0, 0}, "Redirect device name", "IFNAME", false},
        {{"help", 'h', 0, 0}, "Show help", NULL, false},
        { {0, 0, 0, 0} }  // terminator
    };
    
    struct option *options;
    int result = option_wrapper_to_options(wrapper, &options);
    
    assert(result == 0);
    assert(options[0].name != NULL);
    assert(strcmp(options[0].name, "dev") == 0);
    assert(options[1].name != NULL);
    assert(strcmp(options[1].name, "redirect-dev") == 0);
    printf("Option wrapper to options test passed successfully!\n");
    
    free(options);
}

void test_usage() {
    const char *prog_name = "test_program";
    const char *doc = "Test documentation";
    struct option_wrapper wrapper[] = {
        {{"dev", 'd', 0, 0}, "Device name", "IFNAME", true},
        { {0, 0, 0, 0} }  // terminator
    };
    
    // Testing usage output
    usage(prog_name, doc, wrapper, true);
    printf("Usage output test passed successfully!\n");
}

void test_parse_cmdline_args() {
    struct config cfg;
    char *argv[] = {
        "test_program",
        "--dev", "eth0",
        "--redirect-dev", "eth1",
        "-M",
        "arg1", "arg2"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    struct option_wrapper wrapper[] = {
        {{"dev", 'd', 0, 0}, "Device name", "IFNAME", true},
        {{"redirect-dev", 'r', 0, 0}, "Redirect device name", "IFNAME", false},
        {{"reuse-maps", 'M', 0, 0}, "Reuse maps", NULL, false},
        { {0, 0, 0, 0} }  // terminator
    };
    
    // should not crash, can further test cfg values after execution
    parse_cmdline_args(argc, argv, wrapper, &cfg, "Test documentation");
    printf("Parse cmdline args test passed successfully!\n");
}

void test_invalid_option() {
    struct config cfg;
    char *argv[] = {
        "test_program",
        "--invalid-option"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    struct option_wrapper wrapper[] = {
        {{"dev", 'd', 0, 0}, "Device name", "IFNAME", true},
        { {0, 0, 0, 0} }  // terminator
    };
    
    // Capture output and check for expected behavior
    int exit_status = setjmp(env);
    parse_cmdline_args(argc, argv, wrapper, &cfg, "Test documentation");
    assert(exit_status != 0);
    assert(errno == EINVAL);
    printf("Invalid option test passed successfully!\n");
}

int main() {
    test_option_wrapper_to_options();
    test_usage();
    test_parse_cmdline_args();
    test_invalid_option();
    
    printf("All tests passed successfully!\n");
    return 0;
}