import math
from tqdm import tqdm
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# ローパスフィルタ
def lpf(data, time_data, cutoff_F):  # cutoff_F = カットオフ周波数
    T = (time_data[-1] - time_data[0]) / (len(time_data) - 1)  # サンプリング周期
    tau = 1 / (2 * math.pi * cutoff_F)
    alpha = tau / (T + tau)
    
    res_data = []
    for i in range(len(data)):
        if i == 0:
            res_data.append(data[0])
        else:
            res_data.append(alpha * res_data[i - 1] + (1 - alpha) * data[i])
    return res_data

# 微分
def gradient(data, time_data):
    grad_data = []
    grad_time_data = []
    for i in tqdm(range(len(data) - 1)):
        grad_data.append((data[i + 1] - data[i]) / (time_data[i + 1] - time_data[i]))
        grad_time_data.append(time_data[i])
    return grad_data, grad_time_data

# 移動平均
def moving_average(data, span = 10, sigma = 2):
    span = 10
    sigma = 2
    index = np.range(len(data))
    ts = pd.Series(data, index=index)
    ewm_mean = ts.ewm(span=span).mean()
    ewm_std = ts.ewm(span=span).std()  # 指数加重移動標準偏差
    res_data = []
    for i in range(len(data)):
        if i < span:
            res_data.append(np.mean(data[:i+1]))
        else:
            res_data.append(np.mean(data[i-span:i]))
    return res_data

# 距離の計算
def calc_distance(dx_data, dy_data):
    res_data = []
    for dx, dy in zip(dx_data, dy_data):
        distance = math.sqrt(dx ** 2 + dy ** 2)
        res_data.append(res_data)
    return res_data

def fft(data, time_data, save_fig = False, file_name = "output/fft.pdf"):
    N = len(data)  # サンプル数
    dt = (time_data[-1] - time_data[0]) / len(time_data)  # サンプリング間隔
    freq = np.linspace(0, 1.0 / dt, N)  # 周波数軸

    # 高速フーリエ変換（周波数信号に変換）
    F = np.fft.fft(data)

    # 正規化 + 交流成分2倍
    F = F/(N/2)
    F[0] = F[0]/2

    F = F[:N//2]
    F = np.abs(F)
    freq = freq[:N//2]

    if save_fig:
        fig = plt.figure()
        ax1 = fig.add_subplot(111)
        ax1.plot(freq, np.abs(F), label='|F(k)|')
        ax1.set_xlabel('Frequency', fontsize=12)
        ax1.set_ylabel('Amplitude', fontsize=12)
        ax1.grid()
        ax1.legend()
        plt.tight_layout()
        plt.savefig(file_name)
    return list(F).index(max(F)) + 1


# ord src
'''
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
'''