# -*- coding: utf-8 -*-
"""
This progamm simulates the heating of honey - at some point

Created on Fri Jun  3 17:42:09 2022

@author: jonat
"""
import numpy as np
import matplotlib.pyplot as plt
import xarray as xr


height = 0.8  # in m
radius = 0.3  # in m
albedo = 0.9
wall_thickness = 0.46 / 100  # in mm
wall_heat_transmissivity = 3
T = 20  # in °C

nx, ny = 10, 10

T_grid = np.zeros([nx, ny, 1]) + T + 273.15 
x_grid = np.zeros([nx, ny]) + np.linspace(0, 2*radius, nx)
y_grid = np.zeros([nx, ny]) + np.linspace(height, 0, ny)
y_grid = y_grid.T

pot_grid = xr.Dataset(data_vars=dict(temperature=(["x", "y", "time"],
                                                  T_grid),
                                      # heat_capacity=(["x", "y", "time"],
                                      #               0),
                                      # is_simulated_grid=(["x", "y"],
                                      #                   True)
                                                          ),
                      coords=dict(dx=(["x", "y"], x_grid),
                                  dy=(["x", "y"], y_grid),
                                  time=0),
                      attrs=dict(description="pot"))

# honey_1 = liquid(20, 3.82, 'solid')  # some internet values


# for i in range(10):
#     honey_1.temperatur()

# hobbock_steel.ShowGrid()
