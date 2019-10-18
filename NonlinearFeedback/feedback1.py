
#%%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp

#%%
class Biquad:
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

    # Direct-Form II, transposed
    def processSample (self, x):
        y = self.z[1] + self.b[0]*x
        self.z[1] = self.z[2] + self.b[1]*x - self.fb_lambda (self.a[1],y)
        self.z[2] = self.b[2]*x - self.fb_lambda (self.a[2],y)
        return y

    def process_block (self, block):
        for n in range (len (block)):
            block[n] = self.processSample (block[n])
        return block

#%%
class FBProc:
    def __init__(self):
        self.feedback_lambda = lambda x : 0
        self.x1 = 0

    def process_sample (self, x):
        y = self.x1
        self.x1 = x + self.feedback_lambda (y)
        return y

    def process_block (self, block):
        for n in range (len (block)):
            block[n] = self.process_sample (block[n])
        return block


#%%
imp = np.zeros (64); imp[0] = 1
out = np.copy (imp)

proc = FBProc()
proc.feedback_lambda = lambda x : -0.8 * adsp.soft_clipper(x)
out = proc.process_block (out)

plt.plot (imp)
plt.plot (out)

#%%
fs = 44100
sweep = adsp.sweep_log (20, 22000, 4, fs)
y = np.copy (sweep)
y_NL = np.copy (sweep)

procLin = FBProc()
procLin.feedback_lambda = lambda x : 0.5 * (2*x)
y = procLin.process_block (y)

procNL = FBProc()
procNL.feedback_lambda = lambda x : 0.5 * adsp.soft_clipper (2*x)
y_NL = procNL.process_block (y_NL)

#%%
h = adsp.sweep2ir (sweep, y)
h_NL = adsp.sweep2ir (sweep, y_NL)

adsp.plot_magnitude_response (h, [1], fs=fs)
adsp.plot_magnitude_response (h_NL, [1], fs=fs)

#%%
plt.plot (sweep, y_NL)

#%% [markdown]
# $$
# y[n] = x[n] + y[n-1] * a_1
# $$

#%%
def q2damp (freq, Q, fs):
    alpha = np.sin (2 * np.pi * freq / fs) / (2*Q)
    return np.sqrt (1 - alpha) / np.sqrt (1 + alpha)

print (q2damp (1000, 10, 44100))

#%%
def design_allpole (pole_mags, pole_freqs, fs):
    poles = []
    for n in range(len (pole_mags)):
        w_p =  pole_freqs[n] / fs * (2 * np.pi)
        poles.append (pole_mags[n] * np.exp (1j*w_p))
    a = np.poly (np.asarray (poles))
    return np.array([1]), a

mags = np.array ([q2damp (1000, 10, 44100), q2damp (1000, 10, 44100)])
poles = np.array ([1000, -1000])
b, a = design_allpole (mags, poles, 44100)
btest, atest = adsp.design_LPF2 (1000, 10, 44100)

btest = np.array ([1, 0, 0])

roots = np.roots (atest)
print (np.abs (roots[0]))

adsp.plot_magnitude_response (b, a, fs=44100)
adsp.plot_magnitude_response (btest, atest, fs=44100)

plt.axvline (1000, color='r')
        

#%%
class AllPole:
    def __init__(self):
        self.a = np.array([1.0, 0.0, 0.0])
        self.fb_lambda = lambda a,x : a*x
        self.z = np.zeros (3)
    
    def set_coefs(self, a):
        for n in range(3):
            self.a[n] = a[n]

    def process_sample(self, x):
        y = x - self.fb_lambda (self.a[1], self.z[1]) - self.fb_lambda (self.a[2], self.z[2])

        self.z[2] = self.z[1]
        self.z[1] = y
        return y

    def process_block (self, block):
        for n in range (len (block)):
            block[n] = self.process_sample (block[n])
        return block

    

#%%
fs = 44100
sweep = adsp.sweep_log (20, 22000, 2, fs)
y = np.copy (sweep)

mags = np.array ([0.99, 0.99])
poles = np.array ([1000, -1000])
# b, a = design_allpole (mags, poles, fs=fs)

#%%
freqs = np.logspace (1, 3.4, num=1000, base=20)
for g in [0.00001, 0.001, 0.01, 0.1, 0.5, 0.8]:
    cur_sweep = np.copy (sweep) * g
    y = np.copy (cur_sweep)

    filter = Biquad()
    b, a = adsp.design_bell (1000, 0.707, 2, fs)
    filter.setCoefs (b, a)
    # filter.set_coefs (a)
    filter.fb_lambda = lambda a,x : a*np.tanh (x) # adsp.soft_clipper (x)
    y = filter.process_block (y)

    h = adsp.normalize (adsp.sweep2ir (cur_sweep, y))
    adsp.plot_magnitude_response (h, [1], worN=freqs, fs=fs)

# plt.ylim (-15)

#%%
filter = AllPole()
filter.set_coefs (a)
filter.fb_lambda = lambda a,x : a*adsp.soft_clipper (x)
y = filter.process_block (y)

#%%
ylin = np.copy (sweep)

filter_lin = AllPole()
filter_lin.set_coefs (a)
ylin = filter_lin.process_block (ylin)

#%%
h = adsp.normalize (adsp.sweep2ir (sweep, y))
h_lin = adsp.normalize (adsp.sweep2ir (sweep, ylin))
adsp.plot_magnitude_response (h, [1], fs=fs)
adsp.plot_magnitude_response (h_lin, [1], fs=fs)

#%%
