#include <string>
#include <tuple>
#include <vector>
#include <iostream>


class Calibration{
    public:
        Calibration(std::string);
        ~Calibration(void);

        std::tuple<double,double,double,double> calcuration_gaze_point(double, double, double, double);
        void get_parameter(void);
    
    private:
        std::vector<std::vector<double>> parameter;
        double lx_pupil_center, ly_pupil_center, rx_pupil_center, ry_pupil_center;
        double lx_disp, ly_disp, rx_disp, ry_disp;
};