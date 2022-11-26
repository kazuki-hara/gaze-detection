import numpy as np
import matplotlib.pyplot as plt

data_dir = "/share/home/hara/Data/fove/tmp/"

gaze_data_file_path = data_dir + "gaze1.txt"
pupil_data_file_path = data_dir + "pupil1.txt"
time_data_file_path = data_dir + "time1.txt"
key_data_file_path = data_dir + "key.txt"




class DataGetter:
    def __init__(self) -> None:
        self.gaze_data = self.read_gaze_data(gaze_data_file_path)
        self.pupil_data = self.read_pupil_data(pupil_data_file_path)
        self.time_data = self.read_time_data(time_data_file_path)
        self.key_data = self.read_key_data(key_data_file_path)

    def read_gaze_data(self, gaze_data_file_path):
        f = open(gaze_data_file_path, 'r')
        txt_data = f.readlines()
        data = []
        for line in txt_data:
            data.append(list(map(float, line[:-1].split(" "))))
        return data

    def read_pupil_data(self, pupil_data_file_path):
        f = open(pupil_data_file_path, 'r')
        txt_data = f.readlines()
        data = []
        for line in txt_data:
            data.append(list(map(float, line[:-1].split(" "))))
        return data

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
        print(len(data))
        return data

    def remove_error_data(self):
        error_data_index = []
        for i, (gaze, pupil, time) in enumerate(zip(self.gaze_data, self.pupil_data, self.time_data)):
            if -1 in gaze or -1 in pupil:
                error_data_index.append(i)
        
        for i in error_data_index[::-1]:
            del self.gaze_data[i]
            del self.pupil_data[i]
            del self.time_data[i]

    def make_time_series_figure(self, y_data, label, file_name):
        # gaze, time
        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.plot(self.time_data, y_data, label=label)

        for i in range(len(self.key_data)//2):
            start = self.key_data[i*2+1]
            end = self.key_data[i*2+2]
            ax.axvspan(start, end, color="gray", alpha=0.3)
        fig.savefig(file_name)

            



if __name__ == "__main__":
    data_getter = DataGetter()
    data_getter.remove_error_data()
    data_getter.make_time_series_figure([x[0] for x in data_getter.gaze_data], "lx", "gaze_lx.pdf")
    data_getter.make_time_series_figure([x[0] for x in data_getter.pupil_data], "lx", "pupil_lx.pdf")