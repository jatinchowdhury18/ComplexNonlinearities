import numpy as np
import matplotlib.pyplot as plt

#M_r = 0.5e6 (A/m)
#H_c (coercivity) = 25000 - 30000 A/m (Jiles book)

# Langevin with AA
def L_AA (x, x_1):
    if (abs (x - x_1) > 10 ** -2):
        return (L_I (x) - L_I (x_1)) / (x - x_1)
    else:
        return L (0.5 * x + 0.5 * x_1)

# Langevin derivative with AA
def L_d_AA (x, x_1):
    if (abs (x - x_1) > 10 ** -2):
        return (L (x) - L (x_1)) / (x - x_1)
    else:
        return L_d (0.5 * x + 0.5 * x_1)

# Langevin function
def L (x):
    if (abs (x) > 10 ** -4):
        return (1 / np.tanh (x)) - (1/x)
    else:
        return (x / 3)

# Langevin derivative
def L_d (x):
    if (abs(x) > 10 ** -4):
        return (1 / x ** 2) - (1 / np.tanh (x)) ** 2 + 1
    else:
        return (1 / 3)

# Langevin Integral
def L_I (x):
    if (np.abs (x) > 10 ** -4):
        return np.log (np.sinh (np.abs (x))) - np.log (np.abs (x))
    else:
        return x**2 / 6

# trapezoidal rule derivative
def deriv (x_n, x_n1, xDeriv_n1, T):
    return ((2 / T) * (x_n - x_n1)) - xDeriv_n1

# dM/dt or "non-linear function"
def f (M, H, H_d, M_s, a, alpha, k, c, Q_1):
    Q = (H + alpha * M) / a
    M_diff = M_s * L_AA (Q, Q_1) - M
    delta = 1 if H_d > 0 else -1
    delta_M = 1 if np.sign (delta) == np.sign (M_diff) else 0
    L_prime = L_d_AA (Q,  Q_1)

    denominator = 1 - c * alpha * (M_s / a) * L_prime

    t1_num = (1 - c) * delta_M * M_diff
    t1_den = (1 - c) * delta * k - alpha * M_diff
    t1 = (t1_num / t1_den) * H_d

    t2 = c * (M_s / a) * H_d * L_prime

    return (t1 + t2) / denominator

def M_n (M_n1, k1, k2, k3, k4):
    return M_n1 + (k1 / 6) + (k2 / 3) + (k3 / 3) + (k4 / 6)

def Hysteresis_Process (H_in, M_s, a, alpha, k, c, T):
    print ("Test8")
    M_out = np.zeros (len (H_in))
    M_n1 = 0
    H_n1 = 0
    H_d_n1 = 0

    n = 0
    Q_1 = 0
    Q_1_2 = 0
    for H in H_in:
        H_d = deriv (H, H_n1, H_d_n1, 1.5 * T)

        k1 = 1.5 * T * f (M_n1, H_n1, H_d_n1, M_s, a, alpha, k, c, Q_1)
        k2 = 1.5 * T * f (M_n1 + k1/2, (H + H_n1) / 2, (H_d + H_d_n1) / 2, M_s, a, alpha, k, c, Q_1_2)
        # k3 = T * f (M_n1 + k2/2, (H + H_n1) / 2, (H_d + H_d_n1) / 2, M_s, a, alpha, k, c)
        # k4 = T * f (M_n1 + k3, H, H_d, M_s, a, alpha, k, c)

        Q_1 = (H_n1 + alpha * M_n1) / a
        Q_1_2 = ((H + H_n1) / 2 + alpha * (M_n1 + k1/2)) / a

        M = M_n1 + k2 # M_n (M_n1, k1, k2, k3, k4)

        M_n1 = M
        H_n1 = H
        H_d_n1 = H_d

        M_out[n] = M
        n += 1

    return M_out

# fs = 48000*4
# T = 1/fs # sample interval
# f_test = 800
# n = np.arange (100000)
# gain = 1e4
# M_s = gain*50 # saturation
# a = M_s / 20 #adjustable parameter
# alpha = 1.6e-3
# k = 27.0e3 #Coercivity
# c = 1.7e-1 #1.867e-1

# x = gain*np.sin (2 * np.pi * n * f_test / fs)
# y = Hysteresis_Process (x, M_s, a, alpha, k, c, T)
# y2 = Hysteresis_Process (x, M_s, a, alpha, k, c / 100, T)

# plt.plot (x[100:10000], y[100:10000])
# plt.plot (x[100:10000], y2[100:10000])
# plt.show()
