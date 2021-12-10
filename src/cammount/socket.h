#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 512

class Connection{
    public:
        int s;
        char rcv_buffer[BUFFER_SIZE];
        struct sockaddr_in sin;
        struct hostent* dst;
        char buffer[30];
};