#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>


using namespace std;

int ps, pp, ts, tp;
bool control_flag = true;

string make_control_command(int ps, int pp, int ts, int tp){
    string ps_str = to_string(ps);
    string pp_str = to_string(pp);
    string ts_str = to_string(ts);
    string tp_str = to_string(tp);

    char buffer[] = {'p','s', ' ', ' ', ' ', ' ', ' ', 'p', 'p', ' ', ' ', ' ', ' ', ' ', ' ', 't', 's', ' ', ' ', ' ', ' ', ' ', 't', 'p', ' ', ' ', ' ', ' ', ' ','\n'};
    for (int i = 0; i < ps_str.size(); i++) buffer[i + 2] = ps_str[i];
    for (int i = 0; i < pp_str.size(); i++) buffer[i + 9] = pp_str[i];
    for (int i = 0; i < ts_str.size(); i++) buffer[i + 17] = ts_str[i];
    for (int i = 0; i < tp_str.size(); i++) buffer[i + 24] = tp_str[i];

    return buffer;
}


int main(void){
    std::ifstream ifs("test.csv");

    if(!ifs)
    {
        std::cout << "Error! File cannot be opened" << std::endl;
        return 1;
    }


    return 0;
}