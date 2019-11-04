#%%
import numpy as np
import scipy.signal as signal
from scipy.io import wavfile
import matplotlib.pyplot as plt
from matplotlib import animation
import audio_dspy as adsp

# %%
class Biquad:
    def __init__ (self):
        self.z = np.zeros (3)
        self.b = np.array ([1, 0, 0])
        self.a = np.array ([1, 0, 0])
        self.saturator = lambda x : x

        # state
        self.x1 = 0
        self.x2 = 0
    
    def setCoefs (self, b, a):
        assert (np.size (b) == np.size (self.b))
        assert (np.size (a) == np.size (self.a))
        self.b = np.copy (b)
        self.a = np.copy (a)

    # Direct-Form II, transposed
    def processSample (self, x):
        y = self.z[1] + self.b[0]*x

        self.x1 = self.z[2] + self.b[1]*x - self.a[1]*y
        self.x2 = self.b[2]*x - self.a[2]*y

        self.z[1] = self.saturator (self.x1)
        self.z[2] = self.saturator (self.x2)
        return y

    def processBlock (self, block):
        for n in range (len (block)):
            block[n] = self.processSample (block[n])
        return block

# %%
fs = 44100
N = 441*10
freq = 100
x = np.sin(2 * np.pi * np.arange(N) * freq / fs)

# N = 10000
# fs, x = wavfile.read('C:/Users/jatin/Desktop/drums_mono.wav')

# x = adsp.normalize(x[:N])

plt.plot(x)

# %%
bq = Biquad()
b, a = adsp.design_lowshelf (1000, 10, 3, fs)
bq.setCoefs(b, a)
bq.saturator = lambda x : np.tanh(x)

# %%
y = np.zeros(N)
x1 = np.zeros(N)
x2 = np.zeros(N)

for n in range(N):
    y[n] = bq.processSample(2 * x[n])
    x1[n] = bq.x1
    x2[n] = bq.x2

# %%
plt.plot(y)
plt.plot(x)

# %%
plt.plot(x1, x2)
# plt.xlim (-1, 1)
# plt.ylim (-1, 1)

# %%
fig = plt.figure()
size = 0.8
ax = plt.axes(xlim=(-size, size), ylim=(-size, size))
line, = ax.plot([], [], lw=2)
dot, = ax.plot([], [], 'ro')

def init():
    line.set_data([], [])
    dot.set_data([], [])
    return line, dot

def animate(i, x1, x2):
    line.set_data(x1[:i*2], x2[:i*2])
    dot.set_data(x1[i*2], x2[i*2])
    return line, dot

anim = animation.FuncAnimation(fig, animate, init_func=init, fargs=(x1, x2),
                               frames=int(N/8), interval=50, blit=True)

anim.save('test_anim.mp4', fps=30, extra_args=['-vcodec', 'libx264'])

# %%
