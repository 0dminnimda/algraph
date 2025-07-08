#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"
#define get_arg(argv, argc) (argc > 0? shift(argv, argc) : "")
#define NO_COMMAND NULL

#define COMPILER "clang++"
#define COMMON_FLAGS "-O1", "-Wno-unused-const-variable", "-Wno-writable-strings", "-Wno-vla-cxx-extension"
#define WARNING_FLAGS "-Wall", "-Wextra"

#define SOURCE "source/main.cpp"
#define EXE "algraph.exe"

#ifdef _WIN32
#define EXE_PREFIX
#else
#define EXE_PREFIX "./"
#endif

bool compile() {
    Cmd cmd = {0};

    cmd_append(&cmd, COMPILER);

    nob_cc_output(&cmd, EXE);
    cmd_append(&cmd, SOURCE);

    cmd_append(&cmd, WARNING_FLAGS);
    cmd_append(&cmd, COMMON_FLAGS);

    return cmd_run_sync(cmd);
}

bool compile_and_run() {
    if (!compile()) return 1;

    Cmd cmd = {0};
    cmd_append(&cmd, EXE_PREFIX EXE);
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
        if (!compile()) return 1;
    } else if (strcmp(target, "run") == 0) {
        if (!compile_and_run()) return 1;
    } else if (strcmp(target, "debug") == 0) {
        if (!compile_and_run()) return 1;
    } else {
        return help(program);
    }

    return 0;
}
