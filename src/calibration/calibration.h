#include <string>
#include <tuple>
#include <vector>
#include <iostream>

#define DISP_X 640
#define DISP_Y 720


class Calibration{
    public:
        Calibration(std::string);
        ~Calibration(void);

        std::tuple<double,double,double,double> calcuration_gaze_point(double, double, double, double);
        void get_parameter(void);
        bool check_gaze_in_disp_flag(void);
    
    private:
        std::vector<std::vector<double>> parameter;
        double lx_pupil_center, ly_pupil_center, rx_pupil_center, ry_pupil_center;
        double lx_disp, ly_disp, rx_disp, ry_disp;
        bool gaze_in_disp_flag;
};