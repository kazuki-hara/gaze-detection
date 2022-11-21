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
double seconds_arc_per_position = 46.2857; // 1ステップあたりの角度


void get_position_from_rcv_buffer(std::string rcv_buffer){
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
                break;
            }
        }
    }
    std::cout << pp << " " << tp << std::endl;
}



Cammount::Cammount(std::string input_hostname, int input_port){
    hostname = input_hostname;
    port = input_port;
    pp = 0;
    tp = 0;
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
    printf("%s", rcv_buffer);
    return;
}

std::string Cammount::gaze_to_command(std::tuple<double, double, double, double> gaze_data){
    double lx = std::get<0>(gaze_data);
    double ly = std::get<1>(gaze_data);
    double rx = std::get<2>(gaze_data);
    double ry = std::get<3>(gaze_data);

    // lx,ly,rx,ry -> pp,ps,tp,tsをここに実装
    if(check_detect_pupil_flag() && check_gaze_in_disp_flag()){
        double mean_x = (lx + rx) / 2;
        double mean_y = (ly + ry) / 2;

        double theta_x = std::atan(std::tan(50 * M_PI / 180) * abs(mean_x) / 1280);
        double theta_y = std::atan(std::tan(50 * M_PI / 180) * abs(mean_y) / 1280);

        // 移動する方向を指定
        if(mean_x >= 0) pp = PAN_POSITION_MIN;
        else pp = PAN_POSITION_MAX;
        if(mean_y >= 0) tp = TILTE_POSITION_MAX;
        else tp = TILTE_POSITION_MIN;

        // 移動する速度を指定
        ps = (int)(theta_x * kp * 180 / M_PI / (seconds_arc_per_position / 3600));
        ts = (int)(theta_y * kp * 180 / M_PI / (seconds_arc_per_position / 3600));
    }else{
        pp = 0;
        ps = 0;
        tp = 0;
        ts = 0;
    }


    std::string ps_str = std::to_string(ps);
    std::string pp_str = std::to_string(pp);
    std::string ts_str = std::to_string(ts);
    std::string tp_str = std::to_string(tp);

    char buffer[] = {'p','s', ' ', ' ', ' ', ' ', ' ', 'p', 'p', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 't', 's', ' ', ' ', ' ', ' ', ' ', 't', 'p', ' ', ' ', ' ', ' ', ' ','\n'};
    for (unsigned int i = 0; i < ps_str.size(); i++) buffer[i + 2] = ps_str[i];
    for (unsigned int i = 0; i < pp_str.size(); i++) buffer[i + 9] = pp_str[i];
    for (unsigned int i = 0; i < ts_str.size(); i++) buffer[i + 18] = ts_str[i];
    for (unsigned int i = 0; i < tp_str.size(); i++) buffer[i + 25] = tp_str[i];

    return buffer;
}

int Cammount::send_command(void){
    int i = 0;
    char rcv_buffer[BUFFER_SIZE];
    while(true)
    {
        if(check_exit_flag()){
            close(s);
            break;
        }
        
        std::tuple<double, double, double, double> gaze_data = get_gaze_pixel();

        std::string buffer_str;

        if(check_detect_pupil_flag() && check_gaze_in_disp_flag()){
            buffer_str = gaze_to_command(gaze_data);
            const char* buffer = buffer_str.c_str();
            if(sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&sin, sizeof(sin))==-1){
            fprintf(stderr, "Can't sendto from the socket.\n");
            //close(s);
            //return 1;
            }
        }
        else{
            buffer_str = "pp tp ps0 ts0 pp0 tp0\n";
            const char* buffer = buffer_str.c_str();

            if(sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&sin, sizeof(sin))==-1){
                fprintf(stderr, "Can't sendto from the socket.\n");
            //close(s);
            //return 1;
            }
            
            recv(s, rcv_buffer, BUFFER_SIZE, 0);
            std::cout << i << "番目のrcv_buffer #######################" << std::endl;
            //std::cout << rcv_buffer[29] << std::endl;
            std::string string_rcv_buffer = std::string(rcv_buffer);
            get_position_from_rcv_buffer(string_rcv_buffer);
            i++;
        }
	    
        usleep(0.5 *1000000);
        
    }

    //close(s);
	return 0;
}