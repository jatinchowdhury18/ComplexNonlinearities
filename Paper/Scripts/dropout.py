# %%
import numpy as np
import audio_dspy as adsp
import scipy.signal as signal
import matplotlib.pyplot as plt
plt.style.use('dark_background')

# %%
def dropout(x, a):
    if isinstance (x, np.ndarray):
        t = np.copy (x)
        for n in range (len (x)):
            t[n] = dropout (t[n], a)
        return t

    B = np.sqrt(a**3 / 3)
    if x > B:
        return x - B + (B/a)**3
    if x < -B:
        return x + B - (B/a)**3
    return (x/a)**3

adsp.plot_static_curve(lambda x : dropout(x, 0.6), gain=2)
plt.title('Static Curve for Dropout Nonlinearity')
# %%
