import cv2
import sys

width, height = (128, 64)

def pack(bw):
    buffer = bytearray()

    for y in range(height):
        byte_val = 0
        bit_count = 0

        for x in range(width):
            pixel = bw[y, x]
            bit = 1 if pixel == 255 else 0

            byte_val = (byte_val << 1) | bit
            bit_count += 1

            if bit_count == 8:
                buffer.append(byte_val)
                byte_val = 0
                bit_count = 0
    return buffer

def convert_frame(frame):
    frame = cv2.resize(frame, (width, height), interpolation=cv2.INTER_AREA)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    _, bw = cv2.threshold(gray, 70, 255, cv2.THRESH_BINARY)
    return pack(bw)

def convert(path, out):
    cap = cv2.VideoCapture(path)
    if not cap.isOpened():
        print("OH NO!!")
        return

    fps = cap.get(cv2.CAP_PROP_FPS)
    assert fps == 30 or fps == 60

    with open(out, "wb") as out:
        frame_index = 0

        while True:
            ret, frame = cap.read()
            if not ret:
                break
            if fps == 60 and frame_index % 2 == 1:
                frame_index += 1
                continue
            
            packed = convert_frame(frame)
            out.write(packed)
            
            if frame_index % 50 == 0:
                print(f'Processed frame {frame_index}')
            frame_index += 1

    cap.release()
    print('Job done')

if __name__ == "__main__":
    if (len(sys.argv) == 3):
        convert(sys.argv[1], sys.argv[2])
