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
        add_shape   integer tupel ([..., z, y,] x)
                        must match shape of old grid or have one value,
                        added to all sides unless excluded.
        value       flt or int
        exclude     sides to exclude when adding gridpoints
    Output:
        dict(name: new grid)
    """
    grid = np.empty(shape,
                    dtype=type(value) if type(value) == bool else float)
    grid[:] = value
    return {name: grid}


def encase_grid(griddict, name, add_shape=(2, 2), value=0, *, exclude=None):
    """
    test:
    add gridpoints around your existing grid and return the entire thing

    Input:
        griddict    dict of grid
        add_shape   integer tupel ([..., z, y,] x)
                        must match shape of old grid or have one value,
                        added to all sides unless excluded.
        value       flt or int
        exclude     sides to exclude when adding gridpoints
                    use 'left', 'right', 'top', 'bottom' individualy or
                    as a tupel.
                    Default = None.
                    Only tested for 2d cases.

    Output:
        dict(name: new grid)
    """

    def get_side(side, grid, value):
        if side in ('left', 'right'):
            shape = (grid.shape[0], add_shape(1))
        if side in ('top', 'bottom'):
            shape = (add_shape(0), grid.shape[1])

        # make array and fill it with desired value
        addgrid = np.empty(shape,
                           dtype=type(value) if type(value) == bool else float)
        addgrid[:] = value
        return addgrid

    def add_side(grid, addgrid, side_name):
        if side == 'left': return np.concatenate((addgrid, grid), axis=1)
        if side == 'right': return np.concatenate((grid, addgrid), axis=1)
        if side == 'top': return np.concatenate((addgrid, grid), axis=0)
        if side == 'bottom': return np.concatenate((grid, addgrid), axis=0)

    # Do the thing
    grid = griddict[name]
    # add all sides individually to make it easly to exclude specified
    for side in ('left', 'right', 'top', 'bottom'):
        if side in exclude:
            continue
        grid = add_side(grid, get_side(), side)
    return grid


# def main():
    # honey = get_honey()
    # grid = put_in_pot(honey)
    # set_boundarys()

    # run_model()

    # plot_result()  # is honey liquid and maxtemperature was never reached?
    # plot_Temperaturedevelopement()


# define the numerical grid for honey
def get_honey(shape=(10, 10),
              T=20,  # in °C
              ):
    """
    sets up honey as numerical grid
    returns dictonary of grids
    """

    # undergoing change in time
    # physical parameters that (might) undergo change in numerical model.
    honey = build_grid('Temperature', shape, T+273.15)
    honey.update(build_grid('heat capacity', shape, 0))
    honey.update(build_grid('heat conductivity', shape, 0))
    honey.update(build_grid('is_liquid', shape, False))  # physical state of honey

    # constant stuff
    honey.update(build_grid('is_honey', shape, True))
    # to check for spoiled honey:
    honey.update(build_grid('max_Temperature', shape, T+273.15))

    # positional information for grid allocation in plotting
    honey.update(build_grid('x', shape, 0))
    honey.update(build_grid('y', shape, 0))


# define input parameters
height = 0.8  # in m
radius = 0.3  # in m
wall_thickness = 0.46 / 100  # in mm
wall_heat_transmissivity = 3


# if __name__ == 'main':
#     a = main()
#     print(a)
