#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/param.h>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include<typeinfo>

#define BUFFER_SIZE 512

using namespace std;

string make_control_command(int ps, int pp, int ts, int tp);
extern int ps, pp, ts, tp;


bool check_connection(int argc, char *argv[], int s, struct hostent* dst){
    // 正しい引数で実行してね(./Main cammount 4000)
    if(argc<3){
		fprintf(stderr, "usage: %s hostname port\n", argv[0]);
		return false;
	}
    // 繋がらねえよ
	if((s=socket(PF_INET, SOCK_STREAM, 0))==-1){
		perror("Can't create a socket.\n");
		return false;
	}
    // 繋がらねえよ
    if(!(dst=gethostbyname(argv[1]))){
		fprintf(stderr, "Can't resolve the name.\n");
		return false;
	}
    return true;
}

int connection_head(int argc, char *argv[]){

    int s;
    char rcv_buffer[BUFFER_SIZE];
    struct sockaddr_in sin;
    struct hostent* dst;

    // 接続の確認
    bool connection_flag = check_connection(argc, argv, s, dst);
    if(!connection_flag) return -1;

    sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=*((in_addr_t *)*dst->h_addr_list);
	sin.sin_port=htons(atoi(argv[2]));

    connect(s, (const struct sockaddr *)&sin, sizeof(sin));
    recv(s, rcv_buffer, BUFFER_SIZE, 0);
    printf("%s", rcv_buffer);
    /*
    ### PAN-TILT CONTROLLER
    ### v3.5.0, (C)2010-2017 FLIR Systems, Inc., All Rights Reserved
    Initializing...*
    */

    while(true){
        if(ps==5000)
        {
            break;
        }
        string buffer_str = make_control_command(ps, pp, ts, tp);
        auto buffer = buffer_str.c_str();
        if(sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&sin, sizeof(sin))==-1){
		    fprintf(stderr, "Can't sendto from the socket.\n");
		    return 1;
	    }
        usleep(0.5 *1000000);
    }
    return 0;

}

int main(int argc, char *argv[]){

    std::ifstream ifs("test.csv");

    if(!ifs)
    {
        std::cout << "Error! File cannot be opened" << std::endl;
        return 1;
    }


	int			s;
    //char			buffer[BUFFER_SIZE];
    //char buffer[]={'p','s', ' ', ' ', ' ', ' ', ' ', 'p', 'p', ' ', ' ', ' ', ' ', ' ', ' ', 't', 's', ' ', ' ', ' ', ' ', ' ', 't', 'p', ' ', ' ', ' ', ' ', ' ','\n'};
    char			rcv_buffer[BUFFER_SIZE];
	struct sockaddr_in	sin;
	struct hostent		*dst;

	if(argc<3){
		fprintf(stderr, "usage: %s hostname port\n", argv[0]);
		return 1;
	}

	if((s=socket(PF_INET, SOCK_STREAM, 0))==-1){
		perror("Can't create a socket.\n");
		return 1;
	}

	if(!(dst=gethostbyname(argv[1]))){
		fprintf(stderr, "Can't resolve the name.\n");
		return 1;
	}


	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=*((in_addr_t *)*dst->h_addr_list);
	sin.sin_port=htons(atoi(argv[2]));

    connect(s, (const struct sockaddr *)&sin, sizeof(sin));

    recv(s, rcv_buffer, BUFFER_SIZE, 0);
    printf("%s", rcv_buffer);

	float angle;
	uint32_t *tmp;
    int ps, pp, ts, tp;
    std::string str;
    while(getline(ifs, str))
    {
	    std::istringstream ss(str);
        std::string sss;
        int count = 0;
        while(ss >> sss)
        {
            switch(count)
            {
                case 0:
                    ps=std::stoi(sss);
                    break;
                case 1:
                    pp=std::stoi(sss);
                    break;
                case 2:
                    ts=std::stoi(sss);
                    break;
                case 3:
                    tp=std::stoi(sss);
                    break;
            }
            count+=1;
        }
        char			rcv2_buffer[BUFFER_SIZE];
        //fgets(buffer, BUFFER_SIZE, stdin);
	    //std::cin >> ps >> pp >> ts >> tp;
        if(ps==5000)
        {
            break;
        }
        /*
        std::string psstr, ppstr, tsstr, tpstr;
        psstr=std::to_string(ps);
        ppstr=std::to_string(pp);
        tsstr=std::to_string(ts);
        tpstr=std::to_string(tp);
        for(int i=0; i<psstr.size(); ++i)
        {
            buffer[i+2]=psstr[i];
        }
        for(int i=0; i<ppstr.size(); ++i)
        {
            buffer[i+9]=ppstr[i];
        }
        for(int i=0; i<tsstr.size(); ++i)
        {
            buffer[i+17]=tsstr[i];
        }
        for(int i=0; i<tpstr.size(); ++i)
        {
            buffer[i+24]=tpstr[i];
        }
        if(psstr.size()<4)
        {
            buffer[5]=' ';
            if(psstr.size()<3)
            {
                buffer[4]=' ';
                if(psstr.size()<2)
                {
                    buffer[3]=' ';
                }
            }
        }
        if(ppstr.size()<5)
        {
            buffer[13]=' ';
            if(ppstr.size()<4)
            {
                buffer[12]=' ';
            }
        }
        if(tsstr.size()<4)
        {
            buffer[20]=' ';
            if(tsstr.size()<3)
            {
                buffer[19]=' ';
                if(tsstr.size()<2)
                {
                    buffer[18]=' ';
                }
            }
        }

        std::cout << buffer << std::endl;
        */
        string buffer_str = make_control_command(ps, pp, ts, tp);
        auto buffer = buffer_str.c_str();
        std::cout << buffer << endl;


/*
    buffer[2]='2';
    buffer[3]='5';
    buffer[4]='0';
    buffer[5]='0';
    buffer[6]='\n';
    buffer[7]=0;
	*/
        if(sendto(s, buffer, strlen(buffer), 0, (struct sockaddr *)&sin, sizeof(sin))==-1){
		    fprintf(stderr, "Can't sendto from the socket.\n");
		    return 1;
	    }
        usleep(0.5 *1000000);
    }

    close(s);
	return 0;
}