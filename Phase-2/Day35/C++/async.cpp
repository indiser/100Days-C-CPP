#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

constexpr int PORT = 8080;
constexpr int MAX_EVENTS = 64;
constexpr size_t BUF_SIZE = 1024;

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

class EventHandler {
public:
    virtual ~EventHandler() = default;
    virtual int get_fd() const = 0;
    virtual void handle_read() = 0;
    virtual void handle_write() = 0;
    virtual void handle_error() = 0;
};

class Reactor {
private:
    int epoll_fd_;
    std::unordered_map<int, std::shared_ptr<EventHandler>> handlers_;

public:
    Reactor() {
        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ < 0) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }
    }

    ~Reactor() {
        if (epoll_fd_ >= 0) close(epoll_fd_);
    }

    void register_handler(std::shared_ptr<EventHandler> handler, uint32_t events) {
        int fd = handler->get_fd();
        handlers_[fd] = handler;

        struct epoll_event ev{};
        ev.events = events;
        ev.data.fd = fd;

        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
            perror("epoll_ctl ADD");
            handlers_.erase(fd);
        }
    }

    void modify_handler(int fd, uint32_t events) {
        struct epoll_event ev{};
        ev.events = events;
        ev.data.fd = fd;

        if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
            perror("epoll_ctl MOD");
        }
    }

    void remove_handler(int fd) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
        handlers_.erase(fd);
    }

    void run() {
        std::vector<struct epoll_event> events(MAX_EVENTS);
        while (true) {
            int nfds = epoll_wait(epoll_fd_, events.data(), MAX_EVENTS, -1);
            if (nfds < 0) {
                if (errno == EINTR) continue;
                perror("epoll_wait");
                break;
            }

            for (int i = 0; i < nfds; ++i) {
                int fd = events[i].data.fd;
                uint32_t revents = events[i].events;

                auto it = handlers_.find(fd);
                if (it == handlers_.end()) continue;

                auto handler = it->second;

                if (revents & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                    handler->handle_error();
                    continue;
                }

                if (revents & EPOLLIN) {
                    handler->handle_read();
                }

            // Recheck map: handle_read() might have removed handler
                if ((revents & EPOLLOUT) && handlers_.find(fd) != handlers_.end()) {
                    handler->handle_write();
                }
            }
        }
    }
};

class ClientHandler : public EventHandler {
private:
    int fd_;
    Reactor& reactor_;
    std::string write_buf_;
    size_t write_pos_{0};

public:
    ClientHandler(int fd, Reactor& reactor) : fd_(fd), reactor_(reactor) {}

    ~ClientHandler() override {
        if (fd_ >= 0) close(fd_);
    }

    int get_fd() const override { return fd_; }

    void handle_read() override {
        char buf[BUF_SIZE];
        while (true) {
            ssize_t n = read(fd_, buf, sizeof(buf));
            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                handle_error();
                return;
            }
            if (n == 0) {
                handle_error();
                return;
            }
            std::cout.write(buf, n);
        }

        write_buf_ = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello World!\n";
        write_pos_ = 0;
        reactor_.modify_handler(fd_, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP);
    }

    void handle_write() override {
        while (write_pos_ < write_buf_.size()) {
            ssize_t w = write(fd_, write_buf_.data() + write_pos_, write_buf_.size() - write_pos_);
            if (w < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                handle_error();
                return;
            }
            write_pos_ += w;
        }

        if (write_pos_ == write_buf_.size()) {
            reactor_.remove_handler(fd_);
        }
    }

    void handle_error() override {
        reactor_.remove_handler(fd_);
    }
};

class AcceptHandler : public EventHandler {
private:
    int listen_fd_;
    Reactor& reactor_;

public:
    AcceptHandler(int port, Reactor& reactor) : reactor_(reactor) {
        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd_ < 0) { perror("socket"); exit(EXIT_FAILURE); }

        int opt = 1;
        setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(listen_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind"); exit(EXIT_FAILURE);
        }
        if (set_nonblocking(listen_fd_) < 0) {
            perror("set_nonblocking"); exit(EXIT_FAILURE);
        }
        if (listen(listen_fd_, SOMAXCONN) < 0) {
            perror("listen"); exit(EXIT_FAILURE);
        }
    }

    ~AcceptHandler() override {
        if (listen_fd_ >= 0) close(listen_fd_);
    }

    int get_fd() const override { return listen_fd_; }

    void handle_read() override {
        while (true) {
            struct sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int conn_fd = accept(listen_fd_, (struct sockaddr*)&client_addr, &client_len);

            if (conn_fd < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                perror("accept");
                break;
            }

            if (set_nonblocking(conn_fd) < 0) {
                close(conn_fd);
                continue;
            }

            auto client = std::make_shared<ClientHandler>(conn_fd, reactor_);
            reactor_.register_handler(client, EPOLLIN | EPOLLET | EPOLLRDHUP);
        }
    }

    void handle_write() override {}
    void handle_error() override {}
};

int main() {
    Reactor reactor;
    auto acceptor = std::make_shared<AcceptHandler>(PORT, reactor);
    reactor.register_handler(acceptor, EPOLLIN | EPOLLET);
    
    std::cout << "Reactor loop running on port " << PORT << "...\n";
    reactor.run();

    return 0;
}