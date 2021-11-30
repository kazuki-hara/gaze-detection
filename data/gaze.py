import matplotlib.pyplot as plt
import matplotlib
from matplotlib.backends.backend_pdf import PdfPages
matplotlib.use('Agg')
preprocess_gaze_data_path = "/share/home/hara/Data/fove/gaze/old_data/20211015/01/gaze.txt"
non_preprocess_data_path = "/share/home/hara/workspace/fove/src/build/gaze.txt"

range_400_data_path = "/share/home/hara/Data/fove/gaze/gaze.txt"

def read_gaze_data(data_path):
    with open(data_path) as f:
        lines = f.readlines()
    
    data = []
    for line in lines:
        lx ,ly, rx, ry, t= map(float, line[:-1].split())
        if lx!= -1 and ly and -1 and rx!= -1 and ry != -1:
            data.append({"lx": lx, "ly": ly, "rx": rx, "ry": ry, "t":t})
        #lx ,ly, rx, ry, t, l_num, r_num= map(float, line[:-1].split())
        #data.append({"lx": lx, "ly": ly, "rx": rx, "ry": ry, "t":t, "l_num":l_num, "r_num":r_num})
    time_list = [gaze["t"] for gaze in data]
    return data, time_list

def read_non_preprocess_gaze_data(non_preprocess_data_path, time_list):
    with open(non_preprocess_data_path) as f:
        lines = f.readlines()
    
    data = []
    for i,line in enumerate(lines):
        lx, ly, rx, ry, index = map(float, line[:-1].split())
        if lx!= -1 and ly and -1 and rx!= -1 and ry != -1:

            data.append({"lx": lx, "ly": ly, "rx": rx, "ry": ry, "t":time_list[i]})
        #lx ,ly, rx, ry, t, l_num, r_num= map(float, line[:-1].split())
        #data.append({"lx": lx, "ly": ly, "rx": rx, "ry": ry, "t":t, "l_num":l_num, "r_num":r_num})
    return data

def make_xy_fig(data, side):
    fig =plt.figure()
    ax = fig.add_subplot()
    lx_list = [gaze["lx"] for gaze in data]
    ly_list = [gaze["ly"] for gaze in data]
    rx_list = [gaze["rx"] for gaze in data]
    ry_list = [gaze["ry"] for gaze in data]

    if side == "l":
        ax.scatter(lx_list, ly_list, s=5, label="left eye")
    else:
        ax.scatter(rx_list, ry_list, s=5, label="right eye")

    ax.set_xlabel("x [px]")
    ax.set_ylabel("y [px]")
    fig.subplots_adjust(right=0.95, top=0.95, wspace=0.15, hspace=0.15)
    if side == "l":
        pdf = PdfPages("left_xy.pdf")
        pdf.savefig()
    else:
        pdf = PdfPages("right_xy.pdf")
        pdf.savefig()
    pdf.close()


def xt_yt_fig(data, pos):
    fig =plt.figure()
    ax = fig.add_subplot()
    lx_list = [gaze["lx"] for gaze in data]
    ly_list = [gaze["ly"] for gaze in data]
    rx_list = [gaze["rx"] for gaze in data]
    ry_list = [gaze["ry"] for gaze in data]
    t_list = [gaze["t"] for gaze in data]
    if pos == "x":
        ax.scatter(t_list, lx_list, s=2,color="red", label="left eye")
        ax.scatter(t_list, rx_list, s=2,color="blue", label="right eye")
    else:
        ax.scatter(t_list, ly_list, s=2,color="red", label="left eye")
        ax.scatter(t_list, ry_list, s=2,color="blue", label="right eye")

    ax.legend(loc=0)
    ax.set_xlabel("t [s]")
    ax.set_ylim(50, 250)
    if pos == "x":
        ax.set_ylabel("x [px]")
    else:
        ax.set_ylabel("y [px]")
    fig.subplots_adjust(right=0.95, top=0.95, wspace=0.15, hspace=0.15)
    if pos == "x":
        pdf = PdfPages("output/xt.pdf")
        pdf.savefig()
        plt.savefig("output/xt.png")
    else:
        pdf = PdfPages("output/yt.pdf")
        pdf.savefig()
        plt.savefig("output/yt.png")
    pdf.close()


def make_num_xy_fig(data, side):
    fig =plt.figure()
    ax = fig.add_subplot()
    lx_list = [gaze["lx"] for gaze in data]
    ly_list = [gaze["ly"] for gaze in data]
    rx_list = [gaze["rx"] for gaze in data]
    ry_list = [gaze["ry"] for gaze in data]
    l_num = [gaze["l_num"] for gaze in data]
    r_num = [gaze["r_num"] for gaze in data]

    flag_1 = True
    flag_2 = True
    flag_3 = True
    if side == "l":
        for i in range(len(data)):
            if l_num[i] == 1:
                if flag_1:
                    ax.scatter(lx_list[i], ly_list[i], s=5, c="blue",label="num = 1")
                    flag_1 = False
                else:
                    ax.scatter(lx_list[i], ly_list[i], s=5, c="blue")
            elif l_num[i] == 2:
                if flag_2:
                    ax.scatter(lx_list[i], ly_list[i], s=5, c="green",label="num = 2")
                    flag_2 = False
                else:
                    ax.scatter(lx_list[i], ly_list[i], s=5, c="green")
            else:
                if flag_3:
                    ax.scatter(lx_list[i], ly_list[i], s=5, c="red",label="num >= 3")
                    flag_3 = False
                else:
                    ax.scatter(lx_list[i], ly_list[i], s=5, c="red")
    if side == "r":   
        for i in range(len(data)):
            if r_num[i] == 1:
                if flag_1:
                    ax.scatter(rx_list[i], ry_list[i], s=5, c="blue",label="num = 1")
                    flag_1 = False
                else:
                    ax.scatter(rx_list[i], ry_list[i], s=5, c="blue")
            elif r_num[i] == 2:
                if flag_2:
                    ax.scatter(rx_list[i], ry_list[i], s=5, c="green",label="num = 2")
                    flag_2 = False
                else:
                    ax.scatter(rx_list[i], ry_list[i], s=5, c="green")
            else:
                if flag_3:
                    ax.scatter(rx_list[i], ry_list[i], s=5, c="red",label="num >= 3")
                    flag_3 = False
                else:
                    ax.scatter(rx_list[i], ry_list[i], s=5, c="red")

    ax.set_xlabel("x [px]")
    ax.set_ylabel("y [px]")
    ax.legend(loc=0)
    fig.subplots_adjust(right=0.95, top=0.95, wspace=0.15, hspace=0.15)
    if side == "l":
        plt.savefig("l_num.png")
    else:
        plt.savefig("r_num.png")


if __name__ == "__main__":
    data, time_list = read_gaze_data("/share/home/hara/workspace/fove/src/build/gaze.txt")
    make_xy_fig(data, "l")
    make_xy_fig(data, "r")
    xt_yt_fig(data, "x")
    xt_yt_fig(data, "y")