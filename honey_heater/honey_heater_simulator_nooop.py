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
# import xarray as xr


# define grid from inside out
def build_grid(name, shape=(10, 10), value=0):
    """
    test:
    define dictionary item containing grid with specified initial value

    Input:
        grid        dict of grid
        name        sting
        add_shape   integer tupel (x[, y, z, ...])
                        must match shape of old grid or have one value,
                        added to all sides unless excluded.
        value       flt or int
        exclude     sides to exclude when adding gridpoints
    Output:
        dict(name: new grid)
    """
    grid = np.zeros(shape[::-1])
    grid += value
    return {name: grid}


# def encase_grid(grid, name, add_shape=(2, 2), value=0, * , exclude=None):
#     """
#     test:
#     add gridpoints around your existing grid and return the entire thing

#     Input:
#         grid        dict of grid
#         name        sting
#         add_shape   integer tupel (x[, y, z, ...])
#                         must match shape of old grid or have one value,
#                         added to all sides unless excluded.
#         value       flt or int
#         exclude     sides to exclude when adding gridpoints
#                     use 'left', 'right', 'top', 'bottom' individualy or
#                     as a tupel. Default = None

#     Output:
#         dict(name: new grid)
#     """
#     grid =


def main():
    # define the numerical grid for honey
    shape = (10, 10)
    T = 20  # in °C
    honey = build_grid('Temperature', shape, T+273.15)

    # define input parameters
    height = 0.8  # in m
    radius = 0.3  # in m
    albedo = 0.9
    wall_thickness = 0.46 / 100  # in mm
    wall_heat_transmissivity = 3

if __name__ == 'main':
    main()
