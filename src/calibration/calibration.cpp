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

Calibration::Calibration(std::string param_file_path){
    std::ifstream file(param_file_path);
    std::string line;
    while(std::getline(file, line)){
        //std::cout << line << std::endl;
        parameter.push_back(split(line, ','));
    }
    lx_pupil_center = parameter[5][0];
    ly_pupil_center = parameter[5][1];
    rx_pupil_center = parameter[5][2];
    ry_pupil_center = parameter[5][3];
}

Calibration::~Calibration(){

}

std::tuple<double,double,double,double> Calibration::calcuration_gaze_point(double lx, double ly, double rx, double ry){
    double lx_distance, ly_distance, rx_distance, ry_distance;
    lx_distance = lx - lx_pupil_center;
    ly_distance = ly - ly_pupil_center;
    rx_distance = rx - rx_pupil_center;
    ry_distance = ry - ry_pupil_center;
    std::cout << "distance: " << lx_distance << " " << ly_distance << std::endl;
    lx_disp = parameter[0][0] * std::pow(lx, 3.0) + parameter[0][1] * std::pow(lx, 2.0) * ly + parameter[0][2] * lx * std::pow(ly, 2.0) + parameter[0][3] * std::pow(ly, 3.0) + parameter[0][4] * std::pow(lx, 2.0) + parameter[0][5] * lx * ly + parameter[0][6] * std::pow(ly, 2.0) + parameter[0][7] * lx + parameter[0][8] * ly + parameter[0][9];
    ly_disp = parameter[1][0] * std::pow(lx, 3.0) + parameter[1][1] * std::pow(lx, 2.0) * ly + parameter[1][2] * lx * std::pow(ly, 2.0) + parameter[1][3] * std::pow(ly, 3.0) + parameter[1][4] * std::pow(lx, 2.0) + parameter[1][5] * lx * ly + parameter[1][6] * std::pow(ly, 2.0) + parameter[1][7] * lx + parameter[1][8] * ly + parameter[1][9];
    rx_disp = parameter[2][0] * std::pow(rx, 3.0) + parameter[2][1] * std::pow(rx, 2.0) * ry + parameter[2][2] * rx * std::pow(ry, 2.0) + parameter[2][3] * std::pow(ry, 3.0) + parameter[2][4] * std::pow(rx, 2.0) + parameter[2][5] * rx * ry + parameter[2][6] * std::pow(ry, 2.0) + parameter[2][7] * rx + parameter[2][8] * ry + parameter[2][9];
    ry_disp = parameter[3][0] * std::pow(rx, 3.0) + parameter[3][1] * std::pow(rx, 2.0) * ry + parameter[3][2] * rx * std::pow(ry, 2.0) + parameter[3][3] * std::pow(ry, 3.0) + parameter[3][4] * std::pow(rx, 2.0) + parameter[3][5] * rx * ry + parameter[3][6] * std::pow(ry, 2.0) + parameter[3][7] * rx + parameter[3][8] * ry + parameter[3][9];
    std::cout << "display: " << lx << " " << ly << std::endl;
    std::tuple<double, double, double, double> disp_gaze = std::make_tuple(lx_disp, ly_disp, rx_disp, ry_disp);
    return disp_gaze;
}

void Calibration::get_parameter(void){
    std::cout << lx_pupil_center << std::endl;
}