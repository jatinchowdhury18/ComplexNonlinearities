#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
from Hysteresis import Hysteresis_Process as HP
from HysteresisAA import Hysteresis_Process as HP_AA

#%%
def plotSineResponse (func, freq=100, seconds=1, fs=44100):
    n = np.arange (fs * seconds)
    x = np.sin (2 * np.pi * n * freq / fs)
    y = func (x)
    plt.plot (x[1000:], y[1000:])

def plotRisingSineResponse (func, freq=100, seconds=0.1, fs=44100):
    N = fs * seconds
    n = np.arange (N)
    x = np.sin (2 * np.pi * n * freq / fs) * (n/N)
    y = func (x)
    plt.plot (x, y)

#%%
def plotHysteresisRisingSine (drive, width, sat, fs=44100):
    gain = 1 # 1e4
    M_s = gain * (0.5 + 1.5*(1-sat)) # saturation
    a = M_s / (0.01 + 6*drive) #adjustable parameter
    alpha = 1.6e-3

    k = 30 * (1-0.5)**6 + 0.01 # Coercivity
    c = (1-width)**0.5 - 0.01 # Changes slope
    makeup = (1 + 0.6*width) / (0.5 + 1.5*(1-sat))
    plotRisingSineResponse (lambda x : makeup * HP (gain*x, M_s, a, alpha, k, c, 1/fs), fs=fs)

def plotHysteresisRisingSine_AA (drive, width, sat, fs=44100):
    gain = 1 # 1e4
    M_s = gain * (0.5 + 1.5*(1-sat)) # saturation
    a = M_s / (0.01 + 6*drive) #adjustable parameter
    alpha = 1.6e-3

    k = 30 * (1-0.5)**6 + 0.01 # Coercivity
    c = (1-width)**0.5 - 0.01 # Changes slope
    makeup = (1 + 0.6*width) / (0.5 + 1.5*(1-sat))
    plotRisingSineResponse (lambda x : makeup * HP_AA (gain*x, M_s, a, alpha, k, c, 1/fs), fs=fs)

#%%
plt.figure()
plotHysteresisRisingSine (1, 1, 0)

plt.figure()
plotHysteresisRisingSine_AA (1, 1, 0)

#%%
def L_I (x):
    if (np.abs (x) > 10 ** -4):
        return np.log (np.sinh (np.abs (x))) - np.log (np.abs (x))
    else:
        return x**2 / 6

N = 100
x = np.linspace (-12, 12, num=N)
y = np.zeros (N)

for n in range (N):
    y[n] = L_I (x[n])

plt.plot (x, y)

#%%
drive = [1] # [0, 0.25, 0.5, 0.75, 1]
width = [1]
saturation = [0] # [0, 0.25, 0.5, 0.75, 1]
fs = 44100

plt.figure()
for sat in saturation:
    for d in drive:
        for w in width:
            plotHysteresisRisingSine (d, w, sat)


#%%
n = np.linspace (0, 1)
y = n**0.5
plt.plot (n, y)

#%%
N = 2048
x = np.linspace (-5, 5, N)
table = 1.0 / np.tanh (x)

# plt.plot (x, y)
# plt.ylim (-1.1, 1.1)
# plt.xlim (-5, 5)

def cothLookup (x, table, N):
    if x > 5:
        return 1
    
    if x < -5:
        return -1

    index = int ((x + 5) * N / 10)
    return table[index]

xTest = np.linspace (-24, 24, 3000)
yTest = np.zeros (3000)
for n in range (3000):
    yTest[n] = cothLookup (xTest[n], table, N)

plt.plot (xTest, yTest)


#%%
