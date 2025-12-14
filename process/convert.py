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

def dominant_freq(frame, sr):
    window = np.hanning(len(frame))
    spectrum = np.abs(np.fft.rfft(frame * window))
    freqs = np.fft.rfftfreq(len(frame), 1 / sr)

    peak = np.argmax(spectrum)
    return freqs[peak]

def quantize(f):
    if f < 200:
        return 0
    if f > 10000:
        return 20000
    return int(f)

def compress(freqs, frame_ms):
    result = []

    prev = freqs[0]
    length = frame_ms

    for f in freqs[1:]:
        if f == prev:
            length += frame_ms
        else:
            result.append({"hz": prev, "length_ms": length})
            prev = f
            length = frame_ms

    result.append({"hz": prev, "length_ms": length})
    return result

def aud_convert(path, out):
    FRAME_MS = 30
    FRAME_SIZE = int(8000 * FRAME_MS / 1000)
    HOP_SIZE = FRAME_SIZE

    freqs = []

    to_wav(path)
    y, sr = librosa.load(path + ".wav", sr=None, mono=True)
    
    for i in range(0, len(y) - FRAME_SIZE, HOP_SIZE):
        frame = y[i:i + FRAME_SIZE]

        if np.max(np.abs(frame)) < 0.01:
            freqs.append(0)
            continue

        f = dominant_freq(frame, sr)
        freqs.append(f)

    freqs = [quantize(f) for f in freqs]
    seq = compress(freqs, FRAME_MS)

    with open(out, "wb") as out:
        for s in seq:
            out.write(struct.pack(
                "<II",
                int(s["hz"]),
                int(s["length_ms"])
            ))
    print("Audio processed!")

if __name__ == "__main__":
    if (len(sys.argv) == 3):
        aud_convert(sys.argv[1], sys.argv[2] + ".audbin")
        vid_convert(sys.argv[1], sys.argv[2] + ".imgbin")

