# %%
import numpy as np
import matplotlib.pyplot as plt
import audio_dspy as adsp

import SchemDraw
from SchemDraw import dsp
import SchemDraw.elements as e

plt.style.use('dark_background')
plt.tight_layout()

# %% [markdown]
# # Gated Recurrent Distortion
#
# Today we're going to be discussing an interesting
# type of distortion effect, based around the idea
# of a Gated Recurrent Unit (GRU). First introduced
# by [Cho et al](https://arxiv.org/abs/1406.1078) in
# 2014, GRUs have become a basic building block of
# Recurrent Neural Networks. However, when analyzing
# the structure of these units, I started wondering
# about the possibility of using the GRU architecture
# for an interesting type of distortion effect.
#
# ## GRU Architecture
#
# A GRU is made up of two simple nonlinearities: the
# sigmoid and $tanh$ nonlinearities, both shown below.
# While these curves look similar, note that the
# sigmoid function goes from 0 to 1, while the $tanh$
# function goes from -1 to 1.

# %%
def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))

plt.figure()
adsp.plot_static_curve(sigmoid)
plt.title('Sigmoid Nonlinearity')
plt.savefig('Pics/sigmoid.png')

plt.figure()
adsp.plot_static_curve(np.tanh)
plt.title('Tanh Nonlinearity')
plt.savefig('Pics/tanh.png')

# %% [markdown]
# Using these basic nonlinear building blocks we can
# construct a simple type of GRU known as a "minimal
# gated unit" (introduced by
# [Heck and Salem, 2017](https://arxiv.org/abs/1701.03452)).
# The signal processing architecture for this unit is
# shown below, with "Sg" denoting the sigmoid nonlinearity.

# %%
down = 4
d = SchemDraw.Drawing(fontsize=12, color='white')
d.add(e.DOT_OPEN, label='Input')
L1 = d.add(dsp.LINE, d='right', l=1)
d.add(dsp.LINE, d='right', l=1)
d.add(dsp.AMP, toplabel='$W_h$')
d.add(dsp.LINE, d='right', l=0.75)
d.add(dsp.ARROWHEAD, d='right')
SUM0 = d.add(dsp.SUM)
d.add(e.LINE, d='right', l=1)
d.add(dsp.BOX, label='Tanh')
d.add(e.LINE, d='right', l=1)
d.add(dsp.AMP, label='1-f')
d.add(e.LINE, d='right', l=0.75)
d.add(dsp.SUM)
L2 =  d.add(e.LINE, d='right', l=1)
d.add(e.LINE, d='right', l=1)
d.add(e.DOT_OPEN, label='Output')

d.add(e.LINE, d='down', l=down*0.6, xy=L2.end)
L3 = d.add(e.LINE, d='down', l=down*0.4)
d.add(dsp.LINE, d='left', l=1)
d.add(dsp.AMP, toplabel='$U_f$')
d.add(dsp.LINE, d='left', l=2.75)

d.add(e.LINE, d='down', l=down, xy=L1.end)
d.add(dsp.LINE, d='right', l=1)
d.add(dsp.AMP, toplabel='$W_f$')
d.add(dsp.LINE, d='right', l=0.75)
S2 = d.add(dsp.SUM)
d.add(dsp.LINE, d='right', l=1)
S1 = d.add(dsp.SUM)
d.add(e.LINE, d='down', l=0.5, xy=S1.S)
d.add(dsp.BOX, label='Sg')
d.add(e.LINE, d='down', l=0.5)
d.add(e.DOT_OPEN, botlabel='f')
d.add(e.LINE, d='down', l=0.75, xy=S2.S)
d.add(e.DOT_OPEN, botlabel='$b_f$')

L4 = d.add(e.LINE, d='left', l=1.5, xy=L3.start)
d.add(e.LINE, d='up', l=0.6)
d.add(dsp.AMP, label='f')
d.add(e.LINE, d='up', l=0.5)

d.add(e.LINE, d='left', l=3, xy=L4.end)
d.add(dsp.AMP, label='$U_h$*f')
d.add(e.LINE, l=1.75)
d.add(e.LINE, d='up', l=1.9)

d.draw()
plt.savefig('Pics/gru_arch.png')

# %% [markdown]
# In this writing, I'm not going to discuss the
# mathematics of GRUs and minimal gated units in
# depth, but for more information, definitely take
# a look at the linked papers, as well as the Wikipedia
# article linked [here](https://en.wikipedia.org/wiki/Gated_recurrent_unit)
#
# ## Distortion Curve and Parameters
#
# What makes this type of distortion so
# interesting, is its wonderful potential for
# parameterization. Specifically, the architecture
# we use here has 5 parameters: Wf, Wh, Uf, Uh, and bf.
# Below we show how these parameters can change the
# shape of the distortion curve.
#
# The simplest parameter is Wh, which acts as a basic
# "drive" parameter:

# %%
def gru(x, yPrev, Wf, Wh, Uf, Uh, bf):
    f = sigmoid(Wf*x + Uf*yPrev + bf)
    y = f*yPrev + (1-f) * np.tanh(Wh*x + Uh*f*yPrev)
    return y

def gru_block(x, Wf, Wh, Uf, Uh, bf):
    y1 = 0
    y = np.zeros_like(x)
    for n in range(len(x)):
        y[n] = gru(x[n], y1, Wf, Wh, Uf, Uh, bf)
        y1 = y[n]

    return y

# %%
legend = []
for Wh in [0, 0.2, 0.5, 1.5]:
    func = lambda x : gru_block(x, 0.0, Wh, 0.0, 0.0, 0.0)
    legend.append(r'$W_h = {}$'.format(Wh))
    adsp.plot_dynamic_curve(func, gain=8)

plt.legend(legend)
plt.title('GRU Distortion with Varying Wh')
plt.savefig('Pics/wh.png', bbox_inches="tight")

# %% [markdown]
# Wf and Uf change the shape of the top half of the
# distortion curve, to have more saturation and more
# width between the forward and backward parts of the
# curve.

# %%
plt.figure()
legend = []
for Wf in [0, 2, 5, 10]:
    func = lambda x : gru_block(x, Wf, 2.0, 0.0, 0.0, 0.0)
    legend.append(r'$W_f = {}$'.format(Wf))
    adsp.plot_dynamic_curve(func, gain=1.5)

plt.legend(legend)
plt.title('GRU Distortion with Varying Wf')
plt.savefig('Pics/wf.png', bbox_inches="tight")

plt.figure()
legend = []
for Uf in [0, 2, 4, 6]:
    func = lambda x : gru_block(x, 0.0, 2.0, Uf, 0.0, 0.0)
    legend.append(r'$U_f = {}$'.format(Uf))
    adsp.plot_dynamic_curve(func, gain=1.5)

plt.legend(legend)
plt.title('GRU Distortion with Varying Uf')
plt.savefig('Pics/uf.png', bbox_inches="tight")

# %% [markdown]
# Uh and bf affect the overall width of the central
# part of the distortion curve.

# %%
plt.figure()
legend = []
for Uh in [0, 2, 4]:
    func = lambda x : gru_block(x, 0.0, 2.0, 0.0, Uh, 0.0)
    legend.append(r'$U_h = {}$'.format(Uh))
    adsp.plot_dynamic_curve(func, gain=1.5)

plt.legend(legend)
plt.title('GRU Distortion with Varying Uh')
plt.savefig('Pics/uh.png', bbox_inches="tight")

plt.figure()
legend = []
for bf in [-1, 0, 2]:
    func = lambda x : gru_block(x, 0.0, 2.0, 0.0, 0.0, bf)
    legend.append(r'$b_f = {}$'.format(bf))
    adsp.plot_dynamic_curve(func, gain=1.5)

plt.legend(legend)
plt.title('GRU Distortion with Varying bf')
plt.savefig('Pics/bf.png', bbox_inches="tight")

# %% [markdown]
# As is evidenced by the above plots, the GRU is an
# extremely flexible nonlinearity when the parameters
# are used to their fullest extent.
#
# ## Harmonic Response
#
# As shown in the section above, GRU distortion can be
# very different depending on the parameters of the distortion.
# We show below how various parameters can get different
# types of harmonics responses.

# %%
plt.figure()
func = lambda x : gru_block(x, 0.0, 2.0, 0.0, 0.0, 5.0)
adsp.plot_harmonic_response(func, gain=2)
plt.title('GRU Distortion with Odd Harmonics')
plt.savefig('Pics/odd_harm.png', bbox_inches="tight")

plt.figure()
func = lambda x : gru_block(x, 5.0, 2.0, 2.0, 0.0, 0)
adsp.plot_harmonic_response(func, gain=2)
plt.title('GRU Distortion with Odd and Even Harmonics')
plt.savefig('Pics/all_harm.png', bbox_inches="tight")

# %% [markdown]
# ## Conclusion
#
# As usual, we have implemented the above distortion
# unit as an open-source audio plugin (VST, AU).
# The source code is available on [GitHub](https://github.com/jatinchowdhury18/ComplexNonlinearities),
# and a video demo can be found on [YouTube](https://youtu.be/LPelVYpeP00).

# %%
