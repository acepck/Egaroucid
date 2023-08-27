import statistics
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from random import random, randrange
import numpy as np
from scipy.optimize import curve_fit
from matplotlib import animation
import math

#data_file = 'data/mid.txt'

#with open(data_file, 'r') as f:
#    raw_data = f.read().splitlines()

data = [[[[] for _ in range(61)] for _ in range(61)] for _ in range(65)] # n_discs, depth1, depth2 (depth1 < depth2)

#for datum in raw_data:
#    n_discs, depth1, depth2, error = [int(elem) for elem in datum.split()]
#    data[n_discs][depth1][depth2].append(error)

w_n_discs = []
x_depth1 = []
y_depth2 = []
z_error = []
weight = []

'''
for n_discs in range(len(data)):
    for depth1 in range(len(data[n_discs])):
        for depth2 in range(len(data[n_discs][depth1])):
            if len(data[n_discs][depth1][depth2]) >= 3:
                mean = statistics.mean(data[n_discs][depth1][depth2])
                sigma = statistics.stdev(data[n_discs][depth1][depth2])
                w_n_discs.append(n_discs)
                x_depth1.append(depth1)
                y_depth2.append(depth2)
                z_error.append(sigma + const_weight)
                weight.append(1 / len(data[n_discs][depth1][depth2]))
'''

for n_discs in range(61):
    for depth2 in range(10, 60):
        depth1 = 0
        w_n_discs.append(n_discs)
        x_depth1.append(depth1)
        y_depth2.append(depth2)
        s = 16.0 + 3.0 * n_discs / 60 #max(10.0, min(21.0, 18.0 + (21.0 - 18.0) * n_discs / 30)) # d2 = 60
        e = 3.0 + 2.0 * n_discs / 60 #max(5.0, min(13.0, 8.0 + (13.0 - 8.0) * n_discs / 30)) # d2 = 0
        z_error.append(e + (s - e) * depth2 / 60)
        weight.append(0.001)

for n_discs in range(61):
    for depth2 in range(10, 60):
        depth1 = depth2 / 2
        w_n_discs.append(n_discs)
        x_depth1.append(depth1)
        y_depth2.append(depth2)
        s = 11.0 + 2.0 * n_discs / 60 #max(10.0, min(18.0, 18.0 + (10.0 - 18.0) * n_discs / 30)) # d2 = 60
        e = 4.0 + 2.0 * n_discs / 60 #max(5.0, min(10.0, 10.0 + (6.0 - 10.0) * n_discs / 30)) # d2 = 0
        z_error.append(e + (s - e) * depth2 / 60)
        weight.append(0.001)

for n_discs in range(61):
    for depth2 in range(15, 60):
        depth1 = depth2
        w_n_discs.append(n_discs)
        x_depth1.append(depth1)
        y_depth2.append(depth2)
        z_error.append(3.5)
        weight.append(0.002)

def f(wxy, probcut_a, probcut_b, probcut_c, probcut_d, probcut_e, probcut_f, probcut_g, probcut_h, probcut_i, probcut_j):
    w, x, y = wxy
    w = w / 64
    x = x / 60
    y = y / 60
    res = probcut_a * w + probcut_b * x + probcut_c * y
    res = probcut_d * res * res * res + probcut_e * res * res + probcut_f * res + probcut_g
    return res

def f_max(wxy, probcut_a, probcut_b, probcut_c, probcut_d, probcut_e, probcut_f, probcut_g, probcut_h, probcut_i, probcut_j):
    return np.minimum(30.0, np.maximum(-2.0, f(wxy, probcut_a, probcut_b, probcut_c, probcut_d, probcut_e, probcut_f, probcut_g, probcut_h, probcut_i, probcut_j)))

def plot_fit_result_onephase(n_discs, params):
    fig = plt.figure()
    #ax = Axes3D(fig)
    ax = fig.add_subplot(111, projection='3d')
    #ax.plot(xs, ys, zs, ms=3, marker="o",linestyle='None')
    #ax.plot(sdxs, sdys, sdzs, ms=3, marker="o",linestyle='None')
    phase = n_discs
    x_depth1_phase = []
    y_depth2_phase = []
    z_error_phase = []
    for w, x, y, z in zip(w_n_discs, x_depth1, y_depth2, z_error):
        if w == phase:
            x_depth1_phase.append(x)
            y_depth2_phase.append(y)
            z_error_phase.append(z)
    ax.plot(x_depth1_phase, y_depth2_phase, z_error_phase, ms=3, marker="o",linestyle='None')
    mx, my = np.meshgrid(range(61), range(61))
    ax.plot_wireframe(mx, my, f_max((n_discs, mx, my), *params), rstride=10, cstride=10)
    ax.set_xlabel('depth1')
    ax.set_ylabel('depth2')
    ax.set_zlabel('error')
    plt.show()

probcut_params_before = [1.0 for _ in range(10)]

probcut_params_old = [-0.001265116404528472, -1.758143972292579, 1.7566279520842052, -1.0938733019995888, 1.1290649413162603, 10.55327508419982, 3.106465908394277, 1, 1, 1]

popt, pcov = curve_fit(f, (w_n_discs, x_depth1, y_depth2), z_error, np.array(probcut_params_before), sigma=weight, absolute_sigma=True)
#popt = probcut_params_before
print([float(elem) for elem in popt])
for i in range(len(popt)):
    print('#define probcut_' + chr(ord('a') + i), popt[i])

for i in [10, 30, 50]:
    plot_fit_result_onephase(i, popt)
    #plot_fit_result_onephase(i, probcut_params_old)
