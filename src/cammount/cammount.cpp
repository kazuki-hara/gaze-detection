#include <stdlib.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "./cammount.h"
#include "./../main.h"
#include "./../utils/utils.h"


#define BUFFER_SIZE 512

#define L_SPEED 15

#define PAN_POSITION_MAX 12401
#define PAN_POSITION_MIN -12402
#define TILTE_POSITION_MAX 2367
#define TILTE_POSITION_MIN -3630

#define STEPS_PER_SECOND 46.2857

#define CAMERA_VIEW_ANGLE 78
#define FOVE_VEIW_ANGLE 100

double kp = 1.0; // 比例ゲイン

double kp_pos = 0.5;
double kd_pos = 0.2;
double ki_pos = 0.1;
double seconds_arc_per_position = 46.2857; // 1ステップあたりの角度


std::tuple<int, int> get_position_from_rcv_buffer(std::string rcv_buffer){
    std::string s;
    std::replace(rcv_buffer.begin(), rcv_buffer.end(), '\n', ' ');
    std::vector<std::string> split_buffer;
    std::stringstream ss_buffer(rcv_buffer);
    while(getline(ss_buffer, s, ' ')){
        split_buffer.push_back(s);
    }
    //for (const std::string& s : split_buffer){         // vの中身を出力
    //    std::cout << s << std::endl;
    //}
    int pp = NULL, tp = NULL;
    for(int i=0; i<split_buffer.size(); i++){
        if(check_int(split_buffer[i])){
            if(pp == NULL) pp = std::stoi(split_buffer[i]);
            else if(tp == NULL){
                tp = std::stoi(split_buffer[i]);
                return std::make_tuple(pp, tp);
            }
        }
    }
    pp = 0;
    tp = 0;
    return std::make_tuple(pp, tp);
}



Cammount::Cammount(std::string input_hostname, int input_port){
    hostname = input_hostname;
    port = input_port;
    pp = 0;
    tp = 0;
    pp_diff = 0;
    tp_diff = 0;
    pp_dist = 0;
    tp_dist = 0;
    pp_p = 0;
    pp_i = 0;
    pp_d = 0;
    tp_p = 0;
    tp_i = 0;
    tp_d = 0;
}

Cammount::~Cammount(void){}




bool Cammount::check_connection(void){
    bool connection_flag = true;
    // 繋がらねえよ
	if((s=socket(PF_INET, SOCK_STREAM, 0))==-1){
		std::perror("Can't create a socket.\n");
		connection_flag = false;
	}
    // 繋がらねえよ
    if(!(dst=gethostbyname(hostname.c_str()))){
		fprintf(stderr, "Can't resolve the name.\n");
		connection_flag = false;
	}
    return connection_flag;
}

void Cammount::connection(void){
    if(check_connection() == false) return;
    sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=*((in_addr_t *)*dst->h_addr_list);
	sin.sin_port=htons(port);

    char rcv_buffer[BUFFER_SIZE];
    connect(s, (const struct sockaddr *)&sin, sizeof(sin));
    recv(s, rcv_buffer, BUFFER_SIZE, 0);

    return;
}

// 視線方向が画面中央になるまでの必要ステップ数
std::tuple <int, int> Cammount::gaze_to_step(std::tuple<double, double, double, double> gaze_data){
    double lx = std::get<0>(gaze_data);
    double ly = std::get<1>(gaze_data);
    double rx = std::get<2>(gaze_data);
    double ry = std::get<3>(gaze_data);

    double mean_x = (lx + rx) / 2;
    double mean_y = (ly + ry) / 2;

    double theta_x = std::atan(std::tan(50 * M_PI / 180) * abs(mean_x) / 1280);
    double theta_y = std::atan(std::tan(50 * M_PI / 180) * abs(mean_y) / 1280);

    int step_x = (int)(theta_x * 180 / M_PI / (seconds_arc_per_position / 3600));
    int step_y = (int)(theta_y * 180 / M_PI / (seconds_arc_per_position / 3600));

    return std::make_tuple(step_x, step_y);
}

// 視線方向から雲台制御コマンドを生成
std::string Cammount::gaze_to_command(std::tuple<double, double, double, double> gaze_data){
    double lx = std::get<0>(gaze_data);
    double ly = std::get<1>(gaze_data);
    double rx = std::get<2>(gaze_data);
    double ry = std::get<3>(gaze_data);

    // lx,ly,rx,ry -> pp,ps,tp,tsをここに実装
    double mean_x = (lx + rx) / 2;
    double mean_y = (ly + ry) / 2;

    // 移動する方向を指定、目標方向までのステップ数
    std::tuple<int, int> steps = gaze_to_step(gaze_data); 
    int p_steps= std::get<0>(steps);
    int t_steps = std::get<1>(steps); 
    if(mean_x >= 0){
        pp = PAN_POSITION_MIN;
        pp_diff = -1 * p_steps;
    }else{
        pp = PAN_POSITION_MAX;
        pp_diff = p_steps;
    }
    if(mean_y >= 0){
        tp = TILTE_POSITION_MAX;
        tp_diff = t_steps;
    }else{
        tp = TILTE_POSITION_MIN;
        tp_diff = -1 * t_steps;
    }

    // 移動する速度を指定 (kp: 比例ゲイン)
    ps = p_steps * kp;
    ts = t_steps * kp;

    std::string ps_str = std::to_string(ps);
    std::string pp_str = std::to_string(pp);
    std::string ts_str = std::to_string(ts);
    std::string tp_str = std::to_string(tp);

    char buffer[] = {'p','p', ' ', 't', 'p', ' ', 'p','s', ' ', ' ', ' ', ' ', ' ', 'p', 'p', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 't', 's', ' ', ' ', ' ', ' ', ' ', 't', 'p', ' ', ' ', ' ', ' ', ' ','\n'};
    for (unsigned int i = 0; i < ps_str.size(); i++) buffer[i + 8] = ps_str[i];
    for (unsigned int i = 0; i < pp_str.size(); i++) buffer[i + 15] = pp_str[i];
    for (unsigned int i = 0; i < ts_str.size(); i++) buffer[i + 24] = ts_str[i];
    for (unsigned int i = 0; i < tp_str.size(); i++) buffer[i + 31] = tp_str[i];

    return buffer;
}

// 視線検出エラー時に、最後に検出に成功した視線方向を元にPID制御するコマンドを生成
std::string Cammount::position_to_command(void){
    pp = pp_dist;
    tp = tp_dist;

    ps = (int)abs((pp_p * kp_pos + pp_i * ki_pos + pp_d * kd_pos));
    ts = (int)abs((tp_p * kp_pos + tp_i * ki_pos + tp_d * kd_pos));

    std::string ps_str = std::to_string(ps);
    std::string pp_str = std::to_string(pp);
    std::string ts_str = std::to_string(ts);
    std::string tp_str = std::to_string(tp);

    char buffer[] = {'p', 'p', ' ', 't', 'p', ' ', 'p','s', ' ', ' ', ' ', ' ', ' ', 'p', 'p', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 't', 's', ' ', ' ', ' ', ' ', ' ', 't', 'p', ' ', ' ', ' ', ' ', ' ','\n'};
    for (unsigned int i = 0; i < ps_str.size(); i++) buffer[i + 8] = ps_str[i];
    for (unsigned int i = 0; i < pp_str.size(); i++) buffer[i + 15] = pp_str[i];
    for (unsigned int i = 0; i < ts_str.size(); i++) buffer[i + 24] = ts_str[i];
    for (unsigned int i = 0; i < tp_str.size(); i++) buffer[i + 31] = tp_str[i];

    return buffer;
}

int Cammount::send_command(void){
    char rcv_buffer[BUFFER_SIZE];
    while(true)
    {
        if(check_exit_flag()){
            close(s);
            break;
        }
        
        std::tuple<double, double, double, double> gaze_data = get_gaze_pixel();

        std::string buffer_str;
        
        // 注視点推定成功時
        if(check_detect_pupil_flag() && check_gaze_in_disp_flag()){
            // PID制御をリセット
            pp_p = 0;
            pp_i = 0;
            pp_d = 0;
            tp_p = 0;
            tp_i = 0;
            tp_d = 0;

            // 制御コマンドの生成 -> 送信
            buffer_str = gaze_to_command(gaze_data);
            const char* buffer = buffer_str.c_str();
            if(sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&sin, sizeof(sin))==-1){
                fprintf(stderr, "Can't sendto from the socket.\n");
                close(s);
                return -1;
            }

            // recvBufferから現在ステップ位置を取得 -> 目標ステップ位置を計算 (次フレームで瞳孔検出エラー時にPID制御するために使用)
            recv(s, rcv_buffer, BUFFER_SIZE, 0);
            std::string string_rcv_buffer = std::string(rcv_buffer);
            std::tuple<int, int> cammount_pos = get_position_from_rcv_buffer(string_rcv_buffer);
            int pp_tmp = std::get<0>(cammount_pos);
            int tp_tmp = std::get<1>(cammount_pos);
            if(pp_tmp != 0 && tp_tmp != 0){
                pp_dist = pp_tmp + pp_diff;
                tp_dist = tp_tmp + tp_diff; 
            }
        }
        else{
            buffer_str = position_to_command();
            const char* buffer = buffer_str.c_str();

            if(sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&sin, sizeof(sin))==-1){
                fprintf(stderr, "Can't sendto from the socket.\n");
                close(s);
                return -1;
            }
            
            recv(s, rcv_buffer, BUFFER_SIZE, 0);
            std::string string_rcv_buffer = std::string(rcv_buffer);
            std::tuple<int, int> cammount_pos = get_position_from_rcv_buffer(string_rcv_buffer);
            int pp_tmp = std::get<0>(cammount_pos);
            int tp_tmp = std::get<1>(cammount_pos);

            if(pp_dist != 0){
                pp_d = (pp_dist - pp_tmp) - pp_p; // D制御: 現在差分 - 前回差分
                pp_p = pp_dist - pp_tmp; // P制御: 現在差分
                pp_i += pp_p; // I制御: 現在差分を加算
            }
            if(tp_dist != 0){
                tp_d = (tp_dist - tp_tmp) - pp_p; // D制御: 現在差分 - 前回差分
                tp_p = tp_dist - tp_tmp; // P制御: 現在差分
                tp_i += tp_p; // I制御: 現在差分を加算
            }
        }
	    
        usleep(0.5 *1000000);
        
    }

    //close(s);
	return 0;
}