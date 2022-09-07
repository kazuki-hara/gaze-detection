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
#include "./cammount.h"
#include "./../main.h"

#define BUFFER_SIZE 512

Cammount::Cammount(std::string input_hostname, int input_port){
    hostname = input_hostname;
    port = input_port;
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

std::string Cammount::gaze_to_command(double lx, double ly, double rx, double ry){
    int ps=1000, pp=2500, ts=500, tp=500;

    // lx,ly,rx,ry -> pp,ps,tp,tsをここに実装

    std::string ps_str = std::to_string(ps);
    std::string pp_str = std::to_string(pp);
    std::string ts_str = std::to_string(ts);
    std::string tp_str = std::to_string(tp);

    char buffer[] = {'p','s', ' ', ' ', ' ', ' ', ' ', 'p', 'p', ' ', ' ', ' ', ' ', ' ', ' ', 't', 's', ' ', ' ', ' ', ' ', ' ', 't', 'p', ' ', ' ', ' ', ' ', ' ','\n'};
    for (unsigned int i = 0; i < ps_str.size(); i++) buffer[i + 2] = ps_str[i];
    for (unsigned int i = 0; i < pp_str.size(); i++) buffer[i + 9] = pp_str[i];
    for (unsigned int i = 0; i < ts_str.size(); i++) buffer[i + 17] = ts_str[i];
    for (unsigned int i = 0; i < tp_str.size(); i++) buffer[i + 24] = tp_str[i];

    return buffer;
}

int Cammount::send_command(void){
    while(true)
    {
	    std::tuple<double, double, double, double> gaze_data = get_disp_gaze();
        double lx = std::get<0>(gaze_data);
        double ly = std::get<1>(gaze_data);
        double rx = std::get<2>(gaze_data);
        double ry = std::get<3>(gaze_data);
        //char rcv2_buffer[BUFFER_SIZE];

        std::string buffer_str = gaze_to_command(lx, ly, rx, ry);
        std::cout << buffer_str << std::endl;
        const char* buffer = buffer_str.c_str();
        //std::cout << buffer << std::endl;
        std::cout << "result" << sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&sin, sizeof(sin)) << std::endl;
        if(sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&sin, sizeof(sin))==-1){
		    fprintf(stderr, "Can't sendto from the socket.\n");
            //close(s);
		    //return 1;
	    }
        usleep(0.5 *1000000);
    }

    //close(s);
	return 0;
}