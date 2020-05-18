# %%
import numpy as np
import scipy.signal as signal
from scipy.io import wavfile
import audio_dspy as adsp
from Hysteresis import Hysteresis_Process as HP

import matplotlib.pyplot as plt
from matplotlib import patches
plt.style.use('dark_background')

# %% [markdown]
# # Digitizing Nonlinear Differential Equations with the Alpha Transform
#
# Previously on this blog, we've looked a creating digital models
# of several interesting nonlinear systems. One of the most
# sophisticated nonlinear systems that we've looked at is
# [hysteresis](https://medium.com/@jatinchowdhury18/complex-nonlinearities-episode-3-hysteresis-fdeb2cd3e3f6).
#
# This hysteresis model is also the engine for an analog tape
# machine [plugin](https://github.com/jatinchowdhury18/AnalogTapeModel)
# that I develop and maintain. Recently, some
# users asked me for a mode of the plugin that could run without
# oversampling, in order to reduce CPU footprint. This request
# led me to an interesting discretization concept known as the
# "alpha transform", which I thought could be a good teaching
# moment as well.

# %% [markdown]
# ## Hysteresis Revisited
# As you may recall from the hysteresis article, we can model
# the hysteresis function using a nonlinear differential
# equation. The resulting model gives the following dynamic
# response.

# %%
def getRisingSineResponse (func, freq=100, seconds=0.08, fs=44100):
    N = fs * seconds
    n = np.arange (N)
    x = np.sin (2 * np.pi * n * freq / fs) * (n/N)
    y = func (x)
    return x, y

def getHysteresisRisingSine (drive, width, sat, freq=100, seconds=0.08, fs=44100, dAlpha=1.0):
    gain = 1 # 1e4
    M_s = gain * (0.5 + 1.5*(1-sat)) # saturation
    a = M_s / (0.01 + 6*drive) #adjustable parameter
    alpha = 1.6e-3

    k = 30 * (1-0.5)**6 + 0.01 # Coercivity
    c = (1-width)**0.5 - 0.01 # Changes slope
    makeup = (1 + 0.6*width) / (0.5 + 1.5*(1-sat))
    return getRisingSineResponse (lambda x : makeup * HP (gain*x, M_s, a, alpha, k, c, 1/fs, dAlpha=dAlpha), freq=freq, seconds=seconds, fs=fs)

# %%
DRIVE = 0.8
WIDTH = 1.0
SAT = 0.8

plt.figure()
x, y = getHysteresisRisingSine (DRIVE, WIDTH, SAT)
plt.plot(x, y)

plt.title('Hysteresis Response')
plt.xlabel('Input Signal')
plt.xlabel('Output Signal')

# %% [markdown]
# While I won't re-write the hysteresis differential equation here, all
# we really need to know is that the equation defines the derivative of the
# output signal $\dot{y}$ as a function of the derivative
#  of the input signal $\dot{x}$, i.e.
# 
# $$ \dot{y} = f(\dot{x}) $$
#
# To get from these derivatives back to the actual signals that we want
# requires some form of numerical integration. The most often used method
# for doing this in digital signal processing is the
# [trapezoidal rule](https://ccrma.stanford.edu/~jos/pasp/Trapezoidal_Rule.html)
# also called the bilinear transform.
#
# $$ x[n] = x[n-1] + \frac{\dot{x}[n] - \dot{x}[n-1]}{2 f_s} $$
#
# Where $f_s$ is the sample rate of the digital system.
#
# While the bilinear transform is widely used, it can have issues when
# discretizing nonlinear systems, specifically exhibiting an oscillating
# instability when excited by signal at the Nyquist frequency. Let's see what
# happens when we try putting signal at the Nyquist frequency through the
# hysteresis model using the bilinear transform.
#

# %%
plt.figure()
x, y = getHysteresisRisingSine (DRIVE, WIDTH, SAT, freq=20000)
plt.plot(x, y)

plt.title('Hysteresis Response at 20kHz')
plt.xlabel('Input Signal')
plt.xlabel('Output Signal')


# %% [markdown]
# Clearly the system goes unstable! Below we'll take a brief look at
# what factors cause this instability in some nonlinear systems, and then
# examine how we can avoid this instability using the alpha transform.
#
# ## Pole Mappings
#
# A useful way to analyze systems is by looking at their poles. While the
# technical definition of a "pole" is requires some explanation, for our
# purposes, a pole is essentially a resonant frequency of the system.
# For nonlinear systems, these resonant frequencies can change very
# quickly and erratically depending on the behavior of the systems.
#
# When looked at in terms of pole locations, the bilinear transform (or
# any transform) can be viewed as a pole mapping, that maps analog poles
# to digital poles. Specifically, the bilinear transform maps poles along
# the frequency spectrum to various locations within the unit circle. A
# general rul of thumb is that any poles outside the unit circle will cause
# the system to be unstable. This unit circle is typically defined to reside
# in the "z-plane".
#
# The reason why the bilinear transform causes an instability in our
# hysteresis system is that it maps the frequency $f = \infty$ to
# $z =-1$, which is directly on the unit circle. In highly nonlinear
# systems, poles can easily move to very large frequencies, which in
# in our numerical approximation can trigger this undesirable mapping.
# Finally, when this pole is excited by signal at the Nyquist frequency,
# which corresponds to $z = -1$ in digital systems, the system will go
# unstable, as shown above.
#
# The typical method for handling this instability is to use oversampling.
# When oversampling with the correct anti-imaging and anti-aiasing filters,
# the system will never encounter signal at or near the Nyquist frequency,
# thereby ensuring the stability of the system.

# ## Alpha Transform
# However, what if we don't want to use oversampling, due to the
# computational cost? An alternative is to use a technique known as
# the "alpha transform". I was introduced to this transform through the
# wonderful work of Francois Germain and Kurt Werner, specifically their
# paper on [Mobius Transforms](https://www.ntnu.edu/documents/1001201110/1266017954/DAFx-15_submission_52.pdf),
# of which the Alpha Transform is a special case.
# 
# Using the alpha transform from earlier, we can re-write our integration
# forumla from earlier:
#
# $$ x[n] = x[n-1] + \frac{\dot{x}[n] - \dot{x}[n-1]}{(1 + \alpha) f_s} $$
#
# Note that the alpha transform has a parameter $\alpha$ that allows us
# some control over the pole mapping.

# %%
ax = plt.subplot(111)
uc = patches.Circle((0, 0), radius=1, fill=False, color='white')
ax.add_patch(uc)

ax.spines['left'].set_position('center')
ax.spines['bottom'].set_position('center')
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
plt.axis('scaled')

ticks = []
for n in np.linspace(np.max([1.0, 1]), 0, endpoint=False, num=2):
    ticks.append(n)
    ticks.append(-n)
plt.xticks(ticks)
plt.yticks(ticks)

colors = ['red', 'orange', 'y', 'c']
alphas = [0.0, 0.5, 0.8, 1.0]
for alpha, c in zip(alphas, colors):
    R = (1+alpha)/2
    centerX = 1 - R
    ac = patches.Circle((centerX, 0), radius=R,
        fill=False, color=c, ls='dashed', label=fr'$\alpha = {alpha}$')
    ax.add_patch(ac)

plt.title('Alpha Transform Mappings')
plt.legend(bbox_to_anchor=(0.92, 1), loc='upper left')

# %% [markdown]
# As shown above, for $\alpha$ less than 1, the transform maps high
# frequency poles to inside the unit circle, avoiding the potential
# instabiliy of the bilinear transform. Let's try running our hysteresis
# model at the Nyquist frequency again, this time using an alpha transform
# with $\alpha = 0.85$.

# %%
plt.figure()
x, y = getHysteresisRisingSine (DRIVE, WIDTH, SAT, freq=20000, dAlpha=0.85)
plt.plot(x, y)

plt.title(r'Hysteresis Response at 20kHz ($\alpha$=0.85)')
plt.xlabel('Input Signal')
plt.xlabel('Output Signal')

# %% [markdown]
# Clearly this output is erratic, as would be expected at this high frequency.
# but fortunately, it seems to be stable!
#
# The downside of the alpha transform is that it can introduce damping at
# high frequencies. Let's compare the output of the hysteresis model using
# the bilinear transform, to the output of the model using the alpha
# transform to make sure the damping isn't going to be problematic.


# %%
def plotCompare(freq, seconds):
    plt.figure()
    x, y = getHysteresisRisingSine (DRIVE, WIDTH, SAT, freq=freq, seconds=seconds, dAlpha=1.0)
    plt.plot(x, y, label='Bilinear')

    x, y = getHysteresisRisingSine (DRIVE, WIDTH, SAT, freq=freq, seconds=seconds, dAlpha=0.85)
    plt.plot(x, y, '--', label='Alpha')

    plt.title(f'Hysteresis Response at {freq/1000}kHz')
    plt.xlabel('Input Signal')
    plt.xlabel('Output Signal')
    plt.legend()

plotCompare(1000, 0.008)
plotCompare(2000, 0.006)
plotCompare(4000, 0.002)

# %% [markdown]
# Up to 4kHz, the damping is essentially negligible, and even above that,
# the damping is pretty difficult to hear. Of course, using oversampling,
# will help avoid this damping as well.
#
# ## Finally
# Thanks for reading through to the end! This article is certainly more
# technical than most of my other posts, but I think it covers some
# important concepts for implementing real-time models of nonlinear
# systems. If you'd like to see thiss hysteresis model in action,
# definitely check out my [tape machine plugin](https://github.com/jatinchowdhury18/AnalogTapeModel),
# which implements all of the concepts discussed here, and many more.
# Thanks!


# %%
