#%%
import numpy as np
import scipy.signal as signal
from scipy.io import wavfile
import matplotlib.pyplot as plt
from enum import Enum

#%% [markdown]
# In this article I'd like to examine a nonlinear architecture
# used by old aural exciter effects. The general
# signal flow looks something like this:

#%%
import SchemDraw
from SchemDraw import dsp
import SchemDraw.elements as e

d = SchemDraw.Drawing(fontsize=12)
d.add (e.DOT_OPEN, label='Input', color='white')
L1 = d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.FILT, toplabel='Level Detector', fill='navajowhite')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.AMP, d='right', color='white')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.LINE, d='down', l=1, color='white')
d.add (dsp.ARROWHEAD, d='down', l=1, color='white')

d.add (dsp.LINE, xy=L1.end, d='down', l=1.5, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.OSCBOX, botlabel='Nonlinearity', fill='navajowhite')
d.add (dsp.LINE, d='right', l=1.75, color='white')
d.add (dsp.ARROWHEAD, d='right', color='white')
d.add (dsp.MIX, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (e.DOT_OPEN, label='Output', color='white')
d.draw()

#%% [markdown]
# # Implementation
# ## Level Detector
# 
# For the level detector part of this system, you have a lot of options,
# for instance you could start with a level detection scheme similar to
# one you might use in a compressor or gate effect, with parameterized
# attack and release characteristics [link]. In this implementation, I
# will use a somewhat simpler scheme: a rectifying nonlinearity followed
# by a lowpass filter.
#
# ### Rectifying nonlinearity
# 
# The idea of a rectifying nonlinearity is to take a signal that has both
# positive and negative values, and and transform it into one that contains
# only positive values. Here I'll examine three common rectifying nonlinearities:
# 
# 1. Ideal Full Wave Rectifier: The idea here is that the positive half of
#    the waveform are left unchanged, while the negative half is flipped
#    to be positive. Mathematically this is the same as the absolute value operation.
#
# 2. Ideal Half Wave Rectifier: Similar to the Full Wave Rectifier, the positive
#    part of the signal is unchanged, but for the Half Wave Rectifier, the
#    negative part of the waveform is set to zero.
#
# 3. Schockley Diode: A diode is a circuit element that is often used in rectifying
#    circuits. For our purposes, we can think of it as a less ideal Half Wave Rectifier.
# 
#
# Below, you can see the static curves and sine wave responses for three
# types of rectifying nonlinearities.

#%%
def FWR (x):   
    return np.abs (x)

def HWR (x):
    if isinstance (x, np.ndarray):
        t = np.copy (x)
        for n in range (len (x)):
            t[n] = HWR (t[n])
        return t
    
    return x if x > 0 else 0
    
def Diode (x):  
    return 0.2 * (np.exp (0.05 * x / 0.0259) - 1.0)

def plotRect (rect, range=1):
    x = np.linspace (-range, range, 100)
    y = rect (x)
    plt.plot (x,y)

plt.figure()
plotRect (FWR)
plotRect (HWR)
plotRect (Diode)
plt.title ('Static Curves of Rectifying Nonlinearities')
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.legend (['Full Wave Rectifier', 'Half Wave Rectifier', 'Diode'])

def plotRectSine (rect):
    n = np.arange (44100 / 20)
    x = np.sin (2 * np.pi * n * 100 / 44100)
    y = rect (x)
    plt.plot (y)

plt.figure()
plotRectSine (lambda x : x)
plotRectSine (FWR)
plotRectSine (HWR)
plotRectSine (Diode)
plt.title ('Sine Response of Rectifying Nonlinearities')
plt.xlabel ('Time [Samples]')
plt.ylabel ('Magnitude')
plt.legend (['Input', 'Full Wave Rectifier', 'Half Wave Rectifier', 'Diode'])

#%% [markdown]
# ### Lowpass Filter
#
# The lowpass filter used in a level detector typically has a pretty low
# cutoff frequency. I often use a cutoff frequency somewhere around 10 Hz.
# I won't discuss the technicalities of creating a lowpass filter here, but
# more information can be found in Julius Smith's [Introduction to Digital Filters](https://ccrma.stanford.edu/~jos/filters/).
#
# ### Level Detector Implemented
#
# Now let's take a lok at how the level detector handles a full signal.
# Below we see the response of each level detection scheme for a drum sample.

#%%
class LPF:
    def __init__ (self, fs):
        wc = 2 * np.pi * 10 / fs
        c = 1.0 / np.tan (wc / 2.0)

        self.a = np.zeros (2)
        self.b = np.zeros (2)
        self.a[0] = c + 1.0

        self.b[0] = 1.0 / self.a[0]
        self.b[1] = self.b[0]
        self.a[1] = (1.0 - c) / self.a[0]
        self.z = 0

    def process (self, x):
        y = self.z + x * self.b[0]
        self.z = x * self.b[1] - y * self.a[1]
        return y

class DetectorType (Enum):
    FWR = 1
    HWR = 2
    DIODE = 3

class Detector:
    def __init__ (self, type, fs):
        self.rect = lambda x : x
        if type is DetectorType.FWR:
            self.rect = lambda x : FWR (x)
        elif type is DetectorType.HWR:
            self.rect = lambda x : HWR (x)
        elif type is DetectorType.DIODE:
            self.rect = lambda x : Diode (x)

        self.LPF = LPF (fs)
    
    def process (self, x):
        x = self.rect (x)
        return self.LPF.process (x)


#%%
fs, drums = wavfile.read ('DrumLoop.wav')

drums = drums[:60040,0]
drums = drums / (np.max (np.abs (drums)))
plt.plot (drums, color='firebrick')

types = [DetectorType.FWR, DetectorType.HWR, DetectorType.DIODE]
colors = ['dodgerblue', 'gold', 'lightgreen']
i = 0
for type in types:
    y = np.zeros (len(drums))
    detector = Detector (type, fs)
    for n in range (len (drums)):
        y[n] =  detector.process (drums[n])
    plt.plot (y / (np.max (np.abs (y))), color=colors[i])
    i += 1

plt.legend (['Signal', 'FWR', 'HWR', 'Diode'])
plt.xlabel ('Time [Samples]')
plt.ylabel ('Magnitude')
plt.title ('Detector Response for Drum Sample')

#%% [markdown]
# ## Nonlinearity
#
# For the nonlinearity used in an exciter, it is typical to use a saturating
# nonlinearity: that's a nonlinearity that approaches constant output as the
# input gain grows large. I'll show three options here (hard clipper, soft
# clipper, hyperbolic tangent), but feel free to experiment and try your own
# things!


#%%
def HardClipper (x):
    if isinstance (x, np.ndarray):
        t = np.copy (x)
        for n in range (len (x)):
            t[n] = HardClipper (t[n])
        return t
    
    if x > 1: return 1
    if x < -1: return -1
    return x

def SoftClipper (x):
    if isinstance (x, np.ndarray):
        t = np.copy (x)
        for n in range (len (x)):
            t[n] = SoftClipper (t[n])
        return t
    
    if x > 1: return 1
    if x < -1: return -1
    return 1.5 * (x - x**3/3)

def HTan (x):
    if isinstance (x, np.ndarray):
        t = np.copy (x)
        for n in range (len (x)):
            t[n] = HTan (t[n])
        return t

    return np.tanh (x)

plt.figure()
plotRect (HardClipper, range=2.5)
plotRect (SoftClipper, range=2.5)
plotRect (HTan, range=2.5)
plt.title ('Static Curves of Saturating Nonlinearities')
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.legend (['Hard Clipper', 'Soft Clipper', 'Hyperbolic Tangent'])

#%% [markdown]
# ## Putting It All Together
#
# Now let's go ahead and connect these elements as shown by the diagram above.
# Below I'll show the static curve and harmonic response for a 100 Hz sine
# wave, for an exciter with a diode rectifier, 10 Hz Lowpass Filter, and
# hyperbolic tangent nonlinearity.

#%%
def exciter (x, lpfFreq=1, controlGain=100, fs=44100, rect=Diode):
    T = 1/fs
    c = 2*fs
    w0 = 2 * np.pi * lpfFreq
    a0 = c / w0 + 1
    a1 = (-c / w0 + 1) / a0
    b0 = 1 / a0
    b1 = 1 / a0
    a0 = 1
    
    z_1 = 0
    def filt (x, z_1):
        y = z_1 + x * b0
        z_1 = x * b1 - y * a1
        return y, z_1
    
    y = np.zeros (len (x))
    for n in range (len (x)):
        x_abs = rect (x[n])
        x_filt, z_1 = filt (x_abs, z_1)
        y[n] = (controlGain * x_filt) * np.tanh (x[n] / 0.0259 / 2)
    
    return y

def plotExciter (xGain = 0.2, controlGain=30, seconds=1,freq=100):
    fs = 44100
    N = int (44100*seconds)
    n = np.arange (N)
    x = np.sin (2 * np.pi * n * freq / fs)
    y = exciter (x*xGain, fs=fs, rect=Diode, controlGain=controlGain)
    
    x_test = x[int(3*44100/10):int(4*44100/10)]
    y_test = y[int(3*44100/10):int(4*44100/10)]
    
    return x_test, y_test

x, y = plotExciter()
plt.figure()
plt.plot (x, y*3)
plt.xlabel ('Time')
plt.ylabel ('Magnitude')
plt.title ('Sine Response of Exciter')

#%%
x, y = plotExciter(xGain = 0.1, controlGain=30, seconds=10)
N = len (y)

Y = np.fft.rfft (y)
Y = Y / np.max (np.abs (Y))
f = np.linspace (0, 44100/2, int(N/2+1))

plt.figure()
plt.semilogx (f, 20*np.log10 (np.abs (Y)))
plt.xlim (20, 20000)
plt.ylim (-100, 5)
plt.title ('Harmonic Response')
plt.xlabel ('Frequency [Hz]')
plt.ylabel ('Magnitude [dB]')


#%% [markdown]
# The static curve for the exciting nonlinearity has an interesting width to
# it, since the increasing and decreasing parts of the waveform have slightly
# different characteristics. We can see the effects of this asymettry in the
# harmonic response, in the prescence of the even harmonics in the signal.
# Perceptually, the even harmonics help the exciting nonlinearity to sound
# "smooth" compared to a traditional saturating nonlinearity.
# 
# 
# To demonstrate this nonlinearity in a more realistic scenario, I have
# developed an audio plugin (VST, AU) that implements all of the options
# discussed above, as well as a variable filter frequency for the level
# detector, and drive level for the exciter overall. The source code for
# the plugin is available on [GitHub](https://github.com/jatinchowdhury18/ComplexNonlinearities).
# A video demo can be seen below.

# @TODO: video

#
#
# Finally, I'd like to take a moment to show how this type of nonlinearity is often
# used in the context of an aural exciter. Exciter circuits often refer
# to the nonlinear section we have analyzed here as the "Generator", since
# it generates higher harmonics of the input signal. The general architecture
# can be seen below:

#%%
d = SchemDraw.Drawing(fontsize=12)
d.add (e.DOT_OPEN, label='Input', color='white')
L1 = d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, d='right', color='white', toplabel='Drive')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.FILT_HP, toplabel='Highpass', fill='navajowhite')
d.add (dsp.LINE, d='right', l=1.25, color='white')
d.add (dsp.FILT, toplabel='Generator', fill='lightblue')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, d='right', toplabel='Mix', color='white')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.LINE, d='down', l=1, color='white')
d.add (dsp.ARROWHEAD, d='down', l=1, color='white')

d.add (dsp.LINE, xy=L1.end, d='down', l=1.5, color='white')
d.add (dsp.LINE, d='right', l=8, color='white')
d.add (dsp.ARROWHEAD, d='right', color='white')
d.add (dsp.SUM, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (e.DOT_OPEN, label='Output', color='white')
d.draw()


#%%
