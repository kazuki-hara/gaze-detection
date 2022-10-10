from PIL import ImageGrab

def screen_shot():
    output_image = ImageGrab.grab(bbox=(0, 0, 2560, 1440))
    output_image.save("screen_shot.jpg")

if __name__ == "__main__":
    screen_shot()