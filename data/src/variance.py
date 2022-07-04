from math import sqrt

non_preprocess_data_path = "/share/home/hara/workspace/fove/src/build/gaze.txt"
preprocess_data_path = "/share/home/hara/Data/fove/gaze/old_data/20211015/01/gaze.txt"

rad_per_pix = 60 / 78

from statistics import stdev, variance, median

threshold = [i*5 for i in range(25)]

def read_gaze_data(data_path):
    with open(data_path) as f:
        lines = f.readlines()
    
    preprocess_data = []
    index = []
    for i, line in enumerate(lines):
        line_list = list(map(float, line[:-1].split()))
        lx = line_list[0]
        ly = line_list[1]
        rx = line_list[2]
        ry = line_list[3]
        preprocess_data.append({"lx": lx, "ly": ly, "rx": rx, "ry": ry})
        if len(threshold) != 0:
            if line_list[4] >= threshold[0]:
                index.append(i)
                threshold.pop(0)
    return preprocess_data, index


def read_gaze_data_time_data(gaze_data_path, time_data_path):
    with open(gaze_data_path) as f:
        gaze_data_lines = f.readlines()
    with open(time_data_path) as tf:
        time_data_lines = tf.readlines()

    preprocess_data = []
    index = []
    i = 0
    for gaze, time in zip(gaze_data_lines, time_data_lines):
        gaze_list = list(map(float, gaze[:-1].split()))
        lx = gaze_list[0]
        ly = gaze_list[1]
        rx = gaze_list[2]
        ry = gaze_list[3]
        t = float(time[:-1])
        preprocess_data.append({"lx": lx, "ly": ly, "rx": rx, "ry": ry})
        if len(threshold) != 0:
            if gaze_list[4] >= threshold[0]:
                index.append(i)
                threshold.pop(0)
        i+=1
    return preprocess_data, index




def main(datas, index):
    # if start == None:
    #     start = 0
    # if end == None:
    #     end = len(datas)
    # num = end - start

    lx = 0
    ly = 0
    rx = 0
    ry = 0
    num_all = 0
    
    for i in range(4):
        start = index[i*2]
        end = index[i*2+1]

        data = datas[start:end]
        num = len(data)
        num_all += num

        lx_list = [gaze["lx"] for gaze in data]
        ly_list = [gaze["ly"] for gaze in data]
        rx_list = [gaze["rx"] for gaze in data]
        ry_list = [gaze["ry"] for gaze in data]

        average = {
            "lx": sum(lx_list)/num,
            "ly": sum(ly_list)/num,
            "rx": sum(rx_list)/num,
            "ry": sum(ry_list)/num,
        }



        for i in range(num):
            lx += ((lx_list[i] - average["lx"])*rad_per_pix)**2
            ly += ((ly_list[i] - average["ly"])*rad_per_pix)**2
            rx += ((rx_list[i] - average["rx"])*rad_per_pix)**2
            ry += ((ry_list[i] - average["ry"])*rad_per_pix)**2
        
    print(sqrt(lx/num_all), sqrt(ly/num_all), sqrt(rx/num_all), sqrt(ry/num_all))
        #print(variance(lx_list), variance(ly_list), variance(rx_list), variance(ry_list))

if __name__ == "__main__":
    #data, index = read_gaze_data(preprocess_data_path)
    data, index = read_gaze_data_time_data("/share/home/hara/Data/fove/pupil/hara/200/pupil_ellipse.txt", "/share/home/hara/Data/fove/pupil/hara/200/time0.txt")
    #main(data, 700, None)
    main(data, index)
    data, _ = read_gaze_data("/share/home/hara/Data/fove/pupil/hara/200/pupil_ellipse.txt")
    #main(data, 700, None)
    main(data, index)
