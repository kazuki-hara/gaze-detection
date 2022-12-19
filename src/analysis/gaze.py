import numpy as np
import matplotlib.pyplot as plt
import cv2
import os
from tqdm import tqdm
import glob
import re
import math
from utils import lpf, gradient, moving_average, fft

data_dir = "/share/home/hara/Data/fove/20221213/hara_amp_100/"
data_dir = "/home/hara/data/20221215/02/"
data_dir = "/tmp/data/"

gaze_data_file_path = data_dir + "gaze1.txt"
pupil_data_file_path = data_dir + "pupil1.txt"
time_data_file_path = data_dir + "time1.txt"
key_data_file_path = data_dir + "key.txt"

image_dir = data_dir + "image"
gaze_picture_time_file_path = data_dir + "gaze_picture_time.txt"

cammount_pos_data_file_path = data_dir + "cammount_pos.txt"

seconds_arc_per_position = 46.2857

#plt.rcParams['font.family'] = 'Times New Roman'

class ImageAnalyser:
    def __init__(self):
        self.image_list, self.image_pictured_time_list = self.read_image_data()
        self.key_data = self.read_key_data()
        self.data_num = len(self.image_list)
        return

    def read_image_data(self):
        image_list = []
        image_pictured_time_list = []
        image_pictured_time_list_tmp = []

        f = open(gaze_picture_time_file_path, 'r')
        txt_data = f.readlines()
        data = []
        for line in txt_data:
            image_pictured_time_list_tmp.append(float(line[:-1]))

        image_file_num = len(glob.glob(image_dir + "/*.png"))
        for i in range(image_file_num):
            if os.path.exists(f'{image_dir}/{i}.png'):
                image = cv2.imread(f'{image_dir}/{i}.png')
            
                
                image_list.append(image)
                image_pictured_time_list.append(image_pictured_time_list_tmp[i])
        return image_list, image_pictured_time_list
    
    def read_key_data(self):
        f = open(key_data_file_path, 'r')
        txt_data = f.readlines()
        data = []
        for line in txt_data:
            data.append(float(line[:-1]))
        return data


    def calc_similarity(self, index):
        res_data = []
        res_time_data = []
        if index == "psnr":
            for i in tqdm(range(self.data_num - 1)):
                if not np.array_equal(self.image_list[i], self.image_list[i + 1]):
                    psnr = cv2.PSNR(self.image_list[i], self.image_list[i + 1])
                    res_data.append(psnr)
                    res_time_data.append(time_data[i])
            return res_data, res_time_data
        elif index == "ssim":
            for i in tqdm(range(self.data_num - 1)):
                ssim, _ = cv2.quality.QualitySSIM_compute(self.image_list[i], self.image_list[i + 1])
                res_data.append(np.mean(np.array(ssim)))
            return res_data
        elif index == "mse":
            for i in tqdm(range(self.data_num - 1)):
                mse, _ = cv2.quality.QualityMSE_compute(self.image_list[i], self.image_list[i + 1])
                res_data.append(mse[0])
            return res_data
        else:
            assert True, "評価指標の引数が間違ってる"




    
    def calc_psnr(self, data, time_data):
        psnr_list = []
        time_data_list = []
        data_num = len(data)
        for i in tqdm(range(data_num - 1)):
            if not np.array_equal(data[i], data[i + 1]):
                
                psnr = cv2.PSNR(data[i], data[i + 1])
                psnr_list.append(psnr)
                time_data_list.append(time_data[i])
        return psnr_list, time_data_list

    def calc_ssim(self):
        ssim_list = []
        for i in tqdm(range(self.data_num - 1)):
            ssim, _ = cv2.quality.QualitySSIM_compute(self.image_list[i], self.image_list[i + 1])
            ssim_list.append(np.mean(np.array(ssim[:3])))
        return ssim_list

    def calc_mse(self):
        mse_list = []
        for i in tqdm(range(self.data_num - 1)):
            mse, _ = cv2.quality.QualityMSE_compute(self.image_list[i], self.image_list[i + 1])
            mse_list.append(mse[0])
        return mse_list

    def moving_average(self, data):
        span = 10
        res_data = []
        for i in range(len(data)):
            if i < 10:
                res_data.append(np.mean(data[:i]))
            else:
                res_data.append(np.mean(data[i-10:i]))
        return res_data

    
    def make_time_series_figure(self, index):
        fig = plt.figure()
        ax = fig.add_subplot(111)

        X = self.image_pictured_time_list[1:]
        if index == "mse":
            Y = self.calc_mse()
        elif index == "psnr":
            Y, X = self.calc_psnr(self.image_list, self.image_pictured_time_list)
        elif index == "ssim":
            Y = self.calc_ssim()

        Y_ave = lpf(Y, X, 0.1)
        
        ax.plot(X, Y, label = index)
        ax.plot(X, Y_ave, label = "average")
        for i in range(len(self.key_data)//2):
            start = self.key_data[i*2]
            end = self.key_data[i*2+1]
            ax.axvspan(start, end, color="gray", alpha=0.3)
        ax.set_xlim(0, (max(X)//5 + 1)*5)
        ax.set_xlabel("time [s]")
        y_label = index.upper()
        if index == "psnr":
            y_label = "PSNR [dB]"
        ax.set_ylabel(y_label)
        ax.legend()
        plt.savefig(f"./output/image_sim/{index}.pdf")


class DataGetter:
    def __init__(self):
        self.gaze_lx, self.gaze_ly, self.gaze_rx, self.gaze_ry = self.read_pos_data(gaze_data_file_path)
        self.pupil_lx, self.pupil_ly, self.pupil_rx, self.pupil_ry = self.read_pos_data(pupil_data_file_path)
        self.replace_error_data()
        self.time_data = self.read_time_data(time_data_file_path)
        self.pan_position, self.tilte_position, self.cammount_time_data = self.read_cammount_pos_data(cammount_pos_data_file_path)
        self.key_data = self.read_key_data(key_data_file_path)

    def read_pos_data(self, file_path):
        f = open(file_path, 'r')
        txt_data = f.readlines()
        lx = []
        ly = []
        rx = []
        ry = []

        for line in txt_data:
            data = list(map(float, line[:-1].split(" ")))
            lx.append(data[0])
            ly.append(data[1])
            rx.append(data[2])
            ry.append(data[3])
        return lx, ly, rx, ry

    def read_time_data(self, time_data_file_path):
        f = open(time_data_file_path, 'r')
        txt_data = f.readlines()
        data = []
        for line in txt_data:
            data.append(float(line[:-1]))
        # サンプリングの時間幅を均一にする
        data_num = len(data)
        return list(np.linspace(data[0], data[-1], data_num))

    def read_key_data(self, key_data_file_path):
        f = open(key_data_file_path, 'r')
        txt_data = f.readlines()
        data = []
        res_data = []
        for line in txt_data:
            data.append(float(line[:-1]))

        for i in range(len(data)//2):
            start = data[i*2]
            end = data[i*2+1]
            res_data.append([start, end])
        return res_data

    def read_cammount_pos_data(self, cammount_pos_data_file_path):
        f = open(cammount_pos_data_file_path, 'r')
        txt_data = f.readlines()
        pan_position = []
        tilte_position = []
        time_data = []
        for line in txt_data:
            data = list(map(float, line[:-1].split(" ")))
            pan_position.append(int(data[0]))
            tilte_position.append(int(data[1]))
            time_data.append(data[2])
        return pan_position, tilte_position, time_data
    
    def _replace_error_data(self, pupil_data_list, gaze_data_list):
        data_num = len(pupil_data_list)
        res_pupil_data_list = []
        res_gaze_data_list = []
        for i, (pupil, gaze) in enumerate(zip(pupil_data_list, gaze_data_list)):
            if pupil == -1:
                if i == 0:
                    res_pupil_data_list.append(np.mean([x for x in pupil_data_list if x != -1]))
                    res_gaze_data_list.append(np.mean([x for x in gaze_data_list if x != -1]))
                else:
                    res_pupil_data_list.append(pupil_data_list[-1])
                    res_gaze_data_list.append(gaze_data_list[-1])
            else:
                res_pupil_data_list.append(pupil_data_list[i])
                res_gaze_data_list.append(gaze_data_list[i])
        return res_pupil_data_list, res_gaze_data_list
    
    def replace_error_data(self):
        self.pupil_lx, self.gaze_lx = self._replace_error_data(self.pupil_lx, self.gaze_lx)
        self.pupil_ly, self.gaze_ly = self._replace_error_data(self.pupil_ly, self.gaze_ly)
        self.pupil_rx, self.gaze_rx = self._replace_error_data(self.pupil_rx, self.gaze_rx)
        self.pupil_ry, self.gaze_ry = self._replace_error_data(self.pupil_ry, self.gaze_ry)
        
    def remove_outlier_data(self, data, time_data):
        data_arr = np.array(data)
        sigma = 1.5
        under_limit = np.mean(data_arr) - sigma * np.std(data_arr)
        upper_limit = np.mean(data_arr) + sigma * np.std(data_arr)

        res_data = []
        res_time_data = []
        
        for i, (x, time) in enumerate(zip(data, time_data)):
            if under_limit < x < upper_limit:
                res_data.append(x)
                res_time_data.append(time)
        return res_data, res_time_data

    def extract_gazing_data(self, data):
        for i, key_data in enumerate(self.key_data):
            start = key_data[0]
            end = key_data[1]
            start_index =  min([i for i, time in enumerate(self.time_data) if time >= start])
            end_index =  max([i for i, time in enumerate(self.time_data) if time <= end])
            gazing_data = data[start_index:end_index + 1]
            gazing_time_data = self.time_data[start_index:end_index + 1]
            max_freq = fft(gazing_data, gazing_time_data, save_fig = True, file_name = f"output/non_gaze/fft_{i}.pdf")
            print(f"gaze {i}: {max_freq} Hz")


    
    def extract_not_gazing_data(self, data):
        for i in range(len(self.key_data) - 1):
            start = self.key_data[i][1]
            end = self.key_data[i + 1][0]
            start_index =  min([i for i, time in enumerate(self.time_data) if time >= start])
            end_index =  max([i for i, time in enumerate(self.time_data) if time <= end])
            gazing_data = data[start_index:end_index + 1]
            gazing_time_data = self.time_data[start_index:end_index + 1]
            max_freq = fft(gazing_data, gazing_time_data, save_fig = True, file_name = f"output/gaze/fft_{i}.pdf")
            print(f"non gaze {i}: {max_freq} Hz")
    
    def pupil_cam_pos_figure(self, xy="x"):
        pupil_time = self.time_data
        came_time = self.cammount_time_data
        if xy == "x":
            pupil_pos = [(lx + rx) / 2 for lx, rx in zip(self.pupil_lx, self.pupil_rx)]
            cam_pos = [position * (seconds_arc_per_position / 3600) for position in self.pan_position]
        else:
            pupil_pos = [(ly + ry) / 2 for ly, ry in zip(self.pupil_ly, self.pupil_ry)]
            cam_pos = [position * (seconds_arc_per_position / 3600) for position in self.tilte_position]
        
        pupil_pos = lpf(pupil_pos, pupil_time, 1)

        fig = plt.figure()
        ax1 = fig.add_subplot(111)
        ax1.set_xlabel("time (s)")
        if xy == "x":
            ax1.plot(pupil_time, pupil_pos, label = r"$x_\mathrm{pupil}$")
            ax1.set_ylabel(r"$x_\mathrm{pupil}$ (px)")
        else:
            ax1.plot(pupil_time, pupil_pos, label = r"$y_\mathrm{pupil}$")
            ax1.set_ylabel(r"$y_\mathrm{pupil}$ (px)")
        h1, l1 = ax1.get_legend_handles_labels()
        ax2 = ax1.twinx()
        if xy == "x":
            ax2.plot(self.cammount_time_data, cam_pos, color = "orange", label = "camera pan position")
            ax2.set_ylabel(r"camera pan position (deg)")
        if xy == "y":
            ax2.plot(self.cammount_time_data, cam_pos, color = "orange", label = "camera tilte position")
            ax2.set_ylabel(r"camera tilte position (deg)")
        h2, l2 = ax2.get_legend_handles_labels()

        for key_data in self.key_data:
            start = key_data[0]
            end = key_data[1]
            ax1.axvspan(start, end, color="gray", alpha=0.3)

        plt.tight_layout()
        ax1.legend(h1+h2, l1+l2)
        file_name = f"./output/pupil_cam_pos/{xy}.pdf"
        fig.savefig(file_name)


    def make_time_series_figure(self, side = "left"):
        # gaze, time
        grad_x, X = gradient(self.pupil_lx, self.time_data)  # 微分
        grad_y, X = gradient(self.pupil_ly, self.time_data)
        gradY = [math.sqrt(x ** 2 + y ** 2) for x, y in zip(grad_x, grad_y)]
        gradY = lpf(gradY, X, 1) # lpf

        self.extract_not_gazing_data(gradY)
        self.extract_gazing_data(gradY)

        fig = plt.figure()
        ax1 = fig.add_subplot(111)
        ax1.plot(X, gradY, label = r"$\mathrm{d}pupil_\mathrm{left}$")
        ax1.set_xlabel("time (s)")
        ax1.set_ylabel(r"$\mathrm{pupil}_\mathrm{left}$ (px/s)")

        for key_data in self.key_data:
            start = key_data[0]
            end = key_data[1]
            ax1.axvspan(start, end, color="gray", alpha=0.3)

        plt.tight_layout()
        ax1.legend()
        file_name = "./output/grad/left_eye.pdf"
        fig.savefig(file_name)

            



if __name__ == "__main__":
    
    data_getter = DataGetter()
    #data_getter.make_time_series_figure(data_getter.gaze_lx, "grad_x (px)", "output/gaze_lx.pdf")
    data_getter.make_time_series_figure()
    data_getter.pupil_cam_pos_figure("x")
    data_getter.pupil_cam_pos_figure("y")
    '''
    data_getter.make_time_series_figure(data_getter.gaze_ly, "grad_y (px)", "output/gaze_ly.pdf")
    data_getter.make_time_series_figure(data_getter.pupil_ly, "grad_y (px)", "output/pupil_ly.pdf")
    data_getter.make_time_series_figure(data_getter.gaze_rx, "grad_x (px)", "output/gaze_rx.pdf")
    data_getter.make_time_series_figure(data_getter.pupil_rx, "grad_x (px)", "output/pupil_rx.pdf")
    data_getter.make_time_series_figure(data_getter.gaze_ry, "grad_y (px)", "output/gaze_ry.pdf")
    data_getter.make_time_series_figure(data_getter.pupil_ry, "grad_y (px)", "output/pupil_ry.pdf")
    '''
    image_analyser = ImageAnalyser()
    image_analyser.make_time_series_figure("mse")
    image_analyser.make_time_series_figure("psnr")
    image_analyser.make_time_series_figure("ssim")