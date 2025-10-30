
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        return 1;
    }

    char cmd[BUF_SIZE];
    if (strcmp(argv[1], "SET") == 0)
        snprintf(cmd, BUF_SIZE, "SET %s %s", argv[2], argv[3]);
    else
        snprintf(cmd, BUF_SIZE, "GET %s", argv[2]);

    write(fd, cmd, strlen(cmd));

    char buf[BUF_SIZE];
    int n = read(fd, buf, BUF_SIZE - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("%s", buf);
    }

    close(fd);
}
