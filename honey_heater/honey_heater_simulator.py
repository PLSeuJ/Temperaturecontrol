# -*- coding: utf-8 -*-
"""
This progamm simulates the heating of honey - at some point

Created on Fri Jun  3 17:42:09 2022

@author: jonat
"""
import numpy as np

class pot:
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

        dx, dy = self.build(wall_thickness, height, nx, ny)

    def temperatur(self):
        return self.T - 273.15

    def build(self, wall_thickness, height, nx, ny):
        dx = np.linspace(self.radius - self.wall_thickness, self.radius, nx)
        dy = np.linspace(self.height - self.wall_thickness, self.height, ny)
        return dx, dy


class liquid:
    def __init__(self, T, heat_capacity, state):
        self.T = T + 273.15
        self.heat_capacity = heat_capacity
        self.state = state  # ignored for the moment

    def temperatur(self):
        return self.T - 273.15


hobbock_steel = pot(height=0.8,  # in m
                    radius=0.3,  # in m
                    albedo=0.9,
                    wall_thickness=0.46,  # in mm
                    wall_heat_transmissivity=3,
                    T=20)  # in °C

honey_1 = liquid(20, 3.82, 'solid')  # some internet values


for i in range(10):
    print(f'{honey_1.temperatur()} °C')
