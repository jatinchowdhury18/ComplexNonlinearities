# %%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp

# %%
N = 500
fs = 44100
freq = 2000
x = np.sin(2 * np.pi * freq / fs * np.arange(N))

plt.plot(x)

# %%
X = 20*np.log10(adsp.normalize(np.abs(np.fft.rfft(x))))
plt.plot(X)

# %%
eq = adsp.EQ(fs)
Qs = adsp.butter_Qs(6)
print(Qs)
eq.add_LPF(1500, Qs[0])
eq.add_LPF(1500, Qs[1])
eq.add_LPF(1500, Qs[2])

def get_square_half_freq(sig):
    y = np.copy(sig)

    rising = True
    last_x = 0
    output = 1
    switch_count = 0
    for n, x in enumerate(sig):
        y[n] = output

        if rising == True and x < last_x:
            switch_count += 1
            rising = False
        elif rising == False and x > last_x:
            switch_count += 1
            rising = True

        if switch_count == 2:
            output = 1 if output == -1 else -1
            switch_count = 0

        last_x = x
    return y

# %%
y = eq.process_block(get_square_half_freq(x))
plt.plot(y)

# %%
Y = 20*np.log10(adsp.normalize(np.abs(np.fft.rfft(y))))
plt.plot(X)
plt.plot(Y)

# %%
sum = x + y
plt.plot(sum)

# %%
