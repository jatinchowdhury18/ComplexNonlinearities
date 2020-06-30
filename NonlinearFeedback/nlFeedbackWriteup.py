#%%
import numpy as np
import matplotlib.pyplot as plt
import audio_dspy as adsp

#%% [markdown]
# # Nonlinear Feedback Filters
#
# In [last week's article](https://medium.com/@jatinchowdhury18/complex-nonlinearities-episode-4-nonlinear-biquad-filters-ae6b3f23cb0e),
# we discussed a method for enhancing a standard
# framework for digital filters by adding nonlinear elements. In today's
# article we'll be looking at another similar method, with a pretty
# cool final sound.
#
# ## Developing the Structure
#
# Just like last time, we'll be starting the with a Biquad Filter, in
# the "Transposed Direct Form II". If these words don't mean anything to
# you, please take a minute to read through the beginning of the previous
# article, where we discuss these concepts in more detail.

#%%
from IPython.display import Image
Image(filename='../NonlinearBiquad/Pics/TDF-II.png')

#%% [markdown]
# You may recall that previously we added nonlinear elements just before
# each delay element, as shown below.

#%%
Image(filename='../NonlinearBiquad/Pics/NL-TDF-II.png')

#%% [markdown]
# Today we're going to try something a little bit different. What happens
# if we choose instead to add our nonlinear elements into the feedback paths
# of the filters.

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

Y = d.add (dsp.LINE, d='right', xy=SUM0.E, l=3.5, color='white')
d.add (dsp.LINE, d='down', l=3, color='white')
L3 = d.add (dsp.LINE, d='left', l=0.5, color='white')
d.add (dsp.BOX, label='NL', color='white')
d.add (dsp.LINE, d='left', l=0.5, color='white')
A1 = d.add (dsp.AMP, toplabel='$-a_1$', color='white')
d.add (dsp.LINE, d='left', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='left', color='white')

d.add (dsp.LINE, xy=L3.start, d='down', l=3, color='white')
d.add (dsp.LINE, d='left', l=0.5, color='white')
d.add (dsp.BOX, label='NL', color='white')
d.add (dsp.LINE, d='left', l=0.5, color='white')
A1 = d.add (dsp.AMP, toplabel='$-a_2$', color='white')
d.add (dsp.LINE, d='left', l=0.75, color='white')
d.add (dsp.ARROWHEAD, d='left', color='white')

d.add (dsp.LINE, xy=Y.end, d='right', l=1, color='white')
d.add (e.DOT_OPEN, label='Output', color='white')

d.draw()

#%% [markdown]
# ### Frequency Response
#
# Like last time, let's now take a look at what happens to the frequency
# response of the filter when we change the input gain. For this example,
# we'll use a lowpass filter with a cutoff at 1 kHz, a Q of 10, and $\tanh$
# clippers for our nonlinear elements.

#%%
class NLFeedback:
    def __init__ (self):
        self.z = np.zeros (3)
        self.b = np.array ([1, 0, 0])
        self.a = np.array ([1, 0, 0])
        self.fb_lambda = lambda a,x : a*x
    
    def setCoefs (self, b, a):
        assert (np.size (b) == np.size (self.b))
        assert (np.size (a) == np.size (self.a))
        self.b = np.copy (b)
        self.a = np.copy (a)

    def processSample (self, x):
        # Direct-Form II, transposed
        y = self.z[1] + self.b[0]*x
        self.z[1] = self.z[2] + self.b[1]*x - self.fb_lambda (self.a[1],y)
        self.z[2] = self.b[2]*x - self.fb_lambda (self.a[2],y)
        return y

    def process_block (self, block):
        for n in range (len (block)):
            block[n] = self.processSample (block[n])
        return block

#%%
fs = 44100
b, a = adsp.design_LPF2 (1000, 10, fs)

sweep = adsp.sweep_log (20, 22000, 1, fs)

legend = []
freqs = np.logspace (1, 3.4, num=1000, base=20)
for g in [0.00001, 0.04, 0.2, 1.0]:
    cur_sweep = np.copy (sweep) * g
    y = np.copy (cur_sweep)

    filter = NLFeedback()
    filter.setCoefs (b, a)
    filter.fb_lambda = lambda a,x : a*np.tanh (x)
    y = filter.process_block (y)

    h = adsp.normalize (adsp.sweep2ir (cur_sweep, y))
    adsp.plot_magnitude_response (h, [1], worN=freqs, fs=fs)

    if g < 0.0001: legend.append ('Linear')
    else: legend.append ('Nonlinear (A={})'.format (g))

plt.title ('Lowpass Filter with nonlinear feedback')
plt.legend (legend)
plt.grid()
plt.ylim (-10)
plt.xlim(20, 20000)
plt.savefig('D:\\Documents\\CCRMA\\Research\\Complex_Nonlinearities\\NonlinearFeedback\\Pics\\LPF-NL.png')

#%% [markdown]
# From the above plot, it seems pretty obvious that as the gain of the input
# signal increases, the resonant frequency of the filter seems to shift
# as well. Vadim Zavalishin describes a similar effect that occurs when
# adding nonlinear elements to the feedback paths of a ladder filter, and
# he has a useful way of thinking about this phenomenon: "as audio-rate
# modulation of the cutoff (frequency)"[1]. The result of this modulation
# will give some pretty neat sounds which we'll hear more about below.
#
# [1]: *The Art of Virtual Analog Filter Design* (rev. 2.1.0),
# Vadim Zavalishin (https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.0.pdf)

#%% [markdown]
# ## Pole Analysis
#
# I'm going take a small leap here, and try to do a little bit of math
# to show why the nonlinear filter reacts in this way. If the math isn't
# making much sense, feel free to skip this section. First, 
# we need to define a couple of things.
#
# ### Filter Poles
#
# The resonant frequencies of a filter are defined by it's "poles".
# These poles are complex values that show where the feedback of the
# filter reaches a maximum. To find the poles for a linear filter, we
# can use the quadratic formula on the $a$ coefficients of our filter
# as defined in the Direct Form.
#
# $$
# p_{linear} = \frac{-a_1 \pm \sqrt{a_1^2 - 4a_2}}{2}
# $$
#
# The resonant frequencies of the filter are then defined by the "angle"
# of this complex number, which can be found using
#
# $$
# \angle p_{linear} = \arctan \left(\frac{\sqrt{|a_1^2 - 4a_2|}}{a_1} \right)
# $$
#
# A larger pole angle corresponds to a higher cutoff frequency, and a smaller
# pole angle corresponds to a lower cutoff frequency.
#
# ### Nonlinear Gain
#
# The next thing to consider is that a nonlinear function can also be written
# as a dependent gain. For example, the dependent gain for the $\tanh$
# nonlinearity looks like this:
# 
# $$
# g_{tanh} (x) = \frac{\tanh(x)}{x}
# $$
# 
# For the $\tanh$ nonlinearity (as with many nonlinearities), the dependent
# gain will always be in between 0 and 1, and as the input increases, the
# gain decreases to zero.

#%%
adsp.plot_static_curve (lambda x : np.tanh (x))
adsp.plot_static_curve (lambda x : np.tanh (x)/x)
plt.title (r'Comparing $\tanh$ nonlinearity, to dependent gain')
plt.legend (['Tanh', 'Dependent gain'])

#%% [markdown]
# ### What happens to the poles?
#
# Now for the million dollar question: what happens to the poles of the filter
# when we add the nonlinear elements? The basic answer is that the $a$
# coefficients are multiplied by the dependent gain values. Then the new pole
# locations can be described by
#
# $$
# p_{nonlinear} = \frac{-g_{tanh}(x) a_1 \pm \sqrt{g_{tanh}^2(x) a_1^2 - 4g_{tanh}(x) a_2}}{2}
# $$
#
# And then the new pole angles become:
#
# $$
# \angle p_{nonlinear} = \arctan \left(\frac{\sqrt{|g_{tanh}^2(x)a_1^2 - 4g_{tanh}(x)a_2|}}{g_{tanh}(x)a_1} \right) 
# $$
#
# I won't go through the math here in gory detail, but using the property
# that the gain goes to zero as $x$ grows large, we can show that the poles
# shift to larger angles for larger input gain.This movement of the poles
# explains why the cutoff frequency of the filter changes with the
# input gain.

#%% [markdown]
# ## Anti-Aliasing
#
# As usual, it's worth taking a minute to discuss that harmonic response
# of this system, and think about any aliasing problems we might have.
# To get a sense of that we're dealing with, I've taken the example filter
# from earlier (cutoff = 1 kHz, Q = 10, nonlinearity = $\tanh$), and plotted
# it's harmonic response for a 100 Hz, and a 2kHz sine wave.

#%%
fs = 44100
b, a = adsp.design_LPF2 (1000, 10, fs)

filter = NLFeedback()
filter.setCoefs (b, a)
filter.fb_lambda = lambda a,x : a*np.tanh (x)

plt.figure()
plt.title ('Harmonic Response (100 Hz)')
adsp.plot_harmonic_response (lambda x : filter.process_block (x), freq=100, fs=fs, gain=0.5)

plt.figure()
plt.title ('Harmonic Response (2 kHz)')
adsp.plot_harmonic_response (lambda x : filter.process_block (x), freq=2000, fs=fs, gain=0.5)

#%% [markdown]
# So we get kind of an interesting response! Along with a larger bump around
# the resonant frequency of the filter, we see a few odd harmonics generated
# for each input. It also seems like the filter does a little bit of filtering
# of its own generated harmonics, which is pretty interesting. In my own
# rather subjective testing, I haven't had much of an audible issue with
# aliasing artifacts, except for filters with very high resonance above
# 10 kHz, which sound pretty annoying anyway. Just to be safe, I like to
# use 8x oversampling, which typically mitigates any aliasing artifacts
# down to ~90 dB.

#%% [markdown]
# ## Examples
#
# Once again, please check out an implementation of this nonlinear feedback
# filter as an audio plugin on [GitHub](https://github.com/jatinchowdhury18/ComplexNonlinearities).
# There's also a video demo available on [YouTube](https://youtu.be/T0AsIX5oL9A).

#%% [markdown]
# ## P.S.
#
# Those of you who have been reading the previous articles in this series
# may notice that my Python code in this notebook seems considerably
# cleaner and more concise than usual. In an effort to keep my code
# more consolidated and organized, I have been moving a lot of the code
# that I tend to reuse often to a separate Python package. I've uploaded
# this package to GitHub and PyPi, in case anyone out there feels like they
# want to use it or contribute to it. Enjoy: https://github.com/jatinchowdhury18/audio_dspy.

#%%
