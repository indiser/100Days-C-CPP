#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_EVENTS 64
#define BUF_SIZE 1024

struct conn_state {
    int fd;
    char write_buf[BUF_SIZE];
    size_t write_len;
    size_t write_pos;
};

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static void close_conn(int epoll_fd, struct conn_state *st) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, st->fd, NULL);
    close(st->fd);
    free(st);
}

int main(void) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(listen_fd); exit(EXIT_FAILURE);
    }
    if (set_nonblocking(listen_fd) < 0) {
        perror("set_nonblocking"); close(listen_fd); exit(EXIT_FAILURE);
    }
    if (listen(listen_fd, SOMAXCONN) < 0) {
        perror("listen"); close(listen_fd); exit(EXIT_FAILURE);
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) { perror("epoll_create1"); close(listen_fd); exit(EXIT_FAILURE); }

    struct epoll_event ev_listen = {
        .events = EPOLLIN | EPOLLET,
        .data.ptr = NULL
    };
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev_listen) < 0) {
        perror("epoll_ctl listen_fd"); exit(EXIT_FAILURE);
    }

    struct epoll_event events[MAX_EVENTS];
    char read_buf[BUF_SIZE];

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            struct conn_state *st = (struct conn_state *)events[i].data.ptr;

            if (st == NULL) {
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
                    if (conn_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept");
                        break;
                    }

                    if (set_nonblocking(conn_fd) < 0) {
                        close(conn_fd);
                        continue;
                    }

                    struct conn_state *new_st = calloc(1, sizeof(struct conn_state));
                    new_st->fd = conn_fd;

                    struct epoll_event ev_conn = {
                        .events = EPOLLIN | EPOLLET | EPOLLRDHUP,
                        .data.ptr = new_st
                    };
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev_conn) < 0) {
                        perror("epoll_ctl conn_fd");
                        close(conn_fd);
                        free(new_st);
                    }
                }
            } else {
                uint32_t evs = events[i].events;

                if (evs & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                    close_conn(epoll_fd, st);
                    continue;
                }

                if (evs & EPOLLIN) {
                    int closed = 0;
                    while (1) {
                        ssize_t n = read(st->fd, read_buf, sizeof(read_buf));
                        if (n < 0) {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                            closed = 1;
                            break;
                        }
                        if (n == 0) {
                            closed = 1;
                            break;
                        }
                        (void)write(STDOUT_FILENO, read_buf, n);
                    }

                    if (closed) {
                        close_conn(epoll_fd, st);
                        continue;
                    }

                    const char *resp = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello World!\n";
                    st->write_len = strlen(resp);
                    memcpy(st->write_buf, resp, st->write_len);
                    st->write_pos = 0;

                    struct epoll_event ev_mod = {
                        .events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP,
                        .data.ptr = st
                    };
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, st->fd, &ev_mod);
                }

                if (evs & EPOLLOUT) {
                    int error = 0;
                    while (st->write_pos < st->write_len) {
                        ssize_t w = write(st->fd, st->write_buf + st->write_pos, st->write_len - st->write_pos);
                        if (w < 0) {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                            error = 1;
                            break;
                        }
                        st->write_pos += w;
                    }

                    if (error) {
                        close_conn(epoll_fd, st);
                    } else if (st->write_pos == st->write_len) {
                        close_conn(epoll_fd, st);
                    }
                }
            }
        }
    }

    close(listen_fd);
    close(epoll_fd);
    return 0;
}