#include <time.h>

#define LOG_RATE 10
double get_passed_time(void);
int check_mode(void);
std::tuple<double, double, double, double>get_disp_gaze(void);
bool check_detect_pupil_flag(void);