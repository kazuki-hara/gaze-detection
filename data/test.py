import cv2

THRESHOLD = 50

image1_path = "./gaze/old_data/20211015/04/image/frame/0.png"


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


if __name__ == "__main__":
    cv2.imwrite("left.png", left_eye)
    left_eye_images = preprocess(left_eye)
    cv2.imwrite("left_binary.png", left_eye_images[0])
    cv2.imwrite("left_blur1.png", left_eye_images[1])
    cv2.imwrite("left_blur2.png", left_eye_images[2])


    cv2.imwrite("right.png", right_eye)
    right_eye_images = preprocess(right_eye)
    cv2.imwrite("right_binary.png", right_eye_images[0])
    cv2.imwrite("right_blur1.png", right_eye_images[1])
    cv2.imwrite("right_blur2.png", right_eye_images[2])