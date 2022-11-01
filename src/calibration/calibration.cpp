#include <string>
#include <tuple>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include "calibration.h"

std::vector<double> split(std::string s, char delim){
    std::vector<double> elems;
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)){
        if(!item.empty()){
            elems.push_back(stod(item));
        }
    }
    return elems;
}

double pow(double x, int n){
    double y = 1;
    for(int i = 0; i < n; i++){
        y = (double)y*x;
    }
    return y;
}

Calibration::Calibration(std::string param_file_path){
    std::ifstream file(param_file_path);
    std::string line;
    while(std::getline(file, line)){
        //std::cout << line << std::endl;
        parameter.push_back(split(line, ','));
    }
    lx_pupil_center = parameter[4][0];
    ly_pupil_center = parameter[4][1];
    rx_pupil_center = parameter[4][2];
    ry_pupil_center = parameter[4][3];
    gaze_in_disp_flag = false;
}

Calibration::~Calibration(){

}

std::tuple<double,double,double,double> Calibration::calcuration_gaze_point(double lx, double ly, double rx, double ry){
    double lx_distance, ly_distance, rx_distance, ry_distance;
    lx_distance = lx - lx_pupil_center;
    ly_distance = ly - ly_pupil_center;
    rx_distance = rx - rx_pupil_center;
    ry_distance = ry - ry_pupil_center;
    lx_disp = parameter[0][0] * pow(lx_distance, 3) + parameter[0][1] * pow(lx_distance, 2) * ly_distance + parameter[0][2] * lx_distance * pow(ly_distance, 2) + parameter[0][3] * pow(ly_distance, 3) + parameter[0][4] * pow(lx_distance, 2) + parameter[0][5] * lx_distance * ly_distance + parameter[0][6] * pow(ly_distance, 2) + parameter[0][7] * lx_distance + parameter[0][8] * ly_distance + parameter[0][9];
    ly_disp = parameter[1][0] * pow(lx_distance, 3) + parameter[1][1] * pow(lx_distance, 2) * ly_distance + parameter[1][2] * lx_distance * pow(ly_distance, 2) + parameter[1][3] * pow(ly_distance, 3) + parameter[1][4] * pow(lx_distance, 2) + parameter[1][5] * lx_distance * ly_distance + parameter[1][6] * pow(ly_distance, 2) + parameter[1][7] * lx_distance + parameter[1][8] * ly_distance + parameter[1][9];
    rx_disp = parameter[2][0] * pow(rx_distance, 3) + parameter[2][1] * pow(rx_distance, 2) * ry_distance + parameter[2][2] * rx_distance * pow(ry_distance, 2) + parameter[2][3] * pow(ry_distance, 3) + parameter[2][4] * pow(rx_distance, 2) + parameter[2][5] * rx_distance * ry_distance + parameter[2][6] * pow(ry_distance, 2) + parameter[2][7] * rx_distance + parameter[2][8] * ry_distance + parameter[2][9];
    ry_disp = parameter[3][0] * pow(rx_distance, 3) + parameter[3][1] * pow(rx_distance, 2) * ry_distance + parameter[3][2] * rx_distance * pow(ry_distance, 2) + parameter[3][3] * pow(ry_distance, 3) + parameter[3][4] * pow(rx_distance, 2) + parameter[3][5] * rx_distance * ry_distance + parameter[3][6] * pow(ry_distance, 2) + parameter[3][7] * rx_distance + parameter[3][8] * ry_distance + parameter[3][9];
    std::tuple<double, double, double, double> disp_gaze = std::make_tuple(lx_disp, ly_disp, rx_disp, ry_disp);

    if(abs(lx_disp) <= DISP_X && abs(ly_disp) <= DISP_Y && abs(rx_disp) <= DISP_X && abs(ry_disp) <= DISP_Y) gaze_in_disp_flag = true;
    else gaze_in_disp_flag = false;
    
    return disp_gaze;
}


void Calibration::get_parameter(void){
    std::cout << lx_pupil_center << std::endl;
}

bool Calibration::check_gaze_in_disp_flag(void){
    return gaze_in_disp_flag;
}