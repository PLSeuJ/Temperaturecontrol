# -*- coding: utf-8 -*-
"""
This progamm simulates the heating of honey - at some point

build the numerical grid from the inside out,
set the boundary conditions afterwards and
apply the numerical method than.

Use the inbuild plotting function to visualise your results.

Created on Fri Jun  3 17:42:09 2022

@author: jonat
"""
import numpy as np
import math
import matplotlib.pyplot as plt
import xarray as xr

# some constats
T_zero = 20 + 273.15


# number of gridpoints in directions [x, y]
n_honey = [10, 20]
n_container = [5, 5]
n_boundary = [1, 1]

# distances in medium [meter]
d_honey = [0.4, 0.8]
d_container = [2 * 10**(-5), 2 * 10**(-5)]
d_boundary = [10**(-3), 10**(-3)]

n = []
for i in range(2):
    # sum of gridpoints
    n.append(n_boundary[i] +
             n_container[i] +
             n_honey[i] +
             n_container[i] +
             n_boundary[i])


# define masks
honey_mask = np.zeros(n, dtype=bool)
# TODO make the numbers in mask calcualted by the computer instead of human
honey_mask[6:16, 6:26] = True
boundary_mask = np.ones(n, dtype=bool)
boundary_mask[n_boundary[0]:-n_boundary[0],
              n_boundary[1]:-n_boundary[1]] = False
container_mask = np.zeros(n, dtype=bool)
container_mask = np.where(honey_mask, False, True)
container_mask = np.where(boundary_mask, False, container_mask)


# define positions
dx, dy = np.zeros(n[1]), np.zeros(n[0])  # dx varies with y and vice versa

for i in range(len(dx)):
    dx[i] = np.where(honey_mask[int(n[0]/2), i],
                     d_honey[1]/n_honey[1], dx[i])
    dx[i] = np.where(container_mask[int(n[0]/2), i],
                     d_container[1]/n_container[1], dx[i])
    dx[i] = np.where(boundary_mask[int(n[0]/2), i],
                     d_boundary[1]/n_boundary[1], dx[i])

for i in range(len(dy)):
    dy[i] = np.where(honey_mask[i, int(n[1]/2)],
                     d_honey[0]/n_honey[0], dy[i])
    dy[i] = np.where(container_mask[i, int(n[1]/2)],
                     d_container[0]/n_container[0], dy[i])
    dy[i] = np.where(boundary_mask[i, int(n[1]/2)],
                     d_boundary[0]/n_boundary[0], dy[i])

x_grid, y_grid = np.zeros(n), np.zeros(n)
for i in range(1, n[1]):
    y_grid[:, i] = y_grid[:, i-1] + dx[i]
for j in range(1, n[0]):
    x_grid[j, :] = x_grid[j-1, :] + dy[j]


# define Volumes for weightening
r = np.empty((n[0]-1, n[1]))
for i in range(n[0]-1):
    r[i, :] = abs((x_grid[i, :] + x_grid[i+1, :]) - (2 * x_grid[-1, :]))

V_grid = np.empty(n)

V_grid[0, :] = x_grid[-1, :] * (1 - math.pi / 4)
V_grid[-1, :] = x_grid[-1, :] * (1 - math.pi / 4)

for i in range(n[0]-2):
    V_grid[i+1, :] = math.pi/2 * abs(r[i, :]**2 - r[i+1, :]**2)


# define other grids
T = np.ones(n) * T_zero


# numerical scheme


# plot the thing
fig = plt.figure(1, figsize=(6, 7))
plt.contour(x_grid, y_grid, container_mask, [-1, 1, 2],
            cmap='gray_r', vmin=0, vmax=5)
plt.contourf(x_grid, y_grid, T)


plt.scatter(x_grid, y_grid, T, alpha=0.2)
