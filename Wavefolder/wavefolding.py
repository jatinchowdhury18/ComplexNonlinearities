#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp


# %%
def tri_wave(x, freq, fs):
    p = float((1/freq) * fs)
    x = x + p/4
    return 4 * np.abs((x / p) - np.floor((x / p) + 0.5)) - 1

def sine_wave(x, freq, fs):
    return np.sin(2 * np.pi * x * freq / fs)

def tri_tanh(x, amt, freq, fs):
    return np.tanh(x) - amt * tri_wave(x, freq, fs)

def sine_tanh(x, amt, freq, fs):
    return np.tanh(x) - amt * sine_wave(x, freq, fs)

# %%
fs = 44100
N = 441*4
freq = 100

x = 1.5 * np.sin(2 * np.pi * np.arange(N) * freq / fs)

#%%
y = sine_tanh(x, 0.1, fs/4*4, fs)
plt.plot(x)
plt.plot(y)

# %%
adsp.plot_dynamic_curve(lambda x : sine_tanh(x, 0.1, fs/2, fs))

# %%
adsp.plot_harmonic_response(lambda x : sine_tanh(x, 0.1, fs/2, fs), gain=1.5)

# %%
class WaveFolder:
    def __init__(self):
        self.y1 = 0
        self.fb = lambda _ : 0
        self.ff = lambda x : x
        self.wave = lambda x : x

    def process(self, x):
        z = self.ff(x) + self.fb(self.y1)
        y = z + self.wave(x)

        self.y1 = y
        return y

    def process_block(self, block):
        out = np.copy(block)

        for n, _ in enumerate(block):
            out[n] = self.process(block[n])

        return out


# %%
WF = WaveFolder()
WF.ff = lambda x : np.tanh(x)
WF.fb = lambda x : 2*np.tanh(x)
WF.wave = lambda x : -0.1*sine_wave(x, fs/2, fs)

adsp.plot_dynamic_curve(lambda x : WF.process_block(x))

WF2 = WaveFolder()
WF2.ff = lambda x : 0 # np.tanh(x)
WF2.fb = lambda x : 0 # np.tanh(x)
WF2.wave = lambda x : sine_wave(x, fs/2, fs)

# adsp.plot_static_curve(lambda x : WF2.process_block(x))

# adsp.plot_static_curve(lambda x : sine_wave(x, fs/2, fs))

# y = WF.process_block(x)
# plt.plot(x)
# plt.plot(y)

# %%
