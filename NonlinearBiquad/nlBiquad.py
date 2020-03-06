#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import FilterShapes as filters
plt.style.use('dark_background')

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
    
    def setCoefs (self, b, a):
        assert (np.size (b) == np.size (self.b))
        assert (np.size (a) == np.size (self.a))
        self.b = np.copy (b)
        self.a = np.copy (a)

    # Direct-Form II, transposed
    def processSample (self, x):
        y = self.z[1] + self.b[0]*x
        self.z[1] = self.saturator (self.z[2] + self.b[1]*x - self.a[1]*y)
        self.z[2] = self.saturator (self.b[2]*x - self.a[2]*y)
        return y

    def processBlock (self, block):
        for n in range (len (block)):
            block[n] = self.processSample (block[n])
        return block

#%%
fs = 44100
b, a = filters.calcCoefsLPF2 (1000, 10, fs)

normalBQ = Biquad()
normalBQ.setCoefs (b, a)

def softClip (x):
    if x > 1.0:
        return 1.0
    
    if x < -1.0:
        return -1.0

    return x - x**3/3

nlBQ = Biquad()
nlBQ.setCoefs (b, a)
nlBQ.saturator = lambda x : np.sin(x) # np.tanh (x)
# nlBQ.bLambdas[0] = lambda x, b : np.tanh (x) * b
# nlBQ.bLambdas[1] = lambda x, b : np.tanh (x) * b
# nlBQ.bLambdas[2] = lambda x, b : np.tanh (x) * b

#%%
N = 1000
x_0 = 10
x = np.ones (N) * x_0
y = nlBQ.processBlock (x)

# plt.plot (y)
# print (y[N-1])

# guess = (-1.0 / nlBQ.a[2]) * (1.0 - nlBQ.b[2] * x_0)
# print (guess)

#%%
def plotFilterResponse (biquad, fs, gain=0.1):
    x = gain*chirpLog (20, 20000, 1, fs)
    y = biquad.processBlock (np.copy (x))
    N = len (x)

    plt.plot(np.arange(N)/fs, y)

    # h = chrp2ir (x, y)

    # f = np.linspace (0, fs/2, num=N//2+1)
    # H = np.fft.rfft (h)
    # plt.semilogx (f, 20 * np.log10 (np.abs (H)))

def plotStaticCurve (biquad, gain=0.1):
    x = gain*chirpLog (20, 20000, 1, fs)
    y = biquad.processBlock (np.copy (x))
    plt.plot (x, y)

plt.figure()
plotFilterResponse (normalBQ, fs)
plotFilterResponse (nlBQ, fs)
# plt.xlim (20, 20000)
# plt.ylim(-30)

plt.title ('Nonlinear Lowpass Sine Sweep (sine)')
plt.xlabel ('Time [Seconds]')
plt.ylabel ('Magnitude')
plt.legend (['Linear', 'Nonlinear'])

# plt.ylim (-10, 10)

# for gain in [0.0001, 0.1, 1, 2.5]:
#     plotFilterResponse (nlBQ, fs, gain)
# plt.ylim (-20)

#%%
# N = 1000
# x = np.linspace (-3, 3, N)
# y = np.copy (x)

# def tanhClip (x, sat):
#     return sat * np.tanh (x / sat)

# def softClip (x,  sat):
#     if isinstance (x, np.ndarray):
#         t = np.copy (x)
#         for n in range (len (x)):
#             t[n] = softClip (t[n], sat)
#         return t
    
#     if (x > 1.5): return 1
#     if (x < -1.5): return -1
#     x /= 1.5
#     return (x - x*x*x/3)

# plt.plot (x, y)
# plt.plot (x, tanhClip (x, 1), color='red')
# plt.plot (x, tanhClip (x, 3), color='red')

# plt.plot (x, softClip (x, 1), color='skyblue')



#%%
plt.show()
