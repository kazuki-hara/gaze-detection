import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')
gaze_data_path = "/share/home/hara/workspace/fove/data/gaze/gaze.txt"

def read_gaze_data(data_path):
    with open(data_path) as f:
        lines = f.readlines()
    
    data = []
    for line in lines:
        lx ,ly, rx, ry, t = map(float, line[:-1].split())
        data.append({"lx": lx, "ly": ly, "rx": rx, "ry": ry, "t":t})
        #lx ,ly, rx, ry, t, l_num, r_num= map(float, line[:-1].split())
        #data.append({"lx": lx, "ly": ly, "rx": rx, "ry": ry, "t":t, "l_num":l_num, "r_num":r_num})
    return data

def make_xy_fig(data):
    fig =plt.figure()
    ax1 = fig.add_subplot(1, 2, 1)
    ax2 = fig.add_subplot(1, 2, 2)

    lx_list = [gaze["lx"] for gaze in data]
    ly_list = [gaze["ly"] for gaze in data]
    rx_list = [gaze["rx"] for gaze in data]
    ry_list = [gaze["ry"] for gaze in data]

    ax1.scatter(lx_list, ly_list, s=5, label="left eye")
    ax2.scatter(rx_list, ry_list, s=5, label="right eye")

    ax1.set_xlabel("x [px]")
    ax1.set_ylabel("y [px]")
    ax2.set_xlabel("x [px]")
    ax2.set_ylabel("y [px]")

    plt.tight_layout()
    plt.savefig("xy.png")


def xt_yt_fig(data):
    fig =plt.figure()
    ax1 = fig.add_subplot(2, 1, 1)
    ax2 = fig.add_subplot(2, 1, 2)

    lx_list = [gaze["lx"] for gaze in data]
    ly_list = [gaze["ly"] for gaze in data]
    rx_list = [gaze["rx"] for gaze in data]
    ry_list = [gaze["ry"] for gaze in data]
    t_list = [gaze["t"] for gaze in data]

    ax1.plot(t_list, lx_list, color="red", label="left eye")
    ax1.plot(t_list, rx_list, color="blue", label="right eye")
    ax2.plot(t_list, ly_list, color="red", label="left eye")
    ax2.plot(t_list, ry_list, color="blue", label="right eye")

    ax1.legend(loc=0)
    ax2.legend(loc=0)

    ax1.set_xlabel("t [s]")
    ax1.set_ylabel("x [px]")
    ax2.set_xlabel("t [s]")
    ax2.set_ylabel("y [px]")

    plt.tight_layout()
    plt.savefig("xyt.png")


def make_num_xy_fig(data):
    fig =plt.figure()
    ax1 = fig.add_subplot(1, 2, 1)
    ax2 = fig.add_subplot(1, 2, 2)

    lx_list = [gaze["lx"] for gaze in data]
    ly_list = [gaze["ly"] for gaze in data]
    rx_list = [gaze["rx"] for gaze in data]
    ry_list = [gaze["ry"] for gaze in data]
    l_num = [gaze["l_num"] for gaze in data]
    r_num = [gaze["r_num"] for gaze in data]

    for i in range(len(data)):
        if l_num[i] == 1:
            ax1.scatter(lx_list[i], ly_list[i], s=5, c="blue",label="num = 1")
        elif l_num[i] == 2:
            ax1.scatter(lx_list[i], ly_list[i], s=5, c="green",label="num = 2")
        else:
            ax1.scatter(lx_list[i], ly_list[i], s=5, c="red",label="num >= 3")
    for i in range(len(data)):
        if r_num[i] == 1:
            ax2.scatter(rx_list[i], ry_list[i], s=5, c="blue",label="num = 1")
        elif r_num[i] == 2:
            ax2.scatter(rx_list[i], ry_list[i], s=5, c="green",label="num = 2")
        else:
            ax2.scatter(rx_list[i], ry_list[i], s=5, c="red",label="num >= 3")


    ax1.set_xlabel("x [px]")
    ax1.set_ylabel("y [px]")
    ax2.set_xlabel("x [px]")
    ax2.set_ylabel("y [px]")

    plt.tight_layout()
    plt.savefig("xy.png")

if __name__ == "__main__":
    data = read_gaze_data(gaze_data_path)
    make_xy_fig(data)
    xt_yt_fig(data)