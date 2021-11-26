import cv2
INF = 10e9
sample1_path = "/share/home/hara/Data/fove/gaze/old_data/20211015/01/image/right/713.png"
sample2_path = "/share/home/hara/Data/fove/gaze/old_data/20211015/01/image/right/534.png"


def search_green(image):
    min_x = INF
    min_y = INF
    max_x = 0
    max_y = 0
    for i in range(image.shape[0]):
        for j in range(image.shape[1]):
            if all(image[i][j] == [0, 255, 0]):
                min_x = min(min_x, j)
                max_x = max(max_x, j)
                min_y = min(min_y, i)
                max_y = max(max_y, i)
    return min_x, max_x, min_y, max_y


def radius(image):



if __name__ == "__main__":
    sample = cv2.imread(sample1_path)
    search_green(sample)