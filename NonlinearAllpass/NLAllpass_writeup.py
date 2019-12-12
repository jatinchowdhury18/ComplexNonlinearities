#%%
import numpy as np
import audio_dspy as adsp
import scipy.signal as signal
import matplotlib.pyplot as plt
plt.style.use('dark_background')

import SchemDraw
from SchemDraw import dsp
import SchemDraw.elements as e

#%% [markdown]
# # Nonlinear Allpass Filters
#
# Today we'll be examining an interesting nonlinear effect that can be used
# in sound synthesis to create some interesting textures. I discovered this
# technique through a [DAFx paper from 2011](https://ccrma.stanford.edu/~rmichon/publications/doc/DAFx11-Nonl-Allpass.pdf)
# written by two mentors of mine, Romain Michon and Julius Smith. While I
# won't be going much beyond their work in this article, I plan to use this
# processor as a building block for future nonlinear effects.
#
# ## Basic Structure: Ladder Filter
#
# The basic structure that we'll be starting from is that of a ladder filter.
# A first-order "normalized" ladder filter looks like this:

#%%
d = SchemDraw.Drawing(fontsize=12, color='white')

height = 4

d.add(e.DOT_OPEN, label='Input')
L1 = d.add(dsp.LINE, d='right', l=2)
d.add(dsp.ARROWHEAD, d='right')
d.add(dsp.LINE, d='right', l=2.25)
d.add(dsp.AMP, label='c')
d.add(dsp.LINE, d='right', l=1.75)
d.add(dsp.ARROWHEAD, d='right')
d.add(dsp.SUM)

d.add(dsp.LINE, d='right', l=2.5)
d.add(dsp.ARROWHEAD, d='right')
d.add(dsp.LINE, d='down', l=height)
d.add(dsp.ARROWHEAD, d='down')
d.add(dsp.LINE, d='left', l=1)
d.add(dsp.BOX, label='$z^{-1}$')
L2 = d.add(dsp.LINE, d='left', l=1)
d.add(dsp.ARROWHEAD, d='left')

d.add(dsp.LINE, d='left', l=2.25)
d.add(dsp.AMP, label='c')
d.add(dsp.LINE, d='left', l=1.75)
d.add(dsp.ARROWHEAD, d='left')
d.add(dsp.SUM)
d.add(dsp.LINE, d='left', l=1.5)
d.add(e.DOT_OPEN, label='Output')

d.add(dsp.LINE, d='down', l=1.35, xy=L1.end)
d.add(dsp.AMP, label='s')
d.add(dsp.LINE, d='down', l=1.35)
d.add(dsp.ARROWHEAD, d='down')

d.add(dsp.LINE, d='up', l=1.4, xy=L2.end)
d.add(dsp.AMP, label='-s')
d.add(dsp.LINE, d='up', l=1.4)
d.add(dsp.ARROWHEAD, d='up')

d.draw()

# %% [markdown]
# To construct an allpass filter, the filter coefficients are chosen so that
# $s = \sin(\theta)$ and $c = \cos(\theta)$
# where $\theta$ is some numbeer between $-\pi$ and $\pi$.
#
# Now the first really cool part about this filter structure is that it
# can be extended to higher order filters recursively. As an example,
# we show a second-order filter below. The "NLF" box refers to the first-order
# ladder filter shown above.

# %%
d = SchemDraw.Drawing(fontsize=12, color='white')

height = 4

d.add(e.DOT_OPEN, label='Input')
L1 = d.add(dsp.LINE, d='right', l=2)
d.add(dsp.ARROWHEAD, d='right')
d.add(dsp.LINE, d='right', l=2.25)
d.add(dsp.AMP, label='c')
d.add(dsp.LINE, d='right', l=1.75)
d.add(dsp.ARROWHEAD, d='right')
d.add(dsp.SUM)

d.add(dsp.LINE, d='right', l=4.5)
d.add(dsp.ARROWHEAD, d='right')
d.add(dsp.LINE, d='down', l=height)
d.add(dsp.ARROWHEAD, d='down')
d.add(dsp.LINE, d='left', l=1)
d.add(dsp.BOX, label='NLF')
d.add(dsp.LINE, d='left', l=1)
d.add(dsp.BOX, label='$z^{-1}$')
L2 = d.add(dsp.LINE, d='left', l=1)
d.add(dsp.ARROWHEAD, d='left')

d.add(dsp.LINE, d='left', l=2.25)
d.add(dsp.AMP, label='c')
d.add(dsp.LINE, d='left', l=1.75)
d.add(dsp.ARROWHEAD, d='left')
d.add(dsp.SUM)
d.add(dsp.LINE, d='left', l=1.5)
d.add(e.DOT_OPEN, label='Output')

d.add(dsp.LINE, d='down', l=1.35, xy=L1.end)
d.add(dsp.AMP, label='s')
d.add(dsp.LINE, d='down', l=1.35)
d.add(dsp.ARROWHEAD, d='down')

d.add(dsp.LINE, d='up', l=1.4, xy=L2.end)
d.add(dsp.AMP, label='-s')
d.add(dsp.LINE, d='up', l=1.4)
d.add(dsp.ARROWHEAD, d='up')

d.draw()

# %% [markdown]
# The recursive nature shown above can be used to generate ladder filter
# structures of any order with great simplicity!
#
# ## Adding Nonlinearity
#
# We can now make the ladder filter nonlinear by changing the filter
# coefficients every sample so that $s = \sin(g * x[n])$ and $c = \cos(g * x[n])$,
# where $x[n]$ is the input sample at that timestep, and $g$ is some gain factor.
#
# When I first heard of this technique, I thought that it sounded more like
# a time-varying filter than a nonlinear process. After thinking about it
# for a while, I've concluded that in this process can be thought of as
# being either nonlinear or time-varying depending on how you look at it:
# If you see this filter as a linear filter with time-varying coefficients,
# then the process can be examined as a linear time-varying system, and that
# will be perfectly valid. However, you could also view this process as a
# stateful process with input-dependent gains, in which case it seems more
# like a nonlinear system, and can be analyzed as such.
#
# This sort of ambiguitity is actually kind of nice, since when we analyze
# this system we have many tools choose from. We can use the tools from
# time-varying filter theory, or we can use the tools that we typically use
# to analyze nonlinear systems, and both methods will be equally valid.
#
# Below, we show the response of a 100 Hz sine wave to a 4th-order nonlinear
# allpass filter.


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

def NLAllpass(x, order=2, func = lambda x : x):
    nlf = NLF(2)
    y = np.zeros(len(x))

    for n in range(len(x)):
        nlf.set_coefs(func(x[n]))
        y[n] = nlf.process(x[n])
    return y

# %%
fs = 44100
freq = 100
N = int(fs/2)
x = np.sin(2 * np.pi * np.arange(N) * freq / fs)

y = NLAllpass(x, func = lambda x : 3 * x)

# %%
ind = 3000
plt.plot (x[:ind])
plt.plot (y[:ind])

plt.title ('Sine Response for Nonlinear Allpass')
plt.xlabel ('Time [samples]')
plt.legend (['Dry', 'Wet'])

# %%
worN = np.logspace (1, 3.3, num=500, base=20)
adsp.plot_magnitude_response (y, [1], worN=worN, fs=fs, norm=True)
plt.ylim(-100)
plt.title ('Nonlinear Allpass Harmonic Response')

# %% [markdown]
# ## Adding more nonlinearity
#
# One thing I noticed after playing around with these nonlinear allpass filters
# for a little while, is that when increase the gain factor for the filter
# coefficients, the filter started become very harsh, and exhibit a "wrapping"
# effect on the input sine wave.

# %%
y = NLAllpass(x, func = lambda x : 15 * x)

ind = 3000
plt.plot (x[:ind])
plt.plot (y[:ind])

plt.title ('Sine Response for Nonlinear Allpass with Large Gain Factor')
plt.xlabel ('Time [samples]')
plt.legend (['Dry', 'Wet'])

# %% [markdown]
# Sometimes this wrapping sounds pretty cool, but I could definitely imagine
# situations where I might want a "smoother" sound. As a solution, I updated
# the filter coefficient equations to include another nonlinear function: a
# $\tanh$ soft clipper. So the new coefficient equations can be written as
# $s = \sin(\pi \tanh(g * x[n]))$ and $c = \cos(\pi \tanh(g * x[n]))$.
# (Note the extra factors of $\pi$ are to keep the range between $-\pi$
# and $\pi$.)


# %%
y = NLAllpass(x, order=4, func = lambda x : np.pi * np.tanh(15 * x))

ind = 3000
plt.plot (x[:ind])
plt.plot (y[:ind])

plt.title ('Sine Response for Nonlinear Allpass with Soft Clipper')
plt.xlabel ('Time [samples]')
plt.legend (['Dry', 'Wet'])

# %%
worN = np.logspace (1, 3.3, num=500, base=20)
adsp.plot_magnitude_response (y, [1], worN=worN, fs=fs, norm=True)
plt.ylim(-100)
plt.title ('Nonlinear Allpass with Soft Clipper Harmonic Response')

# %% [markdown]
# So now if the gain factor is raised to a very high level, the distortion
# of the nonlinear filter stays relatively tame.
#
# ## Implementation
#
# To give a better sense of the types of sounds
# this effect can create, I've created a simple
# audio plugin (VST, AU) that implements this
# effect. I also included 8x oversampling to help
# avoid nasty aliasing artifacts. To show how the
# nonlinear allpass filter can be used for synthesis
# I've also recorded a short video demo, using the
# nonlinear allpass on a simple sine wave. You can
# find the video on [YouTube](https://youtu.be/3A2DKdlSnlQ)
# and the source code on [GitHub](https://github.com/jatinchowdhury18/ComplexNonlinearities).