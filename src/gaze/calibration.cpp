#include <tuple>
#include <vector>
#include <math.h>
#include "calibration.h"

#define MaxFrameNum 300



void Calibration::Calibration(int max_frame_num){
    max_frame_num = max_frame_num;
}

void Calibration::~Calibration(void){}

void Calibration::add_pupil_pos(std::tuple<double, double, double, double> pupil_pos){
    double lx = std::get<0>(pupil_pos);
    double ly = std::get<1>(pupil_pos);
    double rx = std::get<2>(pupil_pos);
    double ry = std::get<3>(pupil_pos);

    lx_vec.push_back(lx);
    ly_vec.push_back(ly);
    rx_vec.push_back(rx);
    ry_vec.push_back(ry);
}

std::tuple<double, double, double, double> Calibration::calc_pupil_pos_mean(void){
    double lx_mean = 0, ly_mean = 0, rx_mean = 0; ry_mean = 0;
    double lx_var = 0, ly_var = 0, rx_var = 0, ry_var = 0;
    double lx_stdev = 0, ly_stdev = 0, rx_stdev = 0, ry_stdev = 0;

    // 瞳孔の平均位置を算出
    for(int i = 0; i < max_frame_num; i++){
        lx_mean += lx_vec[i];
        ly_mean += ly_vec[i];
        rx_mean += rx_vec[i];
        ry_mean += ry_vec[i];
    }
    lx_mean /= max_frame_num;
    ly_mean /= max_frame_num;
    rx_mean /= max_frame_num;
    ry_mean /= max_frame_num;

    // 分散を計算
    for(int i = 0; i < max_frame_num; i++){
        lx_var += (lx_vec[i] - lx_ave)**2;
        ly_var += (ly_vec[i] - ly_ave)**2;
        rx_var += (rx_vec[i] - rx_ave)**2;
        ry_var += (ry_vec[i] - ry_ave)**2;
    }
    lx_var /= max_frame_num;
    ly_var /= max_frame_num;
    rx_var /= max_frame_num;
    ry_var /= max_frame_num;

    // 標準偏差を計算
    lx_stdev = sqrt(lx_var);
    ly_stdev = sqrt(ly_var);
    rx_stdev = sqrt(rx_var);
    ry_stdev = sqrt(ry_var);

    // ハズレ値を除外した平均位置を算出 (今回は3σ以内)
    double lx_result = 0, ly_result = 0, rx_result = 0, ry_result = 0;
    int l_num = 0, r_num = 0;
    int n = 3;
    for(int i = 0; i < max_frame_num; i++){
        if(lx_mean - lx_stdev * n <= lx_vec[i] && lx_vec[i] <= lx_mean + lx_stdev * n && ly_mean - ly_stdev * n <= ly_vec[i] && ly_vec[i] <= ly_mean + ly_stdev * n){
            lx_result += lx_vec[i];
            ly_result += ly_vec[i];
            l_num += 1;
        }
        if(rx_mean - rx_stdev * n <= rx_vec[i] && rx_vec[i] <= rx_mean + rx_stdev * n && ry_mean - ry_stdev * n <= ry_vec[i] && ry_vec[i] <= ry_mean + ry_stdev * n){
            rx_resurt += rx_vec[i];
            ry_resurt += ry_vec[i];
            r_num += 1;
        }
    }
    lx_result /= l_num;
    ly_result /= l_num;
    rx_result /= r_num;
    ry_result /= r_num;
    
    return std::make_tuple(lx_result, ly_result, rx_result, ry_result);
}