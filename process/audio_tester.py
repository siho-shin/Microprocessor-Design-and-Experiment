import sys
import numpy as np
import struct
import sounddevice as sd

def play(path, sr=8000):
    with open(path, "rb") as f:
        data = np.frombuffer(f.read(), dtype=np.uint8)

    audio = (data.astype(np.float32) - 128.0) / 128.0

    sd.play(audio, samplerate=sr)
    sd.wait()

if __name__ == "__main__":
    if len(sys.argv) == 2:
        play(sys.argv[1])
