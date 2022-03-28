import numpy as np
INF = 10e9

data_dir = "/share/home/hara/Data/fove/tmp/"
pupil_pos_txt_path = data_dir + "gaze.txt"
time_txt_path = data_dir + "time.txt"

class Calibration:
    def __init__(self):
        # 生データ読み込み
        self.pupil_list = []
        self.time_list = []

        # 注視中のデータを抽出
        self.gaze_begin_time = [3.0, 11.0, 18.0, 25.0, 32.0, INF]
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
    

    def calculation_pupil_to_display(self):
        # x方向
        left_eye_x = [0]
        left_eye_x = [data[0] for data in self.pupil_pos[1:]]
        right_eye_x = [data[2] for data in self.pupil_pos]
        display = [0, 400 , 0, -400, 0]
        
        # y方向




if __name__ == "__main__":
    calib = Calibration()
    calib.read_pupil_data(pupil_pos_txt_path)
    calib.read_time_data(time_txt_path)
    calib.extract_gaze_data()
    calib.remove_outliers()

