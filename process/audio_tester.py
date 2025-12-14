import sys
import numpy as np
import struct
import sounddevice as sd

def read_audio(path):
    seq = []

    with open(path, "rb") as out:
        while True:
            data = out.read(8)
            if len(data) < 8:
                break
            hz, length_ms = struct.unpack("<II", data)
            seq.append((hz, length_ms))
    return seq

def generate_tone(hz, length_ms, sr=44100):
    samples = int(sr * length_ms / 1000)

    if hz == 0:
        return np.zeros(samples, dtype=np.float32)

    t = np.arange(samples) / sr
    wave = np.sign(np.sin(2 * np.pi * hz * t))
    return wave.astype(np.float32)

def play_seq(seq, sr=44100):
    audio = []

    for hz, length_ms in seq:
        tone = generate_tone(hz, length_ms, sr)
        audio.append(tone)

    audio = np.concatenate(audio)
    sd.play(audio, samplerate=sr)
    sd.wait()

if __name__ == "__main__":
    if len(sys.argv) == 2:
        seq = read_audio(sys.argv[1])
        print(f"Loaded {len(seq)} tones")
        play_seq(seq)
