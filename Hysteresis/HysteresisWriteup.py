#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
from Hysteresis import Hysteresis_Process as HP

#%% [markdown]
# Today we get to discuss one of the most interesting, most complex
# nonlinearities around, which also happens to be one of my person
# favorites: Hysteresis. Hysteresis is a phenomenon that occurs everywhere
# in nature, from structural engineering, to biomechanicics, control systems,
# and even economics. A wonderful account of the extensive applications of
# hysteresis can be found on [Wikipedia](https://en.wikipedia.org/wiki/Hysteresis).
#
# I first encountered hysteresis when studying electromagnetism. It turns
# out that the way materials store magnetic energy exhibits a hysteretic
# behavior. In audio, we hear this as the characteristic distortion in
# components that use magnetic properties, including transformers and
# magnetic tape.
#
# A basic hysteresis curve looks like this:
#
# ![Pic](https://upload.wikimedia.org/wikipedia/commons/4/4d/Ehysteresis.PNG)
#
# Notice the arrows in the above diagram. This refers to the fact that
# a hysteresis curve distorts a signal differently depending on whether
# the signal is increasing or decreasing.
#
# What makes this nonlinearity so complex is that it is a
# "stateful" nonlinearity, in other words, the behavior of the nonlinearity
# is inherently tied to the previous inputs and outputs of the system.
# Other nonlinearities of this sort exist, however for hysteresis, the way
# in which previous states affect current behavior is particularly complex.
#
# What I'd like to do in the following article is to begin with a well-known
# model for magnetic hysteresis, the Jiles-Atherton model, and try to
# reconstruct it in a way that can be understood without a deep understanding of
# electromagnetic physics. (That said, for any technical readers who are interested
# in the physics, please checkout a
# [recent DAFx paper](http://dafx2019.bcu.ac.uk/papers/DAFx2019_paper_3.pdf) of mine.)

#%% [markdown]
# ## Developing the Nonlinearity
#
# The following section is going to be a litte bit technical. If you'd rather just skip
# ahead to the cool sounds, feel free to move on to the next section. That said, regardless
# of your mathematical background, I'd encourage you to give the following section a
# a try.
#
# To begin, let's examine two static nonlinearities that the Jiles-Atherton model
# is built on. For the technical reader, these functions are known as the Langevin
# function and its derivative, but for our purposes we'll just call them $L$ and $L'$.
# 
# $$ L(x) = \coth(x) - \frac{1}{x} $$
#
# $$ L'(x) = 1 - \coth(x)^2 + \frac{1}{x^2} $$
#
# where $\coth(x)$ refers to the "hyperbolic cotangent". Static curves for these
# nonlinearities can be seen below.

#%%
# Langevin function
def L (x):
    if isinstance (x, np.ndarray):
        t = np.copy (x)
        for n in range (len (x)):
            t[n] = L (t[n])
        return t

    if (abs (x) > 10 ** -4):
        return (1 / np.tanh (x)) - (1/x)
    else:
        return (x / 3)

# Langevin derivative
def L_d (x):
    if isinstance (x, np.ndarray):
        t = np.copy (x)
        for n in range (len (x)):
            t[n] = L_d (t[n])
        return t

    if (abs(x) > 10 ** -4):
        return (1 / x ** 2) - (1 / np.tanh (x)) ** 2 + 1
    else:
        return (1 / 3)

def plotNL (nl, range=1):
    x = np.linspace (-range, range, 100)
    y = nl (x)
    plt.plot (x,y)


plotNL (L, range=10)
plotNL (L_d, range=10)
plt.legend (['L', 'L\''])
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.title ('L and L\' Nonlinearities' )

#%% [markdown]
# Now let's define the quantity that we're going to be putting into our nonlinearities, $L$
# and $L'$, let's call it $Q$:
#
# $$ Q(x,y) = \frac{x + \alpha y}{a} $$
#
# Here $\alpha$ is just a constant value, which won't matter too much to us. $a$
# on the other hand will be a useful value for us later. $x$ is the input to our
# overall system, and $y$ is the output.
#
# Now we are going to need a couple values to keep track of the direction of the system.
# Recall from earlier that nonlinearity acts differently depending on whether the input
# is increasing or decreasing. Let's define two value $\delta_x$, and $\delta_y$:
#
# $$
# \delta_x = \begin{cases} 1 & \text{if $x$ is increasing} \\ -1 & \text{if $x$ is decreasing} \end{cases}
# $$
# 
# $$
# \delta_y = \begin{cases} 1 & \text{if $\delta_x$ and $L(Q) - y$ have the same sign} \\
#            0 & \text{otherwise} \end{cases}
# $$

#%% [markdown]
# Now we need to define the derivatives of the inputs and outputs, we'll call these
# $\dot{x}$ (for the input) and $\dot{y}$ (for the output). These values correspond
# to how quickly the input and output values are changing. To approximate these values
# in our digital system, we can use something called the
# [Trapezoidal Rule](https://ccrma.stanford.edu/~jos/pasp/Trapezoidal_Rule.html).
#
# $$
# \dot{x}[n] = 2 * f_s (x[n] - x[n-1]) - \dot{x}[n-1]
# $$
#
# Where $f_s$ refers to the sample rate of our digital system.
# This allows us to approximate the derivative of $x$ using only the
# values of past samples!
#
# Now we're about ready for the full Jiles-Atherton equation. Though it looks scary
# at first, try to break it down in terms of the pieces that we've already talked about.
#
# $$
# \dot{y} = \frac{\frac{(1-c)\delta_y(SL(Q) - y)}{(1-c)\delta_xk - \alpha(SL(Q) - y)}\dot{x} + c \frac{S}{a} \dot{x} L'(Q)}{1 - c\alpha \frac{S}{a} L'(Q)}
# $$
# 
# Again, this looks scary, but you may notice that there are only a few values that we haven't
# talked about yet. Two of them are constant values that we can mostly ignore: $k$ and $\alpha$.
# Three of the others ($a$, $c$, and $S$), are parameters that we will examine below. But
# everything else refers to values that we can calculate, using the $L$ and $L'$ functions we defined
# above, as well as our derivative calculator.
# 
# The final step is to calculate the output $y$ from the derivative
#  $\dot{y}$. This step is a bit more technical. All I'm going to say here is that
# I typically use the second order
# [Runge-Kutta Method](https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods),
# but other methods can work as well.
# 
# An example of calculating the hysteresis function with this method can be seen below:

#%%
def plotSineResponse (func, freq=100, seconds=1, fs=44100):
    n = np.arange (fs * seconds)
    x = np.sin (2 * np.pi * n * freq / fs)
    y = func (x)
    plt.plot (x[1000:], y[1000:])
    return x, y

fs = 44100
plotSineResponse (lambda x : HP (x, 1.0, 1.0/6.0, 1.6e-3, 30 * (1-0.5)**6 + 0.01, 0.1, 1/fs), fs=fs)
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.title ('Hysteresis Simulation')

#%% [markdown]
# ## Parameters
#
# Now we can start the fun stuff! Let's examine what happens to our curve when we start
# changing things.
# 
# ### Saturation
# 
# Fortunately for us, the parameter $S$ in the above equation maps almost
# perfectly to the level at which the hysteresis function saturates.
# Below we show examples of the hysteresis functions at varying saturation
# points.

#%%
def plotHysteresis (drive, width, sat, fs=44100, makeup=True, plotFunc=plotSineResponse):
    gain = 1 # 1e4
    M_s = gain * (0.5 + 1.5*(1-sat)) # saturation
    a = M_s / (0.01 + 6*drive) #adjustable parameter
    alpha = 1.6e-3

    k = 30 * (1-0.5)**6 + 0.01 # Coercivity
    c = (1-width)**0.5 - 0.01 # Changes slope
    makeup = (1 + 0.6*width) / (0.5 + 1.5*(1-sat)) if makeup else 1
    x, y = plotFunc (lambda x : makeup * HP (gain*x, M_s, a, alpha, k, c, 1/fs), fs=fs)
    return x, y

plt.figure()
for sat in [0, 0.33, 0.67, 1]:
    plotHysteresis (1, 0.8, sat, makeup=False)
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.title ('Hysteresis with varying saturation')

#%% [markdown]
# ### Drive
#
# Next we can adjust the $a$ parameter of the Jiles-Atherton equation. I've noticed
# that when adjusting this parameter as a fraction of $S$ it acts very
# similar to a "drive" control that you often see in distortion effects

#%%
plt.figure()
for drive in [0, 0.15, 0.4, 1]:
    plotHysteresis (drive, 0.8, 1.0, makeup=False)
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.title ('Hysteresis with varying drive')

#%% [markdown]
# ### Width
#
# Finally by adjusting the parameter $c$ from the Jiles-Atherton equation, we can change
# the width of the hysteresis loop. Physically, this parameter controls
# how much of the energy of the nonlinearity is distributed along the
# x-axis as opposed to the y-axis.

#%%
plt.figure()
for width in [0, 0.5, 0.88, 1]:
    plotHysteresis (1.0, width, 1.0, makeup=False)
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.title ('Hysteresis with varying width')

#%% [markdown]
# You may notice that changing the width also seems to adjust the
# saturation point. To correct for this, I've introduced a "makeup" gain,
# similar to one you might find on a compressor, that scales the output
# to give the same apparent saturation point regardless of width.

#%%
plt.figure()
for width in [0, 0.5, 0.88, 1]:
    plotHysteresis (1.0, width, 1.0, makeup=True)
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.title ('Hysteresis with varying width, and makeup gain')

#%% [markdown]
# ## Static vs. Dynamic Curves
# 
# I should mention that the static curves I've been showing above
# are a little bit misleading. I've been hiding a little bit of the
# true complexity of the hysteresis nonlinearity in order to keep my
# plots from getting cluttered. Recall that the static curves shown above
# plot the output of the nonlinearity for a steady-state input, in other
# words, an input with a constant overall gain. The hysteresis curve
# actually changes shape as the input gain changes. To see that in action,
# let's plot the response of our hysteresis nonlinearity to a sine wave
# with rising amplitude. Let's call this the "dynamic curve", as opposed
# to the static curve examined at a single amplitude.

#%%
def plotRisingSineResponse (func, freq=100, seconds=0.1, fs=44100):
    N = fs * seconds
    n = np.arange (N)
    x = np.sin (2 * np.pi * n * freq / fs) * (n/N)
    y = func (x)
    plt.plot (x, y)
    return x, y

plotHysteresis (0.7, 0.95, 0.2, makeup=True, plotFunc=plotRisingSineResponse)
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.title ('Hysteresis Dynamic Curve')

#%% [markdown]
# Now that's pretty neat. Audibly, this curve shows that we'll still hear the
# saturation in the signal even for small inputs, something that is
# typically not the case for simpler saturating nonlinearities. There's a
# lot more going on in this plot that I won't touch on here, but hopefully
# it gives you some sense of how complex the hysteresis nonlinearity really is.
#
# ## Harmonic Response
#
# Now let's take a minute to examine the harmonic response of a couple
# hysteresis curves. Below we see the dynamic curve and harmonic response
# for a relatively "extreme" hysteresis curve.

#%%
x, y = plotHysteresis (1.0, 1.0, 1.0, makeup=True, plotFunc=plotRisingSineResponse)
plt.xlabel ('Input Gain')
plt.ylabel ('Output Gain')
plt.title ('Extreme Hysteresis Dynamic Curve')

#%%
N = len (y)
Y = np.fft.rfft (y)
Y = Y / np.max (np.abs (Y))
f = np.linspace (0, 44100/2, int(N/2+1))

plt.semilogx (f, 20*np.log10 (np.abs (Y)))
plt.xlim (20, 20000)
plt.ylim (-90, 5)
plt.title ('Extreme Hysteresis Harmonic Response')
plt.xlabel ('Frequency [Hz]')
plt.ylabel ('Magnitude [dB]')

#%% [markdown]
# As you can see, we tend to get odd harmonics, but they seem to extend
# quite far up the frequency spectrum. For previous nonlinearities we have
# seen, the upper harmonics tend to fall below -60 dB after maybe a few kHz.
# For hysteresis however, we see high frequency content above -60 dB for
# almost the entire audible spectrum! Tuning the the parameters of
# the hysteresis curve will affect the relative amplitudes of the harmonics,
# but in general, the hysteresis nonlinearity will generate a *lot* of
# high frequency content.

#%% [markdown]
# ### Anti-aliasing
#
# Now how do we deal with so much high frequency content in the context of
# aliasing? The "brute force" answer is to oversample. I've found that
# oversampling by a factor of 32 can mitigate aliasing artifacts down to
# -72 dB, regardless of the shape of the hysteresis curve. In the future,
# I would like to use a method called "antiderivative antialiasing" to
# help mitigate aliasing artifacts with minimal oversampling. You may
# recall that I used this method a couple of articles ago to help with
# aliasiing artifacts from the
# [Double Soft Clipping](https://medium.com/@jatinchowdhury18/complex-nonlinearities-episode-1-double-soft-clipper-5ce826fa82d6)
# nonlinearity. The Double Soft Clipper was a memoryless nonlinearity, meaning
# it can be computed without any knowledge of the previous states of the
# system. Because hysteresis is a stateful nonlinearity, implementing
# antiderivative antialiasing becomes a much more difficult task.
# A [recent DAFx paper](http://dafx2019.bcu.ac.uk/papers/DAFx2019_paper_4.pdf)
# by Martin Holters describes the challenges of this problem, as well as
# offering an elegant solution, which I hope to implement soon.


#%% [markdown]
# ### Optimization
#
# The problem with running this hysteresis model at 32x oversampling
# is that the computational complexity increases by the same factor.
# In order to make sure that our CPU can run this effect in realtime
# without any artifacts or dropped samples, we need to make sure that
# our algorithm for implementing the hysteresis model is optimal.
#
# In signal processing, optimization can often be done by reducing the
# number of multiplications that the algorithm needs to compute per sample,
# and that can go a long way for our purposes as well. However, when I first
# implemented a hysteresis model, I found that the majority of my CPU's cycles
# were tied up computing the $\coth$ function needed by the $L$ and $L'$ functions
# discussed at the beginning. Most library functions for computing complex
# mathematical functions such as this prioritize accuracy over speed, and
# are designed to compute the correct result regardless of the given input.
# For our purposes, we're willing to give up a little bit of accuracy if
# we can improve our speed, plus we know the values given to the function
# will always be within a certain (relatively small) range. With that in
# mind, it can be useful to use an approximate function to calculate
# the $\coth$ function. I'd recommend using a Lambert Continued Fraction
# approximation, as described in [this article](https://varietyofsound.wordpress.com/2011/02/14/efficient-tanh-computation-using-lamberts-continued-fraction/),
# but other alternatives exist as well.

#%%
