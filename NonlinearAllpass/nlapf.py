#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp

# %%
class DelayElement:
    '''
    One sample delay element
    '''
    def __init__(self):
        self._z = 0

    def reset(self):
        self._z = 0

    def write(self, x):
        self._z = x

    def read(self):
        return self._z

    def set_coefs(self, theta):
        pass

    def process(self, x):
        y = self._z
        self._z = x
        return y

# %%
class NLF(DelayElement):
    '''
    Normalized Ladder Filter
    '''
    def __init__(self, order=1):
        self._s = 1
        self._c = 0
        self._z = 0

        if order == 1:
            self._delay = DelayElement()
        else:
            self._delay = NLF(order - 1)

    def reset(self):
        self._z = 0
        self._delay.reset()

    def set_coefs(self, theta):
        self._s = np.sin(theta)
        self._c = np.cos(theta)
        self._delay.set_coefs(theta)

    def write(self, x):
        self._z = self._delay.process(x)

    def read(self):
        return self._z

    def process(self, x):
        y = self._s * x + self._c * self._delay.read()
        self._delay.write(self._c * x - self._s * self._delay.read())
        return y

# %%
fs = 44100
freq = 100
N = int(fs / 2)
x = np.sin(2 * np.pi * np.arange(N) * freq / fs)

plt.plot(x)

# %%
nlf = NLF(2)

y = np.zeros(N)
for n in range(N):
    nlf.set_coefs(1*x[n])
    y[n] = nlf.process(x[n])

plt.plot(y[:5000])

# %%
plt.semilogx(np.abs(np.fft.rfft(x)))
plt.semilogx(np.abs(np.fft.rfft(y)))

# %%
