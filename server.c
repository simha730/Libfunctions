#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#include "kvstore.h"

#define SOCKET_PATH "/tmp/kvstore.sock"
#define BACKLOG 10
#define BUF_SIZE 256

static void die(const char *msg)
{
    perror(msg);
    unlink(SOCKET_PATH);
    exit(EXIT_FAILURE);
}

int main(void)
{
    /* --------------------- Load dynamic library --------------------- */
    void *lib = dlopen("./libkvstore.so", RTLD_LAZY);
    if (!lib) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(1);
    }

    const char *(*kv_get_ptr)(const char *) = dlsym(lib, "kv_get");
    void (*kv_set_ptr)(const char *, const char *) = dlsym(lib, "kv_set");

    if (!kv_get_ptr || !kv_set_ptr) {
        fprintf(stderr, "dlsym error: %s\n", dlerror());
        exit(1);
    }

    /* --------------------- Set up UNIX socket --------------------- */
    int listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd == -1)
        die("socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        die("bind");

    if (listen(listen_fd, BACKLOG) == -1)
        die("listen");

    printf("KV Store server running (dlopen) on %s\n", SOCKET_PATH);

    while (1) {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd == -1)
            continue;

        char buf[BUF_SIZE];
        ssize_t n = read(client_fd, buf, sizeof(buf) - 1);

        if (n > 0) {
            buf[n] = '\0';
            char key[BUF_SIZE], value[BUF_SIZE];

            if (sscanf(buf, "SET %s %[^\n]", key, value) == 2) {
                kv_set_ptr(key, value);
                write(client_fd, "OK\n", 3);
            } else if (sscanf(buf, "GET %s", key) == 1) {
                const char *val = kv_get_ptr(key);
                if (val)
                    dprintf(client_fd, "%s\n", val);
                else
                    write(client_fd, "NOT_FOUND\n", 10);
            } else {
                write(client_fd, "ERROR\n", 6);
            }
        }

        close(client_fd);
    }

    dlclose(lib);
    return 0;
}

