import cv2
import sys

def convert(path, out):
    img = cv2.imread(path)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    _, bw = cv2.threshold(gray, 70, 255, cv2.THRESH_BINARY)
    cv2.imwrite(out, bw)

if __name__ == "__main__":
    if (len(sys.argv) == 3):
        convert(sys.argv[1], sys.argv[2])
