import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
INF = 10e9

data_dir = "/share/home/hara/Data/fove/tmp/"
pupil_pos_txt_path = data_dir + "gaze.txt"
time_txt_path = data_dir + "time.txt"

create_fig = False

class Calibration:
    def __init__(self):
        # 生データ読み込み
        self.pupil_list = []
        self.time_list = []

        # 注視中のデータを抽出
        #self.gaze_begin_time = [3.0, 11.0, 18.0, 25.0, 32.0, INF]
        self.calib_point_num = 25
        self.gaze_begin_time = [i*7.0 + 3.0 for i in range(self.calib_point_num)] + [INF]
        self.gaze_continuation_time = 4.0
        self.extracted_gaze_data = []

        # ハズレ値を除外した注視データ
        self.sigma_n = 2 # 2σ以上離れた値はハズレ値
        self.pupil_pos = [] # 中心，上，右，下，左の[lx, ly, rx, ry]



    def read_pupil_data(self, txt_path):
        f = open(txt_path, 'r')
        data_list = f.readlines()
        for str_data in data_list:
            lx, ly, rx, ry = map(float, str_data[:-1].split())
            self.pupil_list.append([lx, ly, rx, ry])
    
    def read_time_data(self, txt_path):
        f = open(txt_path, 'r')
        data_list = f.readlines()
        self.time_list = [float(data[:-1]) for data in data_list]
    
    def extract_gaze_data(self):
        gaze_index = 0  # center, top, right, bottom, left
        gaze_data = []
        for pupil_data, time in zip(self.pupil_list, self.time_list):
            if time < self.gaze_begin_time[gaze_index]:
                continue
            elif self.gaze_begin_time[gaze_index] <= time and time < self.gaze_begin_time[gaze_index] + self.gaze_continuation_time:
                if not -1 in pupil_data:
                    gaze_data.append(pupil_data)
            else:
                self.extracted_gaze_data.append(gaze_data)
                gaze_data = []
                gaze_index += 1
        self.extracted_gaze_data.append(gaze_data)
        
    
    def _remove_outliers(self, data_list):
        res = []
        mean = np.mean(data_list)
        standard_deviation = np.std(data_list)
        for data in data_list:
            if mean - standard_deviation * self.sigma_n <= data  and data <= mean + standard_deviation * self.sigma_n:
                res.append(data)
        return res


    def remove_outliers(self):
        for gaze_data in self.extracted_gaze_data:
            pupil_pos = []
            for i in range(4):
                pupil_pos.append(np.mean(self._remove_outliers([data[i] for data in gaze_data])))
            self.pupil_pos.append(pupil_pos)
    

    def calculation_distance_from_center(self):
        lx = []
        ly = []
        rx = []
        ry = []
        center = self.pupil_pos[12]
        for data in self.pupil_pos:
            lx.append(data[0] - center[0])
            ly.append(data[1] - center[1])
            rx.append(data[2] - center[2])
            ry.append(data[3] - center[3])
        
        if create_fig:
            fig = plt.figure()
            ax1 = fig.add_subplot(1, 2, 1)
            ax2 = fig.add_subplot(1, 2, 2)
            ax1.scatter(lx, ly)
            ax2.scatter(rx, ry)
            ax1.set_title("left eye")
            ax1.set_xlabel("x [px]")
            ax1.set_ylabel("y [px]")
            ax2.set_title("right eye")
            ax2.set_xlabel("x [px]")
            ax2.set_ylabel("y [px]")
            fig.tight_layout()
            fig.savefig("test2.pdf")
        
        return lx, ly, rx, ry

    
    def calib_test(self, param, lx, ly, rx, ry, disp_x, disp_y):
        exp = np.array([(lambda x: x*x)(lx), (lambda x, y: x*y)(lx, ly), (lambda y: y*y)(ly), lx, ly, np.ones(self.calib_point_num)])
        pxx, pxy, pyy, px, py, p0 = param
        z = pxx * exp[0] + pxy * exp[1] + pyy * exp[2] + px * exp[3] + py * exp[4] + p0 * exp[5]
        LX = np.reshape(lx, (5, 5))
        LY = np.reshape(ly, (5, 5))
        z = np.reshape(z, (5, 5))
        Z = np.reshape(np.array([-300, -150, 0, 150, 300]*5), (5, 5))
        print(LX)
        fig=plt.figure()
        ax11=fig.add_subplot(111,projection='3d')
        x, y = np.meshgrid(np.linspace(-300, 300, 5), np.linspace(-300, 300, 5))
        print(x)
        ax11.plot_wireframe(LX, LY, Z)
        ax11.scatter(LX, LY, z, color = "red")
        print(z)
        plt.show("calib_test.pdf")
        

        

    

    def calculation_pupil_to_display(self):
        # データ
        lx, ly, rx, ry = self.calculation_distance_from_center()        
        lx = np.array(lx)
        ly = np.array(ly)
        rx = np.array(rx)
        ry = np.array(ry)
        disp_x = np.array([-300, -150, 0, 150, 300]*5)
        disp_y = np.array([300] * 5 + [150]*5+[0]*5+[-150]*5+[-300]*5)
        # 近似式 xx + xy + yy + x + y + 1
        l_exp = np.array([(lambda x: x*x)(lx), (lambda x, y: x*y)(lx, ly), (lambda y: y*y)(ly), lx, ly, np.ones(self.calib_point_num)])
        

        param = np.linalg.lstsq(l_exp.T, disp_x)[0]
        pxx, pxy, pyy, px, py, p0 = param
        self.calib_test(param, lx, ly, rx, ry, disp_x, disp_y)




        return pxx, pxy, pyy, px, py, p0
        


if __name__ == "__main__":
    calib = Calibration()
    calib.read_pupil_data(pupil_pos_txt_path)
    calib.read_time_data(time_txt_path)
    calib.extract_gaze_data()
    calib.remove_outliers()
    calib.calculation_distance_from_center()
    calib.calculation_pupil_to_display()