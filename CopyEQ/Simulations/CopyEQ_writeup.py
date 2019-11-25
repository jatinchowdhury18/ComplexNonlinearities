#%%
import numpy as np
import audio_dspy as adsp
import scipy.signal as signal
import matplotlib.pyplot as plt
plt.style.use('dark_background')

# %% [markdown]
# # Extensions on Adaptive Equalization
#
# In this article we'll be examining some interesting uses
# of adaptive filtering for audio equalization, and how
# it can be extended with frequency warping and
# nonlinearities. The resulting effect can be used for
# setting an EQ filter that makes one instrument sound
# optimally similar to another, or to create an interesting
# effect that "copies" the frequencies from one sound onto
# another.
#
# ## Adaptive Equalization
#
# First let's examine the basic mechanism for Adative EQing: the LMS adaptive
# filter. An adaptive filter is simplyy a filter that takes in an input and a
# desired output, and adjusts its filter shape in real-time to produce an output
# that is optimally similar to the desired output. While there are several methods
# for performing the optimization step, such as Least Mean Squared (LMS),
# Normalized Least Mean Squared (NLMS), and Recursive Least Square (RLS), for
# this use case, we prefer LMS. The reason for choosing LMS is that NLMS doesn't
# take into account the amplitude of the input signal when adjusting the weights
# thereby causing the filter to react too much to low-level signals, and RLS
# doesn't do a good job of "forgetting" previous samples, making it react
# poorly for rapidly changing signals like we often find in audio.
#
# Below we show a simple example of using an adaptive EQ to approximate
# a pure sine wave from a white noise signal.

# %%
fs = 44100
N = int(0.5 * 44100)
freq = 8000

desired = np.sin(2 * np.pi * np.arange(N) * freq / fs)

input = np.random.uniform(-1.0, 1.0, N)

output, w, e = adsp.LMS(input, desired, 0.001, 128)

# %%
def plot_specgram(sig, title):
    plt.figure()
    f, t, Zxx = signal.stft(sig, fs=fs, nperseg=256, nfft=4096)
    plt.imshow(20 * np.log10(np.abs(Zxx)), cmap='inferno', origin='lower',
               aspect='auto', extent=[np.min(t), np.max(t), np.min(f), np.max(f)])
    plt.title(title)
    plt.xlabel('Time [s]')
    plt.ylabel('Frequency [Hz]')

plot_specgram(desired, 'Desired Signal')
plot_specgram(input, 'Input Signal')
plot_specgram(output, 'Output Signal')

# %% [markdown]
# So that's pretty cool! DSP engineers use this technique often for signal
# "prediction" where they can use white noise to approximate any desired signal.
# The reason why white noise works well for this is that it has frequency content
# at all frequencies, while your average audio signal may not. However, 
# because adaptive filters are time varying, they can actually shift frequencies
# as well. As an example, we can use our adaptive filtering algorithm to
# filter a sine wave at one frequency to predict a sine wave at another.

# %%
freq1 = 5000
freq2 = 10000

desired = np.sin(2 * np.pi * np.arange(N) * freq2 / fs)
input = np.sin(2 * np.pi * np.arange(N) * freq1 / fs)
output, w, e = adsp.LMS(input, desired, 0.001, 128)

plot_specgram(desired, 'Desired Signal')
plot_specgram(input, 'Input Signal')
plot_specgram(output, 'Output Signal')

#%% [markdown]
# So that works pretty well. But what if there was a way to make our signal a bit
# more broadband, a bit more like white noise, without losing it's amplitude
# envelope, or overall melody and harmony.
#
# ## Nonlinearities
#
# The solution to this problem can come from putting our signal through a
# nonlinear function before putting it through the adaptive filter,
# since the nonlinear function can generate more frequencies.

# %%
freq1 = 1000
freq2 = 9000

desired = np.sin(2 * np.pi * np.arange(N) * freq2 / fs)
input0 = np.sin(2 * np.pi * np.arange(N) * freq1 / fs)
input1 = adsp.soft_clipper(input0, deg=7)
output, filt, e = adsp.LMS(input1, desired, 0.01, 128)

plot_specgram(desired, 'Desired Signal')
plot_specgram(input0, 'Input Signal')
plot_specgram(input1, 'Input Signal after nonlinearity')
plot_specgram(output, 'Output Signal')


# %% [markdown]
# Above we show another example of frequency shifting with our adaptive filter,
# but with a saturating nonlinearity at the input of the filter. A couple of
# interesting things to note here: First, the extra harmonics added by the
# nonlinearity allow for a smoother frequency shift, since there is more
# high frequency content to aid in the frequency shifting. Second, notice
# that the output signal has an interesting harmonic structure to it as well,
# with a number of both overtones and undertones. Adaptive filtering with a
# nonlinear input can give some pretty cool sounding and unique timbres (an
# audio example will be given later on).
#
# ## Frequency Warping
#
# One other modification we can make to our adaptive filter is frequency warping.
# Frequency warping allows us to emphasize certain frequencies, making sure that
# the adpative filter optimizes certain frequency bands more than others. The
# idea behind frequency warping is pretty simple: we can pass the signal through
# a first order allpass filter that stretches out the frequencies we want to
# emphasize.
#
# As an example of the problem, below we show the spectrum of four sine waves,
# at 100, 300, 9000, and 10000 Hz. Audibly, we hear a big difference between
# 100 and 300 Hz, not so much between 9000 and 10000 Hz. Yet when we look at
# the spectrum linearly (much as our adaptive filter will see it), it's
# pretty hard to distinguish between the two low-frequency signals, and pretty
# easy to distinguish between the high-frequency ones.

# %%
freqs = [100, 300, 9000, 10000]

sig = np.zeros(N)
for freq in freqs:
    sig += np.sin(2 * np.pi * np.arange(N) * freq / fs)


f = np.linspace(0, fs/2, N/2+1)
plt.plot(f, 20*np.log10(np.abs(np.fft.rfft(sig))))
plt.title('Spectrum w/out warping')
plt.ylabel('Amplitude [dB]')
plt.xlabel('Frequency [Hz]')

# %% [markdown]
# We can fix this with allpass warping. Let's try passing out signal through
# an allpass filter with a "warping factor" of -0.72, and see what happens.

# %%
warp_factor = -0.72
sig_warp = adsp.allpass_warp(-warp_factor, sig)

plt.plot(f, 20*np.log10(np.abs(np.fft.rfft(sig_warp))))
plt.title('Spectrum w/ warping')
plt.ylabel('Amplitude [dB]')
plt.xlabel('Frequency [Hz]')

# %% [markdown]
# So with allpass warping, we get a pretty drastic improvement in the
# frequency resolution of our filter at low frequencies where we care
# about it most. While the tradeoff is that our frequency resolution is
# is worse at high frequencies, I've found that by tuning the "warping
# factor", it's not too hard to find a happy medium, where the resolution
# across all frequencies is "just right". For our adaptive filter, this
# means we can tune our filter to capture the frequency spectrum of the
# desired signal with an emphasis on the frequencies we care about most.

# %% [markdown]
# # Implementation
#
# As an example of all the concepts we've discussed above, I've developed an
# audio plugin that implements an adaptive EQ, with parameters for frequency
# warping, and nonlinear processing. I've also added simple parameters
# to subtract white noise from the desired signal, and to choose whether or
# not to warp the desired signal. Note that a typical audio plugin only takes
# a single input, but we can make the desired signal a sidechain input.
# Feel free to checkout the source code on [GitHub](https://github.com/jatinchowdhury18/ComplexNonlinearities),
# or checkout a video demo on [YouTube](https://youtu.be/NjOCmKuiLB8).

# %%
