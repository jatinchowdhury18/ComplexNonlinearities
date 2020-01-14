# %%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp

import SchemDraw
from SchemDraw import dsp
import SchemDraw.elements as e

plt.style.use('dark_background')

# %% [markdown]
# # Subharmonics Generator
#
# Today we'll be talking about nonlinear signal processing
# from a little bit different perspective. Typically, in
# audio signal processing, we use nonlinear elements to
# generate frequency content, however this generated
# frequency content is almost always at frequencies higher
# than the original signal.
#
# For instance, the classic soft-clipping distortion effect
# generates signal at the odd harmonics of the original signal.
# As an example, the plot below shows the harmonic response of
# a soft-clipping distortion effect for a 100 Hz sine wave.

# %%
adsp.plot_harmonic_response(np.tanh, gain=2)
plt.title(r'Harmonic Response of $\tanh$ Soft Clipper')

# %% [markdown]
# In today's article, instead of frequency content above
# the original signal, we'll be generating content at lower
# frequencies than the original signal, sometimes known as
# "subharmonics".
#
# From here we'll look at the building blocks of generating
# low frequency content, and then show how to use this
# method to build a sort of "bass enhancer" effect.
#
# ## Creating Low Frequencies
#
# Let's start with a 2 kHz sine wave, and attempt to generate
# the first subharmonic frequency at 1 kHz. First, let's build
# a simple function to determine whether the input signal is
# increasing or decreasing, and output 1 for increasing, and -1
# for decreasing.

# %%
def get_direction(sig):
    y = np.copy(sig)

    rising = True
    last_x = 0
    for n, x in enumerate(sig):
        if rising == True and x < last_x:
            rising = False
        elif rising == False and x > last_x:
            rising = True

        y[n] = 1 if rising else -1

        last_x = x
    return y

# %%
N = 500
fs = 44100
freq = 2000
x = np.sin(2 * np.pi * freq / fs * np.arange(N))

y = get_direction(x)

plt.plot(x[:100])
plt.plot(y[:100])
plt.title('Input Rising/Falling')
plt.xlabel('Time [samples]')
plt.legend(['Input', 'Rise/Fall'], loc="lower right")

# %% [markdown]
# So with that pretty simple trick, we have a method for generating
# a square wave with the same frequency as the input. Now all we
# need to do is cut that frequency in half. We can accomplish this
# by simply switching our output between 1/-1 every *other* time
# the signal changes directions.

# %%
def get_square_half_freq(sig):
    y = np.copy(sig)

    rising = True
    last_x = 0
    output = 1
    switch_count = 0
    for n, x in enumerate(sig):
        y[n] = output

        if rising == True and x < last_x:
            switch_count += 1
            rising = False
        elif rising == False and x > last_x:
            switch_count += 1
            rising = True

        if switch_count == 2:
            output = 1 if output == -1 else -1
            switch_count = 0

        last_x = x
    return y

# %%
y = get_square_half_freq(x)

plt.plot(x[:100])
plt.plot(y[:100])
plt.title('Half Frequency Square Wave')
plt.xlabel('Time [samples]')
plt.legend(['Input', 'Generated'], loc="lower right")

# %% [markdown]
# Now that works for generating a square wave at the desired
# frequency, but what if we would prefer the much more pleasant
# sounding sine wave? Well if we lowpass filter the generated
# square wave, we can get something pretty close.

# %%
def get_sine_half_freq(sig, fs, filter_freq, filter_ord):
    y = get_square_half_freq (sig)

    eq = adsp.EQ(fs)
    Qs = adsp.butter_Qs(filter_ord)
    for n in range(int(filter_ord / 2)):
        eq.add_LPF(filter_freq, Qs[n])
    return eq.process_block(y)
    
# %%
y = get_sine_half_freq(x, fs, 1200, 6)

plt.plot(x[400:500])
plt.plot(y[400:500])
plt.title('Half Frequency Sine Wave')
plt.xlabel('Time [samples]')
plt.legend(['Input', 'Generated'], loc="lower right")

# %% [markdown]
# The difference is a little bit easier to notice
# if we look at the Fourier transforms of the two
# signals.

# %%
X = 20*np.log10(adsp.normalize(np.abs(np.fft.rfft(x))))
Y = 20*np.log10(adsp.normalize(np.abs(np.fft.rfft(y))))

plt.semilogx(X)
plt.semilogx(Y)
plt.title('Frequency Comparison')
plt.xlabel('Frequency')
plt.legend(['Input', 'Generated'])

# %% [markdown]
# Sure enough, the generated signal is at half the frequency
# of the original!
#
# ## Building A Bass Enhancer
#
# So we already know how to create a subharmonic tone generator,
# now let's see if we can incorporate the generator into a
# simple bass enhancer effect, something we can use to beef up
# a weak bass guitar or kick drum without adding any nasty
# artifacts or unwanted distortion.
#
# ### Level Detector
#
# First off, we don't our generator outputting any signal when
# there's no incoming signal. To do this, we can create a
# a level detector, much like one that we would use for a
# compressor or gate, and multiply the output of the generator
# by the detected level.

# %%
d = SchemDraw.Drawing(fontsize=12, color='white')
d.add (e.DOT_OPEN, label='Signal')
L1 = d.add (e.LINE, d='right', l=1)
d.add (e.LINE, d='up', l=1)
d.add (e.LINE, d='right', l=1.75)
d.add (dsp.BOX, label='LD')
d.add (e.LINE, d='right', l=1.75)
d.add (e.LINE, d='down', l=0.5)
M = d.add (dsp.MIX)
d.add (e.LINE, d='right', l=1, xy=M.N)
d.add (e.DOT_OPEN, label='Out')

d.add (e.LINE, d='down', l=1, xy=L1.end)
d.add (e.LINE, d='right', l=1)
d.add (dsp.BOX, label='Gen')
d.add (e.LINE, d='right', l=0.5)
d.add (dsp.BOX, label='LPF')
d.add (e.LINE, d='right', l=1)
d.add (e.LINE, d='up', l=0.5)

d.draw()


# %% [markdown]
# The signal flow diagram above shows the described architecture
# where the output of the generator and lowpass filter is
# multiplied by the output of the level detector (LD).
#
# We can give the level detector attack and release parameters,
# again similar to those used by a compressor, to help tune the
# characteristics of our bass enhancer. Specifically, by
# lengthening the release of our detector, we can make our
# generated bass signal seem to last longer, and "ring" out after
# the original sound has ended.
#
# The plot below shows the output of the generator with level
# detector architecture, with an attack time of 1 millisecond,
# and a release time of 5 milliseconds.

# %%
N = 5000
fs = 44100
freq = 2000
x = np.sin(2 * np.pi * freq / fs * np.arange(N))
x[:1000] = np.zeros(1000)
x[4000:] = np.zeros(1000)

y = get_sine_half_freq (x, fs, 1200, 6)
y *= adsp.level_detect (x, fs, attack_ms=1, release_ms=5)
y = adsp.normalize (y)

plt.plot(x)
plt.plot(y)
plt.title('Generated Subharmonic with Level Detector')
plt.xlabel('Time [samples]')
plt.legend(['Input', 'Generated'], loc="lower right")

# %% [markdown]
# ### Pre-Filter
#
# Finally, we need just one more modification for a full-on bass
# enhancer effect: a lowpass filter before the generator. In the
# real world, the input signal for our effect won't just be sine
# waves. In order to keep our generator from getting confused
# and creating signal at frequencies much higher than we would
# like, we can keep input more or less sine-like by filtering out
# all of the frequencies above where we want our effect to start
# creating subharmonics. The resulting architecture looks like
# this.

# %%
d = SchemDraw.Drawing(fontsize=12, color='white')
d.add (e.DOT_OPEN, label='Signal')
d.add (e.LINE, d='right', l=1)
d.add (dsp.BOX, label='LPF')
L1 = d.add (e.LINE, d='right', l=1)
d.add (e.LINE, d='up', l=1)
d.add (e.LINE, d='right', l=1.75)
d.add (dsp.BOX, label='LD')
d.add (e.LINE, d='right', l=1.75)
d.add (e.LINE, d='down', l=0.5)
M = d.add (dsp.MIX)
d.add (e.LINE, d='right', l=1, xy=M.N)
d.add (e.DOT_OPEN, label='Out')

d.add (e.LINE, d='down', l=1, xy=L1.end)
d.add (e.LINE, d='right', l=1)
d.add (dsp.BOX, label='Gen')
d.add (e.LINE, d='right', l=0.5)
d.add (dsp.BOX, label='LPF')
d.add (e.LINE, d='right', l=1)
d.add (e.LINE, d='up', l=0.5)

d.draw()


# %%
# @TODO: Implementation section