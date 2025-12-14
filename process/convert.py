import cv2
import sys
import struct

import subprocess
import librosa
import numpy as np

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

def vid_convert_frame(frame):
    frame = cv2.resize(frame, (width, height), interpolation=cv2.INTER_AREA)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    _, bw = cv2.threshold(gray, 70, 255, cv2.THRESH_BINARY)
    return pack(bw)

def vid_convert(path, out):
    cap = cv2.VideoCapture(path)
    if not cap.isOpened():
        print("OH NO!!")
        return

    fps = cap.get(cv2.CAP_PROP_FPS)

    with open(out, "wb") as out:
        frame_index = 0

        while True:
            ret, frame = cap.read()
            if not ret:
                break
            if fps == 60 and frame_index % 2 == 1:
                frame_index += 1
                continue
            
            packed = vid_convert_frame(frame)
            out.write(packed)
            
            if frame_index % 50 == 0:
                print(f'Processed frame {frame_index}')
            frame_index += 1

    cap.release()
    print('Job done')

def to_wav(path):
    subprocess.run([
        "ffmpeg",
        "-y",
        "-i", path,
        "-ac", "1",
        "-ar", "8000",
        "-vn",
        path + ".wav"
    ], check=True)


def aud_convert(path, out):
    to_wav(path)
    y, sr = librosa.load(path + ".wav", sr=None, mono=True)
    
    y = y / np.max(np.abs(y))
    pcm = ((y + 1.0) * 127.5).astype(np.uint8)

    with open(out, "wb") as f:
        f.write(pcm.tobytes())

    print("Audio processed!")

if __name__ == "__main__":
    if (len(sys.argv) == 3):
        aud_convert(sys.argv[1], sys.argv[2] + ".audbin")
        vid_convert(sys.argv[1], sys.argv[2] + ".imgbin")

