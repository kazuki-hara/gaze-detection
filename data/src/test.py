import cv2

THRESHOLD = 50

image1_path = "/share/home/hara/Data/fove/gaze/old_data/20211015/04/image/frame/0.png"


input_image = cv2.imread(image1_path)
image = cv2.cvtColor(input_image, cv2.COLOR_BGR2GRAY)
left_eye = image[:, 0:320]
right_eye = image[:, 321:]

def preprocess(image):
    res = []
    binary_image = cv2.threshold(image, THRESHOLD, 255, cv2.THRESH_BINARY)[1]
    res.append(binary_image)
    blur_1_image = cv2.blur(binary_image, (5, 5))
    res.append(blur_1_image)
    canny_image = cv2.Canny(blur_1_image, 80/2, 80)
    blur_2_image = cv2.blur(canny_image, (3, 3))
    res.append(blur_2_image)
    return res


def combine_image(left_image, right_image):
    dist = cv2.hconcat([left_image, right_image])
    cv2.imwrite("combine.png", dist)

if __name__ == "__main__":
    left_image = cv2.imread("/share/home/hara/workspace/fove/src/build/left.png")
    right_image = cv2.imread("/share/home/hara/workspace/fove/src/build/right.png")
    combine_image(left_image, right_image)