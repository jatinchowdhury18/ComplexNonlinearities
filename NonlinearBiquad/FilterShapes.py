import numpy as np

def calcCoefsBell (fc, Q, gain, fs):
    """Calculates filter coefficients for a bell filter.

    Parameters
    ----------
    fc : float
        Center frequency of the filter in Hz
    Q : float
        Quality factor of the filter
    gain : float
        Linear gain for the center frequency of the filter
    fs :  float
        Sample rate in Hz

    Returns
    -------
    b : ndarray
        "b" (feedforward) coefficients of the filter
    a : ndarray
        "a" (feedback) coefficients of the filter
    """
    wc = 2 * np.pi * fc / fs
    c = 1.0 / np.tan (wc / 2.0)
    phi = c*c
    Knum = c / Q
    Kdenom = Knum

    if (gain > 1.0):
        Knum *= gain
    elif (gain < 1.0):
        Kdenom /= gain

    a0 = phi + Kdenom + 1.0

    b = [(phi + Knum + 1.0) / a0, 2.0 * (1.0 - phi) / a0, (phi - Knum + 1.0) / a0]
    a = [1, 2.0 * (1.0 - phi) / a0, (phi - Kdenom + 1.0) / a0]

    return np.asarray (b), np.asarray(a)

def calcCoefsLPF2 (fc, Q,  fs):
    """Calculates filter coefficients for a 2nd-order lowpass filter

    Parameters
    ----------
    fc : float
        Cutoff frequency of the filter in Hz
    Q : float
        Quality factor of the filter
    fs : float
        Sample rate in Hz

    Returns
    -------
    b : ndarray
        "b" (feedforward) coefficients of the filter
    a : ndarray
        "a" (feedback) coefficients of the filter
    """
    wc = 2 * np.pi * fc / fs
    c = 1.0 / np.tan (wc / 2.0)
    phi = c*c
    K = c / Q
    a0 = phi + K + 1.0

    b = [1 / a0, 2.0 / a0, 1.0 / a0]
    a = [1, 2.0 * (1.0 - phi) / a0, (phi - K + 1.0) / a0]

    return np.asarray (b), np.asarray(a)

def calcCoefsHPF2 (fc, Q,  fs):
    """Calculates filter coefficients for a 2nd-order highpass filter

    Parameters
    ----------
    fc : float
        Cutoff frequency of the filter in Hz
    Q : float
        Quality factor of the filter
    fs : float
        Sample rate in Hz

    Returns
    -------
    b : ndarray
        "b" (feedforward) coefficients of the filter
    a : ndarray
        "a" (feedback) coefficients of the filter
    """
    wc = 2 * np.pi * fc / fs
    c = 1.0 / np.tan (wc / 2.0)
    phi = c*c
    K = c / Q
    a0 = phi + K + 1.0

    b = [phi / a0, -2.0 * phi / a0, phi / a0]
    a = [1, 2.0 * (1.0 - phi) / a0, (phi - K + 1.0) / a0]
    return np.asarray (b), np.asarray(a)

def calcCoefsNotch (fc, Q, fs):
    """Calculates filter coefficients for a notch filter.

    Parameters
    ----------
    fc : float
        Center frequency of the filter in Hz
    Q : float
        Quality factor of the filter
    fs :  float
        Sample rate in Hz

    Returns
    -------
    b : ndarray
        "b" (feedforward) coefficients of the filter
    a : ndarray
        "a" (feedback) coefficients of the filter
    """
    wc = 2 * np.pi * fc / fs
    wS = np.sin (wc)
    wC = np.cos (wc)
    alpha = wS / (2.0 * Q)

    a0 = 1.0 + alpha

    b = [1.0 / a0, -2.0 * wC / a0, 1.0 / a0]
    a = [1, -2.0 * wC / a0, (1.0 - alpha) / a0]
    return np.asarray (b), np.asarray(a)

def calcCoefsHighShelf (fc, Q, gain, fs):
    """Calculates filter coefficients for a High Shelf filter.

    Parameters
    ----------
    fc : float
        Center frequency of the filter in Hz
    Q : float
        Quality factor of the filter
    gain : float
        Linear gain for the shelved frequencies
    fs :  float
        Sample rate in Hz

    Returns
    -------
    b : ndarray
        "b" (feedforward) coefficients of the filter
    a : ndarray
        "a" (feedback) coefficients of the filter
    """
    A = np.sqrt (gain)
    wc = 2 * np.pi * fc / fs
    wS = np.sin (wc)
    wC = np.cos (wc)
    beta = np.sqrt (A) / Q

    a0 = ((A+1.0) - ((A-1.0) * wC) + (beta*wS))

    b = np.zeros (3)
    a = np.zeros (3)
    b[0] = A*((A+1.0) + ((A-1.0)*wC) + (beta*wS)) / a0
    b[1] = -2.0*A * ((A-1.0) + ((A+1.0)*wC)) / a0
    b[2] = A*((A+1.0) + ((A-1.0)*wC) - (beta*wS)) / a0

    a[0] = 1
    a[1] = 2.0 * ((A-1.0) - ((A+1.0)*wC)) / a0
    a[2] = ((A+1.0) - ((A-1.0)*wC)-(beta*wS)) / a0
    return b, a

def calcCoefsLowShelf (fc, Q, gain, fs):
    """Calculates filter coefficients for a Low Shelf filter.

    Parameters
    ----------
    fc : float
        Center frequency of the filter in Hz
    Q : float
        Quality factor of the filter
    gain : float
        Linear gain for the shelved frequencies
    fs :  float
        Sample rate in Hz

    Returns
    -------
    b : ndarray
        "b" (feedforward) coefficients of the filter
    a : ndarray
        "a" (feedback) coefficients of the filter
    """
    A = np.sqrt (gain)
    wc = 2 * np.pi * fc / fs
    wS = np.sin (wc)
    wC = np.cos (wc)
    beta = np.sqrt (A) / Q

    a0 = ((A+1.0) + ((A-1.0) * wC) + (beta*wS))

    b = np.zeros (3)
    a = np.zeros (3)
    b[0] = A*((A+1.0) - ((A-1.0)*wC) + (beta*wS)) / a0
    b[1] = 2.0*A * ((A-1.0) - ((A+1.0)*wC)) / a0
    b[2] = A*((A+1.0) - ((A-1.0)*wC) - (beta*wS)) / a0

    a[0] = 1
    a[1] = -2.0 * ((A-1.0) + ((A+1.0)*wC)) / a0
    a[2] = ((A+1.0) + ((A-1.0)*wC)-(beta*wS)) / a0
    return b, a
