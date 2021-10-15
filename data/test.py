import cv2

THRESHOLD = 50

image1_path = "./image/126.png"
image2_path = "./image/111.png"

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
    return res


if __name__ == "__main__":
    cv2.imwrite("left.png", left_eye)
    left_eye_images = preprocess(left_eye)
    cv2.imwrite("left_binary.png", left_eye_images[0])
    cv2.imwrite("left_blur1.png", left_eye_images[1])