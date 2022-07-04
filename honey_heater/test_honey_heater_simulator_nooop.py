# -*- coding: utf-8 -*-
"""
This progamm simulates the heating of honey - at some point

Created on Fri Jun  3 17:42:09 2022

@author: jonat
"""
import numpy as np
import xarray as xr

import honey_heater_simulator_nooop as hhsn

# define input parameters
height = 0.8  # in m
radius = 0.3  # in m
albedo = 0.9
wall_thickness = 0.46 / 100  # in mm
wall_heat_transmissivity = 3
T = 20  # in °C


# define grid from inside out
def test_build_grid():
    a_real = {'test': [[2, 2],
                       [2, 2],
                       [2, 2]]}
    a_test = hhsn.build_grid('test', (3, 2), 2)
    # assert type(a_test) == dict
    assert a_real.keys() == a_test.keys()
    np.testing.assert_array_equal(a_real['test'], a_test['test'])


def test_encase_grid():
    a_real = [[2., 2., 2., 2.],
              [2., 2., 2., 2.],
              [2., 0., 0., 2.],
              [2., 0., 0., 2.],
              [2., 2., 2., 2.],
              [2., 2., 2., 2.]]
    a = np.zeros((2,2))
    np.testing.assert_array_equal(hhsn.encase_grid(a, (2, 1), 2), a_real)


# def test_encase_grid(grid, name, add_shape=(2, 2), value=0, *, exclude=None):
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

