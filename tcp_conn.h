
class TCPConn 
{
public:
    TCPConn(/* args */);
    ~TCPConn();

    int start();
    int do_tcp_connect();
private:
    int accept(int epoll_fd);
    int set_non_block(int fd);

    int fd_;
};