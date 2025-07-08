#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"
#define get_arg(argv, argc) (argc > 0? shift(argv, argc) : "")
#define NO_COMMAND NULL

#define COMPILER "clang++"
#define COMMON_FLAGS "-O1"
#define WARNING_FLAGS "-Wall", "-Wextra", \
    "-Wno-unused-const-variable", "-Wno-writable-strings", \
    "-Wno-vla-cxx-extension"
#define DEBUG_FLAGS "-ggdb", "-O0"

#define SOURCE "source/main.cpp"
#define EXE "algraph.exe"

bool compile(bool in_debug) {
    Cmd cmd = {0};

    cmd_append(&cmd, COMPILER);

    nob_cc_output(&cmd, EXE);
    cmd_append(&cmd, SOURCE);

    cmd_append(&cmd, COMMON_FLAGS);
    cmd_append(&cmd, WARNING_FLAGS);
    if (in_debug) {
        cmd_append(&cmd, DEBUG_FLAGS);
    }

    return cmd_run_sync(cmd);
}

bool compile_and_run(bool in_debug) {
    if (!compile(in_debug)) return 1;

    Cmd cmd = {0};
    if (in_debug) {
        cmd_append(&cmd, "gdb");
        cmd_append(&cmd, "-ex", "b _assert");
        cmd_append(&cmd, "-ex", "handle SIGABRT stop nopass");
        cmd_append(&cmd, "-ex", "handle SIGQUIT stop nopass");
        cmd_append(&cmd, "-ex", "run", "--args");
    }
    cmd_append(&cmd, "./"EXE);
    return cmd_run_sync_and_reset(&cmd);
}

int help(char *program) {
    printf("\n");
    printf("Usage: %s [COMMAND]\n", program);
    printf("COMMAND:\n");
    printf("    help         show this message and exit\n");
    printf("    run          compile and run the program\n");
    printf("\n");
    printf("The default action is to just compile the program\n");
    return 0;
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    char *program = get_arg(argv, argc);
    char *target = get_arg(argv, argc);

    if (target == NO_COMMAND) {
        if (!compile(/*in_debug*/false)) return 1;
    } else if (strcmp(target, "run") == 0) {
        if (!compile_and_run(/*in_debug*/false)) return 1;
    } else if (strcmp(target, "debug") == 0) {
        if (!compile_and_run(/*in_debug*/true)) return 1;
    } else {
        return help(program);
    }

    return 0;
}
