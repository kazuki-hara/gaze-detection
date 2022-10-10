#include <string>
#include <netdb.h>

class Cammount{
public:
    Cammount(std::string, int);
    ~Cammount(void);
    bool check_connection(void);
    void connection(void);
    std::string gaze_to_command(double, double, double, double);
    int send_command(void);

private:
    std::string hostname;
    struct sockaddr_in sin;
    int port;
    struct hostent* dst;
    int s;
    int pp, ps, tp, ts;
};