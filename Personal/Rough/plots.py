import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, filtfilt

# Define parameters
fs = 100  # Sampling frequency (Hz)
t = np.arange(0, 2, 1/fs)  # Time vector from 0 to 2 seconds
fc = 25  # Carrier frequency (Hz)

# Generate message signal s(t)
s_t = np.cos(6 * np.pi * t)

# Generate x(t) before nonlinearity
x_t = s_t + 5 * np.cos(2 * np.pi * fc * t)

# Apply nonlinearity y(t) = x(t) + 0.2 * x^2(t)
y_t = x_t + 0.2 * x_t**2

# Design a bandpass filter to extract AM signal around fc = 25 Hz
lowcut = 22  # Lower cutoff frequency (Hz)
highcut = 28  # Upper cutoff frequency (Hz)
order = 6

# Butterworth bandpass filter function
def butter_bandpass(lowcut, highcut, fs, order=6):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

# Apply the bandpass filter
b, a = butter_bandpass(lowcut, highcut, fs, order=order)
z_t = filtfilt(b, a, y_t)

# Plot the trapezoidal modulation plot (s(t), z(t))
plt.figure(figsize=(6, 6))
plt.plot(s_t, z_t, 'b', alpha=0.6)
plt.xlabel("s(t)")
plt.ylabel("z(t)")
plt.title("Trapezoidal Modulation Plot")
plt.grid(True)
plt.show()
