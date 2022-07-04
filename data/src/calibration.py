import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pandas as pd
from sys import exit

INF = 10e9

data_dir = "/share/home/hara/Data/fove/tmp/"
#data_dir = "/share/home/hara/Data/fove/gaze/old_data/20220419/02/"
pupil_pos_txt_path = data_dir + "pupil0.txt"
time_txt_path = data_dir + "time0.txt"

pupil_pos_txt_path = "/share/home/hara/Data/fove/pupil/hara/200/pupil_ellipse.txt"
time_txt_path = "/share/home/hara/Data/fove/pupil/hara/200/time0.txt"

create_figure =  True

np.seterr(divide='ignore', invalid='ignore')

class Calibration:
    def __init__(self):
        # 生データ読み込み
        self.pupil_list = []
        self.time_list = []

        # 注視中のデータを抽出
        #self.gaze_begin_time = [3.0, 11.0, 18.0, 25.0, 32.0, INF]
        self.calib_point_num = 25
        self.gaze_begin_time = [i *5.0 + 0.5 for i in range(self.calib_point_num)] + [INF]
        self.gaze_continuation_time = 4.0
        self.extracted_gaze_data = []

        # ハズレ値を除外した注視データ
        self.sigma_n = 2 # 2σ以上離れた値はハズレ値
        self.pupil_pos = [] # 中心，上，右，下，左の[lx, ly, rx, ry]


    def read_pupil_data(self, txt_path):
        f = open(txt_path, 'r')
        data_list = f.readlines()
        for str_data in data_list:
            try:
                lx, ly, rx, ry, i = map(float, str_data[:-1].split())
                self.pupil_list.append([lx, ly, rx, ry])
            except:
                pass
            
    

    def read_time_data(self, txt_path):
        f = open(txt_path, 'r')
        data_list = f.readlines()
        self.time_list = [float(data[:-1]) for data in data_list]


    def extract_gaze_data(self):
        gaze_index = 0  # center, top, right, bottom, left
        gaze_data = []
        data_length = min(len(self.pupil_list), len(self.time_list))
        for pupil_data, time in zip(self.pupil_list[:data_length], self.time_list[:data_length]):
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
        if np.isnan(mean):
            for data in data_list:
                print(data)
            exit()
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
        self.pupil_pos = self.pupil_pos[:25]
    

    def calcuration_distance_from_center(self):
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
        
        if create_figure:
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
        # データ変形
        LX = np.reshape(lx, (5, 5))
        LY = np.reshape(ly, (5, 5))
        RX = np.reshape(rx, (5, 5))
        RY = np.reshape(ry, (5, 5))

        # 近似式 xx + xy + yy + x + y + 1
        #l_exp = np.array([(lambda x: x*x)(lx), (lambda x, y: x*y)(lx, ly), (lambda y: y*y)(ly), lx, ly, np.ones(self.calib_point_num)])
        #r_exp = np.array([(lambda x: x*x)(rx), (lambda x, y: x*y)(rx, ry), (lambda y: y*y)(ry), rx, ry, np.ones(self.calib_point_num)])
        l_exp = np.array([(lambda x: x*x*x)(lx), (lambda x, y: x*x*y)(lx, ly), (lambda x, y: x*y*y)(lx, ly), (lambda y: y*y*y)(ly),(lambda x: x*x)(lx), (lambda x, y: x*y)(lx, ly), (lambda y: y*y)(ly), lx, ly, np.ones(self.calib_point_num)])
        r_exp = np.array([(lambda x: x*x*x)(rx), (lambda x, y: x*x*y)(rx, ry), (lambda x, y: x*y*y)(rx, ry), (lambda y: y*y*y)(ry),(lambda x: x*x)(rx), (lambda x, y: x*y)(rx, ry), (lambda y: y*y)(ry), rx, ry, np.ones(self.calib_point_num)])


        # 注視点を計算
        lx_disp = sum([param["lx"][i] * l_exp[i] for i in range(len(param["lx"]))])
        ly_disp = sum([param["ly"][i] * l_exp[i] for i in range(len(param["ly"]))])
        rx_disp = sum([param["rx"][i] * r_exp[i] for i in range(len(param["rx"]))])
        ry_disp = sum([param["ry"][i] * r_exp[i] for i in range(len(param["ry"]))])

        # データ変形
        lx_disp = np.reshape(lx_disp, (5, 5))
        ly_disp = np.reshape(ly_disp, (5, 5))
        rx_disp = np.reshape(rx_disp, (5, 5))
        ry_disp = np.reshape(ry_disp, (5, 5))
        x_disp = np.reshape(np.array([-200, -100, 0, 100, 200]*5), (5, 5)) # 表示した注視点のx座標(理論値)
        y_disp = np.reshape(np.array([200]*5 + [100]*5 + [0]*5 + [-100]*5 + [-200]*5), (5, 5)) # 表示した注視点のy座標(理論値)

        
        def create_figure(name):
            fig=plt.figure()
            fig.subplots_adjust(left=0, right=1, bottom=0, top=1)
            x, y = np.meshgrid(np.linspace(-200, 200, 5), np.linspace(-200, 200, 5))
            ax11=fig.add_subplot(111,projection='3d')
            if name == "lx":
                ax11.plot_wireframe(LX, LY, lx_disp-x_disp)
            elif name == "ly":
                ax11.plot_wireframe(LX, LY, ly_disp-y_disp)
            elif name == "rx":
                ax11.plot_wireframe(RX, RY, rx_disp-x_disp)
            else:
                ax11.plot_wireframe(RX, RY, ry_disp-y_disp)
            ax11.set_xlabel("$x_{pupil} [px]$")
            ax11.set_ylabel("$y_{pupil} [px]$")
            if name == "lx":
                ax11.set_zlabel("$error_{lx} [px]$")
            elif name == "ly":
                ax11.set_zlabel("$error_{ly} [px]$")
            elif name == "rx":
                ax11.set_zlabel("$error_{rx} [px]$")
            else:
                ax11.set_zlabel("$error_{ry} [px]$")
            
            ax11.set_zlim(-15, 15)
            #ax11.set_zticks(np.array([-200, -100, 0, 100, 200]))
            fig.tight_layout()
            plt.savefig(name+".pdf")
        
        create_figure("lx")
        create_figure("ly")
        create_figure("rx")
        create_figure("ry")


    def calcuration_pupil_to_display(self):
        # データ
        lx, ly, rx, ry = self.calcuration_distance_from_center()    

        lx = np.array(lx)[:self.calib_point_num]
        ly = np.array(ly)[:self.calib_point_num]
        rx = np.array(rx)[:self.calib_point_num]
        ry = np.array(ry)[:self.calib_point_num]
        disp_x = np.array([-200, -100, 0, 100, 200]*5)
        disp_y = np.array([200] * 5 + [100]*5+[0]*5+[-100]*5+[-200]*5)

        # 近似式 xx + xy + yy + x + y + 1
        #l_exp = np.array([(lambda x: x*x)(lx), (lambda x, y: x*y)(lx, ly), (lambda y: y*y)(ly), lx, ly, np.ones(self.calib_point_num)])
        #r_exp = np.array([(lambda x: x*x)(rx), (lambda x, y: x*y)(rx, ry), (lambda y: y*y)(ry), rx, ry, np.ones(self.calib_point_num)])
        l_exp = np.array([(lambda x: x*x*x)(lx), (lambda x, y: x*x*y)(lx, ly), (lambda x, y: x*y*y)(lx, ly), (lambda y: y*y*y)(ly),(lambda x: x*x)(lx), (lambda x, y: x*y)(lx, ly), (lambda y: y*y)(ly), lx, ly, np.ones(self.calib_point_num)])
        r_exp = np.array([(lambda x: x*x*x)(rx), (lambda x, y: x*x*y)(rx, ry), (lambda x, y: x*y*y)(rx, ry), (lambda y: y*y*y)(ry),(lambda x: x*x)(rx), (lambda x, y: x*y)(rx, ry), (lambda y: y*y)(ry), rx, ry, np.ones(self.calib_point_num)])

        # 近似パラメータ
        lx_param = np.linalg.lstsq(l_exp.T, disp_x, rcond=None)[0]
        ly_param = np.linalg.lstsq(l_exp.T, disp_y, rcond=None)[0]
        rx_param = np.linalg.lstsq(r_exp.T, disp_x, rcond=None)[0]
        ry_param = np.linalg.lstsq(r_exp.T, disp_y, rcond=None)[0]
        param = {"lx": lx_param, "ly": ly_param, "rx": rx_param, "ry": ry_param}

        with open("/share/home/hara/workspace/fove/config/calibration/parameter.txt", "w") as f:
            f.writelines(",".join(map(str, param["lx"]))+"\n")
            f.writelines(",".join(map(str, param["ly"]))+"\n")
            f.writelines(",".join(map(str, param["rx"]))+"\n")
            f.writelines(",".join(map(str, param["ry"]))+"\n")
            f.writelines(",".join(map(str,self.pupil_pos[12]))+"\n")
        # 近似できているか確認
        if create_figure:
            self.calib_test(param, lx, ly, rx, ry, disp_x, disp_y)

        return param




if __name__ == "__main__":
    import warnings
    warnings.simplefilter('ignore')
    calib = Calibration()
    calib.read_pupil_data(pupil_pos_txt_path)
    calib.read_time_data(time_txt_path)
    calib.extract_gaze_data()
    calib.remove_outliers()
    calib.calcuration_distance_from_center()
    calib.calcuration_pupil_to_display()