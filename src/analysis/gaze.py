import numpy as np
import matplotlib.pyplot as plt
import cv2
import os
from tqdm import tqdm
import glob
import re

data_dir = "/Users/kazuki/Desktop/fukui_with_image_data/"

gaze_data_file_path = data_dir + "gaze1.txt"
pupil_data_file_path = data_dir + "pupil1.txt"
time_data_file_path = data_dir + "time1.txt"
key_data_file_path = data_dir + "key.txt"

image_dir = "/Users/kazuki/Desktop/fukui_with_image_data/image"
gaze_picture_time_file_path = data_dir + "gaze_picture_time.txt"

plt.rcParams['font.family'] = 'Times New Roman'

class ImageAnalyser:
    def __init__(self):
        self.image_list, self.image_pictured_time_list = self.read_image_data()
        self.key_data = self.read_key_data()
        self.data_num = len(self.image_list)
        #print(self.data_num/(max(self.image_pictured_time_list) - min(self.image_pictured_time_list)))
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
            ssim_list.append(np.mean(np.array(ssim)))
        return ssim_list

    def calc_mse(self):
        mse_list = []
        for i in tqdm(range(self.data_num - 1)):
            mse, _ = cv2.quality.QualityMSE_compute(self.image_list[i], self.image_list[i + 1])
            mse_list.append(mse[0])
        return mse_list
    
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

        
        
        ax.plot(X, Y)
        for i in range(len(self.key_data)//2):
            start = self.key_data[i*2]
            end = self.key_data[i*2+1]
            ax.axvspan(start, end, color="gray", alpha=0.3)
        ax.set_xlim(0, (max(X)//5 + 1)*5)
        ax.set_xticks(np.arange(0, (max(X)//5 + 1)*5, 5))
        ax.set_xlabel("time [s]")
        y_label = index.upper()
        if index == "psnr":
            y_label = "PSNR [dB]"
        ax.set_ylabel(y_label)
        plt.savefig(f"{index}.pdf")


class DataGetter:
    def __init__(self) -> None:
        self.gaze_lx, self.gaze_ly, self.gaze_rx, self.gaze_ry = self.read_pos_data(gaze_data_file_path)
        self.pupil_lx, self.pupil_ly, self.pupil_rx, self.pupil_ry = self.read_pos_data(pupil_data_file_path)
        self.time_data = self.read_time_data(time_data_file_path)
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
        return data

    def read_key_data(self, key_data_file_path):
        f = open(key_data_file_path, 'r')
        txt_data = f.readlines()
        data = []
        for line in txt_data:
            data.append(float(line[:-1]))
        return data

    def remove_error_data(self, data, time_data, label):
        res_data = []
        res_time_data = []
        pupil_data = None
        if label == "lx":
            pupil_data = self.pupil_lx
        elif label == "ly":
            pupil_data = self.pupil_ly
        elif label == "rx":
            pupil_data = self.pupil_rx
        elif label == "ry":
            pupil_data = self.pupil_ry
        for i, (pupil, x, time) in enumerate(zip(pupil_data, data, self.time_data)):
            if pupil != -1:
                res_data.append(x)
                res_time_data.append(time)
        return res_data, res_time_data

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
    
    def sampling(self, data):
        fove_fps = 120
        data_fps = len(data) / (max(self.time_data) - min(self.time_data))
        sampling_rate = int(data_fps / fove_fps)
        return data[::sampling_rate], self.time_data[::sampling_rate]
        
    def sampling_v2(self, data, time_data):
        data_tmp = []
        time_data_tmp = []
        for i in range(len(data)):
            if i == 0 :
                data_tmp.append(data[i])
                time_data_tmp.append(time_data[i])
            else:
                if data[i - 1] != data[i] and time_data[i - 1] != time_data[i]:
                    data_tmp.append(data[i])
                    time_data_tmp.append(time_data[i])
        return data_tmp, time_data_tmp

    def sampling_v3(self, data, time_data):
        data_tmp = []
        time_data_tmp = []
        bin_time = 0.2
        start_time = time_data[0]
        tmp = []
        for i in range(len(data)):
            if time_data[i] < start_time + bin_time:
                tmp.append(data[i])
            else:
                if len(tmp) != 0:
                    data_tmp.append(sum(tmp)/len(tmp))
                    time_data_tmp.append(start_time)
                start_time += bin_time
                tmp = []
                tmp.append(data[i])
        return data_tmp, time_data_tmp



    def gradient(self, data, time_data):
        grad_data = []
        grad_time_data = []
        for i in tqdm(range(len(data) - 1)):
            try:
                grad_data.append((data[i + 1] - data[i]) / (time_data[i + 1] - time_data[i]))
                grad_time_data.append(time_data[i])
            except:
                pass
        return grad_data, grad_time_data

    


    def make_time_series_figure(self, y_data, label, file_name):
        # gaze, time
        X = self.time_data
        Y = y_data
        Y, X = self.remove_error_data(Y, X, file_name[-6:-4])
        Y, X = self.remove_outlier_data(Y, X)
        Y, X = self.sampling_v3(Y, X)

        Y, X = self.gradient(Y, X)
        Y, X = self.gradient(Y, X)

        fig = plt.figure()
        ax1 = fig.add_subplot(111)
        ax1.plot(X, Y)
        ax1.set_xlabel("time (s)")
        ax1.set_ylabel(label)

        for i in range(len(self.key_data)//2):
            start = self.key_data[i*2]
            end = self.key_data[i*2+1]
            ax1.axvspan(start, end, color="gray", alpha=0.3)
            #ax2.axvspan(start, end, color="gray", alpha=0.3)

        plt.tight_layout()
        fig.savefig(file_name)

            



if __name__ == "__main__":
    data_getter = DataGetter()
    data_getter.make_time_series_figure(data_getter.gaze_lx, "grad_x (px)", "output/gaze_lx.pdf")
    data_getter.make_time_series_figure(data_getter.pupil_lx, "grad_x (px)", "output/pupil_lx.pdf")
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
    '''