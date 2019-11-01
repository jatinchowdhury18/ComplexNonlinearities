#%%
import numpy as np
import matplotlib.pyplot as plt
import audio_dspy as adsp

#%%[markdown]
# # Wavefolding
#
# For today's article I'd like to take a look at
# wavefolding. For those who are unfamiliar,
# wavefolding is an interesting type of
# distortion that can be found in several
# notable analog synthesizer circuits.
# The general idea behind wavefolding
# distortion is that as the signal grows
# larger it eventually reflects over itself.
# There seems to be a lot of mystery about
# wavefolding in what I've found on the
# internet, so I'm hoping to clarify some
# things here by breaking down some typical digital
# wavefolding techniques, and then introducing
# a couple of my own modifications on the
# standard techniques.
#
# Note that I won't really be touching on analog
# wavefolding circuits in this post, for more information
# on that subject, I recommend this [blog post
# by Keith McMillen](https://www.keithmcmillen.com/blog/simple-synthesis-part-8-wavefolding/)
# as well as this [DAFx paper](http://www.dafx17.eca.ed.ac.uk/papers/DAFx17_paper_82.pdf)
# from 2017 which provides an insightful discussion
# of the Buchla 259 wavefolder circuit.

#%%[markdown]
# ## Standard Digital Wavefolding
#
# The typical method for implementing a
# digital wavefolding effect is to modulate
# the input signal by another wave, often
# a triangle or sine wave. This means that
# the static curves we're used to looking at
# for nonlinear effects will be simple
# triangle or sine waves.

# %%
def tri_wave(x, freq, fs):
    p = float((1/freq) * fs)
    x = x + p/4
    return 4 * np.abs((x / p) - np.floor((x / p) + 0.5)) - 1

def sine_wave(x, freq, fs):
    return np.sin(2 * np.pi * x * freq / fs)

# %%
fs = 44100

plt.figure()
adsp.plot_static_curve(lambda x : tri_wave(x, fs/2, fs), gain = 5)
plt.title('Triangle Wavefolder Static Curve')

plt.figure()
adsp.plot_static_curve(lambda x : sine_wave(x, fs/2, fs), gain = 5)
plt.title('Sine Wavefolder Static Curve')

# %% [markdown]
# When we apply this simple wavefolding technique to an input sine
# wave, we get back the expected "folding" behavior.

# %%
N = fs / 50
n = np.arange(N)
freq = 100
x = np.sin(2 * np.pi * n * freq / fs)
y = sine_wave(x,  fs/2.5, fs)
y2 = tri_wave(x, fs/2.5, fs)

plt.plot(x)
plt.plot(y)
plt.plot(y2)
plt.legend(['Dry', 'Sine Folder', 'Tri Folder'])
plt.title('Folded Sine Wave')

# %% [markdown]
# ## Modified Wavefolding 1: Saturation
#
# The first modification we'll make to the traditional digital
# wavefolder is to sum the outputs of a saturating nonlinearity
# and a wavefolding nonlinearity. Although I haven't compared the
# two on a mathematical level, I've found through my own listening
# tests that this architecture sounds a little bit more similar to
# a typical analog wavefolder, especially for large amounts of folding. 

# %%
import SchemDraw
from SchemDraw import dsp
import SchemDraw.elements as e

d = SchemDraw.Drawing(fontsize=12)
d.add(e.DOT_OPEN, label='Input', color='white')
L1 = d.add(dsp.LINE, d='right', l=1, color='white')
d.add(dsp.LINE, d='up', l=1, color='white')
d.add(dsp.LINE, d='right', l=1.25, color='white')
d.add(dsp.BOX, label='Sat', color='white')
d.add(dsp.LINE, d='right', l=1.25, color='white')
d.add(dsp.LINE, d='down', l=0.5, color='white')
d.add(dsp.ARROWHEAD, d='down', color='white')

d.add(dsp.LINE, d='down', xy=L1.end, l=1, color='white')
d.add(dsp.LINE, d='right', l=0.75, color='white')
d.add(dsp.BOX, label='WF', color='white')
d.add(dsp.LINE, d='right', l=0.5, color='white')
d.add(dsp.AMP, color='white', label='G')
d.add(dsp.LINE, d='right', l=0.5, color='white')
d.add(dsp.LINE, d='up', l=0.5, color='white')
d.add(dsp.ARROWHEAD, d='up', color='white')

SUM0=d.add(dsp.SUM, color='white')
d.add(dsp.LINE, d='right', xy=SUM0.S, l=1, color='white')
d.add(e.DOT_OPEN, label='Output', color='white')
d.draw()

# %% [markdown]
# The diagram above shows the signal flow for the saturating wavefolder,
# where "WF" denotes the wavefolding nonlinearity. For the gain "G",
# I typically use somwhere between -0.5 and -0.1. Below we show the
# static curve and sine wave response for a saturating wavefolder
# with a hyperbolic tangent saturator, a sine wavefolder, and G=-0.2.

# %%
def sine_tanh(x, G, freq, fs):
    return np.tanh(x) + G * sine_wave(x, freq, fs)

plt.figure()
adsp.plot_static_curve(lambda x : sine_tanh(x, -0.2, fs/2, fs), gain=5)
plt.title('Saturating Wavefolder Static Curve')

y3 = adsp.normalize(sine_tanh(3*x, -0.2, fs/2.5, fs))

plt.figure()
plt.plot(x)
plt.plot(y)
plt.plot(y3)
plt.legend(['Dry', 'Wavefolder', 'Saturating Wavefolder'])
plt.title('Wavfolder vs. Saturating Wavefolder')

# %% [markdown]
# While the modified structure may seem only marginally different from the
# original wavefolder, watch how the response changes for a large input:

# %%
N = fs / 50
n = np.arange(N)
x = 2.5 * np.sin(2 * np.pi * n * freq / fs)
y = sine_wave(x, fs/2.5, fs)
y3 = adsp.normalize(sine_tanh(3*x, -0.2, fs/2.5, fs))

plt.figure()
plt.plot(adsp.normalize(x))
plt.plot(y)
plt.plot(y3)
plt.legend(['Dry', 'Wavefolder', 'Saturating Wavefolder'])
plt.title('Wavfolder vs. Saturating Wavefolder For Large Input')

# %% [markdown]
# ## Modified Wavefolder 2: Feedback
#
# Next, let's modify our saturating wavefolder by adding a nonlinear feedback
# path.

# %%
d = SchemDraw.Drawing(fontsize=12)
d.add(e.DOT_OPEN, label='Input', color='white')
L1 = d.add(dsp.LINE, d='right', l=1, color='white')
d.add(dsp.LINE, d='up', l=1, color='white')
d.add(dsp.LINE, d='right', l=1.25, color='white')
d.add(dsp.BOX, label='Sat', color='white')
d.add(dsp.LINE, d='right', l=1, color='white')
d.add(dsp.LINE, d='down', l=0.5, color='white')
d.add(dsp.ARROWHEAD, d='down', color='white')

d.add(dsp.LINE, d='down', xy=L1.end, l=1, color='white')
d.add(dsp.LINE, d='right', l=0.75, color='white')
d.add(dsp.BOX, label='WF', color='white')
d.add(dsp.LINE, d='right', l=0.5, color='white')
d.add(dsp.AMP, color='white', label='G')
d.add(dsp.LINE, d='right', l=0.5, color='white')
d.add(dsp.LINE, d='up', l=0.5, color='white')
d.add(dsp.ARROWHEAD, d='up', color='white')

SUM0=d.add(dsp.SUM, color='white')
d.add(dsp.LINE, d='right', xy=SUM0.S, l=1.25, color='white')
L2 = d.add(dsp.LINE, d='right', l=1, color='white')
d.add(e.DOT_OPEN, label='Output', color='white')

d.add(dsp.LINE, d='up', xy=L2.start, l=1, color='white')
d.add(dsp.LINE, d='left', l=0.25, color='white')
d.add(dsp.BOX, label='Sat', color='white')
d.add(dsp.LINE, d='left', l=0.25, color='white')
d.add(dsp.LINE, d='down', l=0.5, color='white')
d.add(dsp.ARROWHEAD, d='down', color='white')
d.draw()

# %% [markdown]
# This modification will add an interesting texture to our nonlinearity.
# In particular, it will add a little bit of movement, almost like a
# resonance, that reflects the behavior of the wavefolder. It's a little
# bit difficult to describe, but an audio example will be provided later
# on.
#
# Below we show the dynamic response of the saturating feedback wavefolder
# in which both the feedforward and feedback nonlinearities are $\tanh$
# functions, the wavefolder is a sine wave function, and G = -0.5.

# %%
class WaveFolder:
    def __init__(self):
        self.y1 = 0
        self.fb = lambda _ : 0
        self.ff = lambda x : x
        self.wave = lambda x : x

    def process(self, x):
        z = self.ff(x) + self.fb(self.y1)
        y = z + self.wave(x)

        self.y1 = y
        return y

    def process_block(self, block):
        out = np.copy(block)

        for n, _ in enumerate(block):
            out[n] = self.process(block[n])

        return out

# %%
WF = WaveFolder()
WF.ff = lambda x : np.tanh(x)
WF.fb = lambda x : 0.9*np.tanh(x)
WF.wave = lambda x : -0.5*sine_wave(x, fs/2, fs)

adsp.plot_dynamic_curve(lambda x : WF.process_block(x))
plt.title('Saturating Feedback Wavefolder Dynamic Response')

# %% [markdown]
# ## Antialiasing
#
# Wavefolders are notorious for having a pretty gnarly harmonic response,
# which can result in some serious aliasing distortion if not handled
# properly. First, let's look at the harmonic response for the traditional
# digital wavefolder, for both sine and triangle waves.

# %%
plt.figure()
adsp.plot_harmonic_response(lambda x : tri_wave(x, fs/2, fs), gain=2)
plt.title('Triangle Wavefolder Harmonic Response')

plt.figure()
adsp.plot_harmonic_response(lambda x : sine_wave(x, fs/2, fs), gain=2)
plt.title('Sine Wavefolder Harmonic Response')

# %% [markdown]
# So first off, note the shape of the harmonic structures for both
# wavefolders: in both cases the 5th harmonic is the most prominent, even
# more so than the fundamental. We also see that the triangle shape has an
# almost unbounded harmonic response, which looks like it might lead to a
# ton of aliasing. While I personally don't like the triangle wavefolder
# very much, if you would like to use it I recommend using a method called
# BLAMP (I'm serious that is actually what it's called) to "round" the
# corners of the triangle function.
# You can read more [here](http://research.spa.aalto.fi/publications/papers/dafx16-blamp/).
#
# Next let's take a look at the harmonic response for the saturating
# wavefolder, again with sine wavefolding, $\tanh$ saturation, and G=-0.2.

# %%
plt.figure()
adsp.plot_harmonic_response(lambda x : sine_tanh(x, -0.2, fs/2, fs), gain=2)
plt.title('Saturating Wavefolder Hamornic Response')

# %% [markdown]
# Not too much of interest going on here, the harmonic response
# is almost indistinguishable from a standard $\tanh$ nonlinearity.
# Finally let's examine the feedback saturating wavefolder, again with
# feedforward and feedback nonlinearities as $\tanh$
# functions, the wavefolder as a sine wave function, and G = -0.5.

# %%
WF = WaveFolder()
WF.ff = lambda x : np.tanh(x)
WF.fb = lambda x : 0.9*np.tanh(x)
WF.wave = lambda x : -0.5*tri_wave(x, fs/2, fs)

adsp.plot_harmonic_response(lambda x : WF.process_block(x), gain=2)
plt.title('Feedback Wavefolder Harmonic Response')

# %% [markdown]
# Here we see a much more rich harmonic response, with smoothly decaying
# harmonics going most of the way up the spectrum. Aliasing could be an
# issue here, but fortunately our feedback path introduces a little bit
# of an integrating effect that cancels out some of the highest frequency
# harmonics. I've found that 4x oversampling mitigates most aliasing
# artifacts from this system, but I typically use 8x just to be safe.

# %% [markdown]
# ## Examples
#
# As usual, I've implemented the above wavefolding processors, along
# with variable feedforward and feedback gains, as an audio plugin
# (VST, AU). Feel free to checkout the source code on
# [GitHub](https://github.com/jatinchowdhury18/ComplexNonlinearities),
# and checkout a video demo on [YouTube](https://youtu.be/XljV1Cw1COI).

# %%
