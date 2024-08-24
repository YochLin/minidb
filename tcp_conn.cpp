#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

#include <tcp_conn.h>

const int SERVER_LISTEN_CONNECTIONS = 512;
const int MAX_EVENTS = 10;

TCPConn::TCPConn()
{
}

TCPConn::~TCPConn()
{
}

int TCPConn::start()
{
  struct epoll_event ev, events[MAX_EVENTS];
  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    return -1;
  }

  ev.data.fd = fd_;
  ev.events = EPOLLIN;

  int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_, &ev);
  if (ret == -1) {
    return -1;
  }

  while (true) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); 
    if (nfds == -1) {
      return -1;
    }

    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == fd_) {
        this->accept(epoll_fd);
      }
    }
  }
}

int TCPConn::do_tcp_connect()
{
    int ret = 0;
    struct sockaddr_in addr;

    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
      return -1;
    }

    ret = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &ret, sizeof(ret)); 
    if (ret < 0) {
      return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    ret = ::bind(fd_, (struct sockaddr *)&addr, sizeof(addr));
    if (ret) {
      return -1;
    }

    ret = ::listen(fd_, SERVER_LISTEN_CONNECTIONS);
    if (ret < 0) {
      return -1;
    }

    return 0;
}

int TCPConn::accept(int epoll_fd)
{
  int ret = 0;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  struct epoll_event ev;

  int client_fd = ::accept(fd_, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd < 0) {
    return -1;
  }

  set_non_block(client_fd);
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = client_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
      return -1;
  }

  std::cout << "New Connection: " << client_fd << std::endl;

  return 0;
}

int TCPConn::set_non_block(int fd)
{
  int ret = 0;
  ret = fcntl(fd, F_GETFL, 0);
  if (ret < 0) {
    return -1;
  }
  ret = fcntl(fd, F_SETFL, ret | O_NONBLOCK);
  if (ret < 0) {
    return -1;
  }
  return 0;
}

