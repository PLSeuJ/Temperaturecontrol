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
import matplotlib.pyplot as plt
import xarray as xr

# number of gridpoints in directions [y, x]
n_honey = [10, 10]
n_container = [5, 5]
n_boundary = [1, 1]

# distances in medium
d_honey = [0.8, 0.4]
d_container = [2 * 10**(-5), 2 * 10**(-5)]
d_boundary = [10**(-3), 10**(-3)]

n, dd = [], []
for i in range(2):
    # sum of gridpoints
    n.append(n_boundary[i] +
             n_container[i] +
             n_honey[i] +
             n_container[i] +
             n_boundary[i])

    # distance step in medium
    dd.append([d_boundary[i]/n_boundary[i],
               d_container[i]/n_container[i],
               d_honey[i]/n_honey[i],
               d_container[i]/n_container[i],
               d_boundary[i]/n_boundary[i]])

# define masks
honey_mask = np.zeros(n, dtype=bool)
boundary_mask = np.ones(n, dtype=bool)
container_mask = np.zeros(n, dtype=bool)
# TODO make the numbers of masks calcualted by the computer instead of human
honey_mask[6:16, 6:16] = True
boundary_mask[n_boundary[0]:-n_boundary[0],
              n_boundary[1]:-n_boundary[1]] = False
container_mask = np.where(honey_mask, False, True)
container_mask = np.where(boundary_mask, False, container_mask)

# define positions
dx_grid, dy_grid = np.zeros(n), np.zeros(n)

dx_grid = np.where(honey_mask, d_honey[0]/n_honey[0], dx_grid)
dx_grid = np.where(container_mask, d_container[0]/n_container[0], dx_grid)
dx_grid = np.where(boundary_mask, d_boundary[0]/n_boundary[0], dx_grid)

dy_grid = np.where(honey_mask, d_honey[0]/n_honey[0], dy_grid)
dy_grid = np.where(container_mask, d_container[0]/n_container[0], dy_grid)
dy_grid = np.where(boundary_mask, d_boundary[0]/n_boundary[0], dy_grid)

x_grid, y_grid = np.zeros(n), np.zeros(n)

for i in range(n[0]):
    dx_grid[i, :] = max(dx_grid[i, 1:-1])
for j in range(n[1]):
    dy_grid[:, j] = max(dy_grid[1:-1, j])


for i in range(n[0]-1):
    x_grid[i+1, :] += dx_grid[i, :]
for j in range(n[1]):
    dy_grid[:, j] = max(dy_grid[1:-1, j])


x_grid[:] += dx_grid[:]



# def main():
#     honey = build()
#     grid = put_in_pot(honey)
#     set_boundarys()

#     run_model()

#     plot_result()  # is honey liquid and maxtemperature was never reached?
#     plot_Temperaturedevelopement()

# if __name__ == 'main':
#     a = main()
#     print(a)
