#include <tuple>

class Calibration{
    public:
        void Calibration(int);
        void ~Calibration(void);

        void add_pupil_pos(std::tuple<double, double, double, double>);
        std::tuple<double, double, double, double> calc_pupil_pos_mean(void);
        

        

    private:
        std::vector<double> lx_vec;
        std::vector<double> ly_vec;
        std::vector<double> rx_vec;
        std::vector<double> ry_vec;

        int max_frame_num;
        int frame_num;

}