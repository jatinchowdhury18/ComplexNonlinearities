#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import matplotlib.ticker
import FilterShapes as filters

#%% [markdown]
# # Nonlinear Biquad Filter
#
# For today's article, we'll be talking about filters. So far in this series
# I haven't spoken too much about filters, which might seem odd considering
# how much of signal processing in general is all about filters. The reason
# I've avoided filters is that most filters in audio signal processing are
# implemented as linear processors, and I've been focusing on nonlinear
# processing concepts.
# 
# However, this does not have to be the case. For instance, Vadim Zavalishin
# at Native Instruments has written extensively about including nonlinear elements in
# ladder filters and state variable filters, in ways that accurately reflect
# how these filters often behave in the analog world. Actually, Vadim's
# entire book, [The Art of Virtual Analog Filter Design](https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.0.pdf),
# is a fantastic resource, that I highly recommend for anyone with an interest
# in filter design for audio. That said, Vadim focuses mostly on the
# virtual analog world, and in this series of articles I've been striving
# to construct signal processing ideas that can be conceived and implemented
# entirely in the digital realm.
#
# That said, one of the coolest things about analog filters is the way that
# they resonate. A "resonance" occurs in a filter when the filter is tuned to amplify
# a certain frequency, much like a guitar string is designed to resonate at
# the frequency to which it is tuned. In the analog world, whenever something
# is amplified, we run the risk of moving into nonlinear territory, where
# saturation and distortion can occur. One of the reasons that analog filters
# often sound "warm" to musicians and engineers is due to the fact that as
# the filters resonate, they begin to saturate as well. Thinking about this
# got me thinking: could there be a way to alter some existing digital filter
# forms so that any resonance in the filter automatically undergoes some
# saturation? This idea is what I'll be developing below...

#%% [markdown]
# ## Biquad Filters
#
# To begin, I'd like to introduce the idea of a "biquad" filter, which is
# a short way for DSP engineers to refer to a 2nd-order filter. "2nd-order"
# means that the filter has "memory" that extends back to the two previous
# timesteps that it has computed. In other words, the current output
# of the filter depends not just on the current input, but also on the
# previous two inputs and outputs. DSP engineers like biquad filters a lot,
# because any filter can be broken down into a "cascade" of biquad filters.
#
# ### Transposed Direct Form-II
#
# There are several way to implement a biquad filter, but one of my favorites
# is the so-called "Transposed Direct Form-II". This a rather complicated
# name for a relatively simple thing, but more information on why
# this name exists, as well as other filter forms, can be found
# in Julius Smith's wonderful [Introduction to Digital Filters](https://ccrma.stanford.edu/~jos/filters/Implementation_Structures_Recursive_Digital.html).

#%%
import SchemDraw
from SchemDraw import dsp
import SchemDraw.elements as e

d = SchemDraw.Drawing(fontsize=12)
d.add (e.DOT_OPEN, label='Input', color='white')
L1 = d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, toplabel='$b_0$', color='white')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='right', color='white')
SUM0 = d.add (dsp.SUM, color='white')

d.add (dsp.LINE, xy=L1.end, d='down', l=3, color='white')
L2 = d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, toplabel='$b_1$', color='white')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='right', color='white')
SUM1 = d.add (dsp.SUM, color='white')
d.add (dsp.LINE, d='up', xy=SUM1.N, l=0.375, color='white')
d.add (dsp.BOX, label='$z^{-1}$', color='white')
d.add (dsp.LINE, d='up', l=0.625, color='white')
d.add (dsp.ARROWHEAD, d='up', color='white')

d.add (dsp.LINE, xy=L2.start, d='down', l=3, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, toplabel='$b_2$', color='white')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='right', color='white')
SUM2 = d.add (dsp.SUM, color='white')
d.add (dsp.LINE, d='up', xy=SUM2.N, l=0.375, color='white')
d.add (dsp.BOX, label='$z^{-1}$', color='white')
d.add (dsp.LINE, d='up', l=0.625, color='white')
d.add (dsp.ARROWHEAD, d='up', color='white')

Y = d.add (dsp.LINE, d='right', xy=SUM0.E, l=2.5, color='white')
d.add (dsp.LINE, d='down', l=3, color='white')
L3 = d.add (dsp.LINE, d='left', l=1, color='white')
A1 = d.add (dsp.AMP, toplabel='$-a_1$', color='white')
d.add (dsp.LINE, d='left', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='left', color='white')

d.add (dsp.LINE, xy=L3.start, d='down', l=3, color='white')
d.add (dsp.LINE, d='left', l=1, color='white')
A1 = d.add (dsp.AMP, toplabel='$-a_2$', color='white')
d.add (dsp.LINE, d='left', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='left', color='white')

d.add (dsp.LINE, xy=Y.end, d='right', l=1, color='white')
d.add (e.DOT_OPEN, label='Output', color='white')

d.draw()

#%% [markdown]
# Above we see a signal flow diagram for Transposed Direct Form-II (TDF-II).
# TDF-II is a favorite of mine for several reasons. For one thing, it is pretty
# efficient in that it only uses a handful of multiplies and two delay
# elements. It also has some nice [numerical properties](https://ccrma.stanford.edu/~jos/filters/Numerical_Robustness_TDF_II.html)
# for implementing highly resonant filters. Another convenient thing about TDF-II
# which will be useful here, is that we can easily see what values are stored
# in the unit delay ($z^{-1}$) elements. We'll refer to these values as the
# "state" of the filter.
#
# ### Filter Shapes
#
# There are several particular filter "shapes" that are often useful for
# processing audio, and all of them can be constructed using a TDF-II
# structure. In this article, I'm going to focus on 6 shapes: Highpass, Lowpass,
# High Shelf, Low Shelf, Bell, and Notch. Plots of the frequency response for
# each of these filters can be found below. For instructions on how to implement
# these filters using a biquad filter, please check out Robert Bristow Johnson's
# [Audio EQ Cookbook](https://www.w3.org/2011/audio/audio-eq-cookbook.html),
# another indespensible resource for audio DSP engineers.

#%%
def plotFilter (b, a, title, ylim, fs=44100):
    w, H = signal.freqz (b, a, fs=fs)
    plt.figure()
    plt.semilogx (w, 20 * np.log10 (np.abs (H)))
    plt.title (title)
    plt.ylim (ylim)
    plt.gca().xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
    plt.xlabel ('Frequency [Hz]')
    plt.ylabel ('Magnitude [dB]')
    plt.xlim (20, 20000)

fs = 44100
b, a = filters.calcCoefsLPF2 (1000, 0.707*2, fs)
plotFilter (b, a, 'Lowpass Filter', ylim=(-30, 10))

b, a = filters.calcCoefsHPF2 (1000, 0.707*2, fs)
plotFilter (b, a, 'Highpass Filter', ylim=(-30, 10))

b, a = filters.calcCoefsLowShelf (1000, 0.707*2, 2, fs)
plotFilter (b, a, 'Low Shelf Filter', ylim=(-5, 10))

b, a = filters.calcCoefsHighShelf (1000, 0.707*2, 2, fs)
plotFilter (b, a, 'High Shelf Filter', ylim=(-5, 10))

b, a = filters.calcCoefsBell (1000, 0.707*2, 2, fs)
plotFilter (b, a, 'Bell Filter', ylim=(-5, 10))

b, a = filters.calcCoefsNotch (1000, 0.707*2, fs)
plotFilter (b, a, 'Notch Filter', ylim=(-30, 10))

#%% [markdown]
# ## Nonlinear Resonance
#
# In order to make any TDF-II filter exhibit nonlinear resonance, we can
# simply saturate the states. In the following examples, I will demonstrate
# three saturating nonlinearities: a soft clipper, a hard clipper, and a $\tanh$
# clipper. The mathematics that describe why this works, and
# ensure that we don't accidentally make an unstable filter are a litle bit
# complex, and I'm not going to explain them here, partly because I'm still
# wrapping my head around them myself. The resulting signal flow diagram
# will look something like this (the "NL" blocks represent nonlinear elements):

#%%
d = SchemDraw.Drawing(fontsize=12)
d.add (e.DOT_OPEN, label='Input', color='white')
L1 = d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, toplabel='$b_0$', color='white')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='right', color='white')
SUM0 = d.add (dsp.SUM, color='white')

d.add (dsp.LINE, xy=L1.end, d='down', l=4.25, color='white')
L2 = d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, toplabel='$b_1$', color='white')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='right', color='white')
SUM1 = d.add (dsp.SUM, color='white')
d.add (dsp.LINE, d='up', xy=SUM1.N, l=0.25, color='white')
d.add (dsp.BOX, label='NL', color='white')
d.add (dsp.LINE, d='up', l=0.375, color='white')
d.add (dsp.BOX, label='$z^{-1}$', color='white')
d.add (dsp.LINE, d='up', l=0.625, color='white')
d.add (dsp.ARROWHEAD, d='up', color='white')

d.add (dsp.LINE, xy=L2.start, d='down', l=4.25, color='white')
d.add (dsp.LINE, d='right', l=1, color='white')
d.add (dsp.AMP, toplabel='$b_2$', color='white')
d.add (dsp.LINE, d='right', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='right', color='white')
SUM2 = d.add (dsp.SUM, color='white')
d.add (dsp.LINE, d='up', xy=SUM2.N, l=0.25, color='white')
d.add (dsp.BOX, label='NL', color='white')
d.add (dsp.LINE, d='up', l=0.375, color='white')
d.add (dsp.BOX, label='$z^{-1}$', color='white')
d.add (dsp.LINE, d='up', l=0.625, color='white')
d.add (dsp.ARROWHEAD, d='up', color='white')

Y = d.add (dsp.LINE, d='right', xy=SUM0.E, l=2.5, color='white')
d.add (dsp.LINE, d='down', l=4.25, color='white')
L3 = d.add (dsp.LINE, d='left', l=1, color='white')
A1 = d.add (dsp.AMP, toplabel='$-a_1$', color='white')
d.add (dsp.LINE, d='left', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='left', color='white')

d.add (dsp.LINE, xy=L3.start, d='down', l=4.25, color='white')
d.add (dsp.LINE, d='left', l=1, color='white')
A1 = d.add (dsp.AMP, toplabel='$-a_2$', color='white')
d.add (dsp.LINE, d='left', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='left', color='white')

d.add (dsp.LINE, xy=Y.end, d='right', l=1, color='white')
d.add (e.DOT_OPEN, label='Output', color='white')

d.draw()

#%% [markdown]
# ### Frequency Responses
#
# To get a sense of what this modified structure does to our filter, it would
# be nice to look at the frequency response of the filter. Unfortunately, as
# I mentioned in my [introductory article]() for this series, a nonlinear system
# does not have a frequency response the same way that a linear system does.
# However, what we can do is choose some level (often called an "operating point"),
# and see how our system reacts to various frequencies at that level. Below, we show
# the "frequency response" for a low shelf filter with $\tanh$ nonlinearities
# at various operating points.

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
fs = 44100
b, a = filters.calcCoefsLowShelf (1000, 0.707*2, 2, fs)

normalBQ = Biquad()
normalBQ.setCoefs (b, a)

nlBQ = Biquad()
nlBQ.setCoefs (b, a)
nlBQ.saturator = lambda x : np.tanh (x)

nlBQ2 = Biquad()
nlBQ2.setCoefs (b, a)
nlBQ2.saturator = lambda x : np.tanh (x)

nlBQ3 = Biquad()
nlBQ3.setCoefs (b, a)
nlBQ3.saturator = lambda x : np.tanh (x)

def plotFilterResponse (biquad, fs, gain=0.1):
    x = gain*chirpLog (20, 20000, 1, fs)
    y = biquad.processBlock (np.copy (x))
    N = len (x)

    h = chrp2ir (x, y)

    f = np.linspace (0, fs/2, num=N/2+1)
    H = np.fft.rfft (h)
    plt.semilogx (f, 20 * np.log10 (np.abs (H)))

plt.figure()
plotFilterResponse (nlBQ3, fs, gain=0.5)
plotFilterResponse (nlBQ2, fs, gain=0.25)
plotFilterResponse (nlBQ, fs)
plotFilterResponse (normalBQ, fs)
plt.xlim (20, 20000)
plt.gca().xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())

plt.title ('Nonlinear Low Shelf')
plt.xlabel ('Frequency [Hz]')
plt.ylabel ('Magnitude [dB]')
plt.legend (['Nonlinear (gain  = 0.5)', 'Nonlinear (gain  = 0.25)', 'Nonlinear (gain  = 0.1)', 'Linear'])

#%% [markdown]
# From the above plot, it may seem that all we're doing with our nonlinear bell filter
# is creating a bell filter for which the resonance decreases as the input level
# increases, almost like a voltage controlled filter (VCF).
# To an extent this is true, but remember that the active ingredients
# for this process of decreasing resonance are the saturating nonlinearities,
# which add harmonics, thus making our filter sound harmonically richer as well.

#%% [markdown]
# ## Anti-Aliasing
#
# Now if we can measure the harmonic response of this nonlinear
# filter, that will be useful for us in avoiding aliasing
# artifacts. Below, we show the harmonic response of a 100 Hz
# sine wave, for a nonlinear bell filter with a center frequency at the
# same frequency, and a gain of 12 dB. We show this response
# for three types of saturating nonlinearities: hard clipper,
# soft clipper, and $\tanh$ clipper.


#%%
def plotHarmonicResponse (biquad, fs, gain=0.1, freq=100):
    N = fs/2
    n = np.arange (N)
    x = gain*np.sin (2 * np.pi * n * freq / fs)
    y = biquad.processBlock (np.copy (x))

    f = np.linspace (0, fs/2, num=N/2+1)
    H = np.fft.rfft (y)
    plt.semilogx (f, 20 * np.log10 (np.abs (H)) - 90)
    plt.gca().xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
    plt.xlabel ('Frequency [Hz]')
    plt.ylabel ('Magnitude [dB]')
    plt.xlim (20, 20000)

fs = 44100
b, a = filters.calcCoefsBell (100, 0.707, 2, fs)

def hardClip (x):
    if x > 1: return 1
    if x < -1: return -1
    return x

plt.figure()
hardBQ = Biquad()
hardBQ.setCoefs (b, a)
hardBQ.saturator = lambda x : hardClip (x)
plotHarmonicResponse (hardBQ, fs, gain = 1.5)
plt.title ('Hard Clip Bell Filter Harmonic Response')

def softClip (x):
    if x > 1: return 2 / 3
    if x < -1: return -2 / 3
    return x - x**3/3

plt.figure()
softBQ = Biquad()
softBQ.setCoefs (b, a)
softBQ.saturator = lambda x : softClip (x)
plotHarmonicResponse (softBQ, fs, gain = 1.5)
plt.title ('Soft Clip Bell Filter Harmonic Response')

plt.figure()
tanhBQ = Biquad()
tanhBQ.setCoefs (b, a)
tanhBQ.saturator = lambda x : np.tanh (x)
plotHarmonicResponse (tanhBQ, fs, gain = 1.5)
plt.title ('Tanh Clip Bell Filter Harmonic Response')

#%% [markdown]
# As you can see the soft clipping and $\tanh$ response generate harmonics
# up past 10x the fundamental, while the hard clipper generates significantly
# more. For my purposes, I've found that upsampling by a factor of 8 mitigates
# aliasing artifacts down below -80 dB for the soft clipper and $\tanh$ clipper.

#%%
