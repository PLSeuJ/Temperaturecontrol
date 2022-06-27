# -*- coding: utf-8 -*-
"""
This progamm simulates the heating of honey - at some point

Created on Fri Jun  3 17:42:09 2022

@author: jonat
"""
import numpy as np
import matplotlib.pyplot as plt
from math import pi


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

        self.Tgrid, self.dx, self.dy = self.build(wall_thickness,
                                                  height,
                                                  nx,
                                                  ny)
        self.Tgrid = T

    def build(self, wall_thickness, height, nx, ny):
        dx = np.linspace(self.radius - self.wall_thickness, self.radius, nx)
        dy = np.linspace(self.height - self.wall_thickness, self.height, ny)
        Tgrid = np.zeros(nx, ny) + self.T
        dxgrid = np.zeros(nx, ny) + dx
        dygrid = np.zeros(ny, nx) + dy
        dygrid = dygrid.T

        dx_weight = 0.5 * (np.roll(dxgrid, 1) - np.roll(dxgrid, -1))
        dx_weight[(0, -1)] = 0.5 * dx_weight[(1, -2)]

        dy_weight = 0.5 * (np.roll(dygrid, 1) - np.roll(dygrid, -1))
        dy_weight[(0, -1)] = 0.5 * dy_weight[(1, -2)]

        dA_grid = (np.zeros(nx, ny) + dx_weight * dx_grid) * pi/2
        dV_grig = dA_grid * dy_weight
        return Tgrid, dx, dy

    def ShowGrid(self):
        plt.scatter(self.dxgrid, self.dygrid)
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


for i in range(10):
    honey_1.temperatur()

hobbock_steel.ShowGrid()
