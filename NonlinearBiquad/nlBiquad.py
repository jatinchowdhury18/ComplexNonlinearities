#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import FilterShapes as filters

#%%
#%%
def chirpLog (f0, f1, duration, fs):
    N = int (duration * fs)
    n = np.arange (N)
    
    beta = N / np.log(f1 / f0)
    phase = 2 * np.pi * beta * f0 * (pow(f1 / f0, n / N) - 1.0)
    phi = np.pi / 180
    
    return np.cos ((phase + phi)/fs)

def chrp2ir (ss, rs):
    N = max (len (rs), len (ss))
    SS = np.fft.fft (ss, n=N)
    RS = np.fft.fft (rs, n=N)
    
    H = RS/SS
    h = np.real (np.fft.ifft (H))
    return h

#%%
class Biquad:
    def __init__ (self):
        self.z = np.zeros (3)
        self.b = np.array ([1, 0, 0])
        self.a = np.array ([1, 0, 0])
        
        self.saturator = lambda x : x

        self.bLambdas = []
        self.aLambdas = []
        for _ in range (3):
            self.bLambdas.append (lambda x, b : x*b)
            self.aLambdas.append (lambda x, a : x*a)
    
    def setCoefs (self, b, a):
        assert (np.size (b) == np.size (self.b))
        assert (np.size (a) == np.size (self.a))

        self.b = np.copy (b)
        self.a = np.copy (a)

    # Direct-Form II, transposed
    def processSample (self, x):
        y = self.z[1] + self.bLambdas[0] (x, self.b[0])

        self.z[1] = self.saturator (self.z[2] +  self.bLambdas[1] (x, self.b[1]) - self.aLambdas[1] (y, self.a[1]))
        self.z[2] = self.saturator (self.bLambdas[2] (x, self.b[2]) - self.aLambdas[2] (y, self.a[2]))

        return y

    def processBlock (self, block):
        for n in range (len (block)):
            block[n] = self.processSample (block[n])
        
        return block

#%%
fs = 44100
b, a = filters.calcCoefsLowShelf (1000, 10, 2, fs)

normalBQ = Biquad()
normalBQ.setCoefs (b, a)

def softClip (x):
    if x > 1.0:
        return 1.0
    
    if x < -1.0:
        return -1.0

    return 1.5 * (x - x**3/3)

nlBQ = Biquad()
nlBQ.setCoefs (b, a)
nlBQ.saturator = lambda x : np.tanh (x)
# nlBQ.bLambdas[0] = lambda x, b : np.tanh (x) * b
# nlBQ.bLambdas[1] = lambda x, b : np.tanh (x) * b
# nlBQ.bLambdas[2] = lambda x, b : np.tanh (x) * b

#%%
def plotFilterResponse (biquad, fs, gain=0.1):
    x = gain*chirpLog (20, 20000, 1, fs)
    y = biquad.processBlock (np.copy (x))
    N = len (x)

    h = chrp2ir (x, y)

    f = np.linspace (0, fs/2, num=N/2+1)
    H = np.fft.rfft (h)
    plt.semilogx (f, 20 * np.log10 (np.abs (H)))

def plotStaticCurve (biquad, gain=0.1):
    x = gain*chirpLog (20, 20000, 1, fs)
    y = biquad.processBlock (np.copy (x))
    plt.plot (x, y)

plt.figure()
plotFilterResponse (normalBQ, fs)
plotFilterResponse (nlBQ, fs)
plt.xlim (20, 20000)

plt.title ('Nonlinear Low Shelf')
plt.xlabel ('Frequency [Hz]')
plt.ylabel ('Magnitude [dB]')
plt.legend (['Linear', 'Nonlinear'])

# plt.ylim (-10, 10)

# for gain in [0.0001, 0.1, 1, 2.5]:
#     plotFilterResponse (nlBQ, fs, gain)
# plt.ylim (-20)

#%%
