# -*- coding: utf-8 -*-
"""
This progamm simulates the heating of honey - at some point

Created on Fri Jun  3 17:42:09 2022

@author: jonat
"""
import numpy as np
import matplotlib.pyplot as plt
import xarray as xr


class grided_numerics:
    def temperatur(self):
        # calculate average here in example
        return print(f'{self.T - 273.15} °C')


class pot(grided_numerics):
    def __init__(self,
                 height,  # outside in m
                 radius,  # outside in m
                 albedo,
                 wall_thickness,  # in mm
                 wall_heat_transmissivity,
                 T,  # in °C
                 nx=10,
                 ny=10,  # simulated points in wall
                 ):
        self.height = height
        self.radius = radius
        self.albedo = albedo
        self.emissivity = albedo
        self.wall_thickness = wall_thickness * 100
        self.wall_heat_transmissivity = wall_heat_transmissivity
        self.T = T + 273.15  # now Temperature is in K
        self.nx = nx
        self.ny = ny
        #  forgot anything?

        self.grid = self.build(wall_thickness,
                               height,
                               T,
                               wall_heat_transmissivity,
                               nx,
                               ny)
        # self.Tgrid = T

    def build(self, wall_thickness, height, T, heat_capacity, nx, ny):
        dx_wall = np.linspace(self.radius + self.wall_thickness, self.radius, nx)
        dx = np.concatenate(dx_wall, np.linspace(self.radius, 0, nx))
        dy = np.linspace(self.height - self.wall_thickness, self.height, ny)

        T_grid = np.zeros([nx, ny, 1]) + T
        dx_grid = np.zeros([nx, ny]) + dx
        dy_grid = np.zeros([nx, ny]) + [dy]

        pot_grid = xr.Dataset(data_vars=dict(temperature=(["x", "y", "time"],
                                                          T_grid),
                                              # heat_capacity=(["x", "y", "time"],
                                              #               0),
                                              # is_simulated_grid=(["x", "y"],
                                              #                   True)
                                                                  ),
                              coords=dict(x=(["x", "y"], dx),
                                          y=(["x", "y"], dy),
                                          time=0),
                              attrs=dict(description="pot"))
        return pot_grid

    def ShowGrid(self):
        plt.scatter(self.dx, self.dy, self.Tgrid)
        plt.show()


class liquid(grided_numerics):
    def __init__(self, T, heat_capacity, state):
        self.T = T + 273.15
        self.heat_capacity = heat_capacity
        self.state = state  # ignored for the moment


hobbock_steel = pot(height=0.8,  # in m
                    radius=0.3,  # in m
                    albedo=0.9,
                    wall_thickness=0.46,  # in mm
                    wall_heat_transmissivity=3,
                    T=20)  # in °C

honey_1 = liquid(20, 3.82, 'solid')  # some internet values


# for i in range(10):
#     honey_1.temperatur()

# hobbock_steel.ShowGrid()
