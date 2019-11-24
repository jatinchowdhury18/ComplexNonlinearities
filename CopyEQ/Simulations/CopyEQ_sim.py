#%%
import numpy as np
import matplotlib.pyplot as plt
import audio_dspy as adsp
import scipy.signal as signal


# %%
noise = np.random.uniform (-1, 1, 1024)

# %%
N = 1024
freq = 100
fs = 44100
sin = np.sin(2 * np.pi * np.arange(N) * freq / fs)

# %%
y, e, w = adsp.LMS (noise, sin, 0.1, 128)
plt.plot(y)
print(w)

# %%
