
#%%
import numpy as np
import matplotlib.pyplot as plt
import audio_dspy as adsp

#%%
from  matplotlib import patches

def zplane (p, z):
    ax = plt.subplot(111)
    uc = patches.Circle((0,0), radius=1, fill=False,
                        color='white', ls='dashed')
    ax.add_patch(uc)

    ax.spines['left'].set_position('center')
    ax.spines['bottom'].set_position('center')
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    
    t1 = plt.plot(z.real, z.imag, 'go', ms=10)
    t2 = plt.plot(p.real, p.imag, 'rx', ms=10)

    r = 1.5; plt.axis('scaled'); plt.axis([-r, r, -r, r])
    ticks = [-1, -.5, .5, 1]; plt.xticks(ticks); plt.yticks(ticks)

def getPoles (b, a, g=1):
    return np.roots ([1,  g * a[1], g * a[2]])

def getZeros (b, a, g=1):
    return np.roots ([b[0], b[1], b[2]])

fs = 44100
b, a = adsp.design_LPF2 (1000, 10, fs)

for g in [1, 0.8, 0.6, 0.4, 0.2]:
    plt.figure()
    zplane (getPoles (b, a, g=g), getZeros (b, a, g=g))
    plt.title ('Poles/Zeros for g={}'.format (g))

#%%
