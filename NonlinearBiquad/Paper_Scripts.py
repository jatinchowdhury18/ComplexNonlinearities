#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import matplotlib.ticker
import audio_dspy as adsp
import FilterShapes as filters


#%%
def lin(x, x_0):
    slope = 1.0 / np.cosh (x_0)**2
    correct_y = np.tanh (x_0)
    off = correct_y - slope*x_0
    return slope * x + off

x = np.linspace (-5, 5)
x0 = 2
plt.plot (x, np.tanh (x))
plt.plot (x, lin (x, 1))
plt.plot (1, np.tanh (1), '-ro')
plt.ylim (-1.5, 1.5)
plt.title (r'$\tanh$ Nonlinearity, Linearized at $x=1$')
plt.legend ([r'$f_{NL}(x)$', r'$\bar{f}_{NL}(x), x_0 = 1$'])

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
def plotFilterResponse (biquad, fs, gain=0.1):
    x = gain*chirpLog (20, 20000, 1, fs)
    y = biquad.processBlock (np.copy (x))
    N = len (x)

    h = chrp2ir (x, y)

    f = np.linspace (0, fs/2, num=N/2+1)
    H = np.fft.rfft (h)
    plt.semilogx (f, 20 * np.log10 (np.abs (H)))

def plotNonlinearFilterResponse (b, a, title, gains=[0.5, 0.25, 0.1, 0.05, 0.025]):
    plt.figure()
    legend = []
    for gain in gains:
        nlBQ = Biquad()
        nlBQ.setCoefs (b, a)
        nlBQ.saturator = lambda x : np.tanh (x)
        plotFilterResponse (nlBQ, fs, gain=gain)
        legend.append ('Nonlinear (gain={})'.format (gain))

    normalBQ = Biquad()
    normalBQ.setCoefs (b, a)
    plotFilterResponse (normalBQ, fs)
    legend.append ('Linear')

    plt.xlim (20, 20000)
    plt.gca().xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())

    plt.title (title)
    plt.xlabel ('Frequency [Hz]')
    plt.ylabel ('Magnitude [dB]')
    plt.legend (legend)

fs = 44100
b, a = filters.calcCoefsLPF2 (1000, 10, fs)
plotNonlinearFilterResponse (b, a, 'Nonlinear Resonant Lowpass')
plt.ylim (-15)

#%%
import audio_dspy as adsp

r = 2
adsp.plot_static_curve (lambda x : np.tanh (x), range=r)
adsp.plot_static_curve (lambda x : 1.5*adsp.soft_clipper (x), range=r)
adsp.plot_static_curve (lambda x : adsp.hard_clipper (x), range=r)

plt.title ('Comparing Saturating Nonlinearities')
plt.legend (['Tanh Clipper', 'Soft Clipper', 'Hard Clipper'])

#%%
from  matplotlib import patches

def zplane (p, z):
    ax = plt.subplot(111)
    uc = patches.Circle((0,0), radius=1, fill=False,
                        color='white', ls='dashed')
    ax.add_patch(uc)

    ax.spines['left'].set_position('center')
    ax.spines['bottom'].set_position('center')
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    
    t1 = plt.plot(z.real, z.imag, 'go', ms=10)
    t2 = plt.plot(p.real, p.imag, 'rx', ms=10)

    r = 1.5; plt.axis('scaled'); plt.axis([-r, r, -r, r])
    ticks = [-1, -.5, .5, 1]; plt.xticks(ticks); plt.yticks(ticks)

def getPoles (b, a, g=1):
    return np.roots ([1,  g * a[1], g**2 * a[2]])

def getZeros (b, a, g=1):
    return np.roots ([b[0], g * b[1], g**2 * b[2]])

fs = 44100
b, a = filters.calcCoefsLPF2 (1000, 10, fs)

for g in [1, 0.8, 0.6, 0.4, 0.2]:
    plt.figure()
    zplane (getPoles (b, a, g=g), getZeros (b, a, g=g))
    plt.title ('Poles/Zeros for g={}'.format (g))

#%%
def plotSquareResponse (biquad, freq, fs=44100, gain=1, plotInput=False, mult=1, color=''):
    numCycles = 10
    x = np.zeros (int ((1/freq)*fs* numCycles))
    N = len (x)
    for n in range (N):
        n2 = numCycles*2
        for k in range (0, n2+1, 2):
            if (n > k*N/n2 and n < (k+1)*N/n2):
                x[n] = 1
                break
            if (k >= n2):
                x[n] = -1

    x *= gain
    y = np.copy (x)
    y = biquad.processBlock (y)

    x = x[int((numCycles-2)*N/numCycles):]
    y = y[int((numCycles-2)*N/numCycles):]

    t = np.arange (len(y))/fs * 1000
    if (plotInput): plt.plot (t, x/gain)
    if color == '': plt.plot (t, y/gain*mult)
    else: plt.plot (t, y/gain*mult, color=color)
    plt.xlabel ('Time [ms]')
    return t, x/gain, y/gain

legend = []
x = np.zeros(100); t = np.zeros(100)
for g in [0.05, 0.2]:
    nlBQ = Biquad()
    b, a = filters.calcCoefsLPF2 (250, 10, fs=44100)
    nlBQ.setCoefs (b, a)
    nlBQ.saturator = lambda x : np.tanh (x)
    t, x, y = plotSquareResponse (nlBQ, 250, gain=g)
    legend.append ('gain={}'.format (g))

plt.plot (t, x)
plt.legend (legend)
plt.title ('Filter Response with Varying Input Gain')


#%%
def plotFile (file):
    data = open (file, 'r').readlines()
    N = len (data)

    t = np.zeros (N)
    y = np.zeros (N)

    time0 = -1
    for n in range (1, N):
        time = float (data[n].split ('\t')[0])
        # if (time < (1 / 250)):
            # continue

        if (time0 < 0):
            time0 = time

        t[n] = time - time0
        y[n] = data[n].split ('\t')[1]

    plt.plot (t*1000, y)
    return t, y

time, y = plotFile ('SPICE/Sallen-Key_LPF-250.txt')

nlBQ = Biquad()
b, a = filters.calcCoefsLPF2 (1000, 10, fs=44100)
nlBQ.setCoefs (b, a)
nlBQ.saturator = lambda x : np.tanh (x)
t, x, ytest = plotSquareResponse (nlBQ, 250, gain=0.2)
# plt.xlim (0, 2)
#%%
y_re = np.zeros (len (x))
for n in range (len (x)):
    t = n * (1 / 44100)
    for k in range (len (time)):
        if time[k] > t:
            y_re[n] = y[k-1]
            break

print (len (y_re))
print (len (ytest))


#%%
def calc_error (x, y):
    x /= np.max (np.abs (x))
    y /= np.max (np.abs (y))
    sum = 0
    for n in range (len (x)):
        diff = x[n] - y[n]
        sum += np.sqrt (abs (diff))
    return sum / len (x)

def calc_min_gain (x, tolerance = 0.0001, max_iters=100):
    gain = 1
    direction = 1
    last = 1000000000
    step = 0.05
    for _ in range (max_iters):
        # adjust value
        if (direction == 1): # up
            gain += step
        else: # down
            gain -= step

        # calc error
        nlBQ = Biquad()
        b, a = filters.calcCoefsLPF2 (1000, 10, fs=44100)
        nlBQ.setCoefs (b, a)
        nlBQ.saturator = lambda x : np.tanh (x)
        _, _, ytest = plotSquareResponse (nlBQ, 250, gain=gain)
        error = calc_error (x, ytest)

        print ('{}, {}'.format (error, gain))

        # adjust
        if (error > last): # went too far
            direction *= -1
            step /= 1.2
        if (abs (error) < tolerance):
            break
        if (gain < 0):
            direction = 1
        last = error
    print (error)
    return gain

# print (calc_error (y_re, ytest))
optimal_gain = calc_min_gain (y_re)
print (optimal_gain)
# optimal gain = 0.226

#%%
t, x, ytest = plotSquareResponse (nlBQ, 250, gain=0.00001, color='sienna')
time, y = plotFile ('SPICE/Sallen-Key_LPF-250.txt')

nlBQ = Biquad()
b, a = filters.calcCoefsLPF2 (1000, 10, fs=44100)
nlBQ.setCoefs (b, a)
nlBQ.saturator = lambda x : np.tanh (x)
t, x, ytest = plotSquareResponse (nlBQ, 250, gain=0.2825, mult=1.25)
plt.legend (['Linear', 'LTSpice', 'NL Biquad'])
plt.title ('Modelling an Analog Sallen-Key Filter')

#%%
import SchemDraw
from SchemDraw import dsp
import SchemDraw.elements as e

d = SchemDraw.Drawing(fontsize=12)
d.add (e.DOT_OPEN, label='x[n]', color='white')
L1 = d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.BOX, label='$f(x)$', color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
y = d.add (e.DOT_OPEN, label='y[n]', color='white')

d.draw()

# %%
d = SchemDraw.Drawing(fontsize=12)
d.add (e.DOT_OPEN, label='x[n]', color='white')
L1 = d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, label='$f\'(x_0)$', color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
sum = d.add (dsp.SUM, color='white')
d.add (dsp.LINE, d='up', l=0.5, color='white', xy=sum.N)
d.add (e.DOT_OPEN, label='$c(x_0)$', color='white')
d.add (dsp.LINE, d='right', l=1, color='white', xy=sum.E)
y = d.add (e.DOT_OPEN, label='y[n]', color='white')

d.draw()

# %%
