#include <string>
#include <netdb.h>
#include <tuple>

class Cammount{
public:
    Cammount(std::string, int);
    ~Cammount(void);
    bool check_connection(void);
    void connection(void);
    std::string gaze_to_command(std::tuple<double, double, double, double>);
    std::string position_to_command(void);
    int send_command(void);
    std::tuple <int, int> gaze_to_step(std::tuple<double, double, double, double>);

private:
    std::string hostname;
    struct sockaddr_in sin;
    int port;
    struct hostent* dst;
    int s;
    int pp, ps, tp, ts; // コマンド送信用変数
    int pp_diff, tp_diff; // 目標ステップ位置までの差分ステップ数
    int pp_dist, tp_dist; // 目標ステップ位置
    int pp_p, pp_i, pp_d, tp_p, tp_i, tp_d; // PID制御用変数
};