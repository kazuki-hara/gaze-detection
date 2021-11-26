import cv2

image_path = "/share/home/hara/Data/fove/gaze/old_data/20211015/01/image/frame/0.png"

image = cv2.imread(image_path)
height, width, channels = image.shape[:3]
print("width: " + str(width))
print("height: " + str(height))
for i in range(240):
    image[i, 105] = [255,0,0]
    image[i, 183] = [255,0,0]
cv2.imwrite("test.png", image)