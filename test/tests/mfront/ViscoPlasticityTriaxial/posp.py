###############################################
def setupMatplotlib(height=8.,width=8.):
    plt.rcParams['xtick.direction']='out'
    plt.rcParams['ytick.direction']='out'
    plt.rcParams['lines.linewidth']= 1.0
    plt.rcParams['lines.color']= 'black'
    plt.rcParams['legend.frameon']=False
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['legend.fontsize']=10
    plt.rcParams['font.size'] = 10
    plt.rcParams['lines.markersize'] = 1
    plt.rcParams['figure.figsize'] = (height,width)
    plt.rcParams['grid.linewidth'] = 0.5
    grid_line_color = '0.5'
    plt.rcParams['grid.color'] = grid_line_color
    plt.rcParams['grid.linestyle'] = '-'

def create_color_map(str_cl,n_col):
    cmap = matplotlib.cm.get_cmap(str_cl)
    max_col=n_col
    rgba=[cmap(0)]*max_col
    for i in range(max_col):
        rgba[i] = cmap(float(i)/max_col)
    return rgba

def commonFormat(ax_el):
    ax_el.spines['top'].set_linewidth(0.5)
    ax_el.spines['right'].set_linewidth(0.5)
    ax_el.spines['bottom'].set_linewidth(0.5)
    ax_el.spines['left'].set_linewidth(0.5)
    ax_el.xaxis.set_ticks_position('bottom')
    ax_el.yaxis.set_ticks_position('left')

def commonFormat_two_axis_primary(ax_el):
    ax_el.spines['top'].set_linewidth(0)
    ax_el.spines['right'].set_linewidth(0)
    ax_el.spines['bottom'].set_linewidth(0.5)
    ax_el.spines['left'].set_linewidth(0.5)
    ax_el.xaxis.set_ticks_position('bottom')
    ax_el.yaxis.set_ticks_position('left')

def commonFormat_two_axis_secondary(ax_el):
    ax_el.spines['top'].set_linewidth(0.5)
    ax_el.spines['right'].set_linewidth(0.5)
    ax_el.spines['bottom'].set_linewidth(0)
    ax_el.spines['left'].set_linewidth(0)
    ax_el.xaxis.set_ticks_position('top')
    ax_el.yaxis.set_ticks_position('right')

def find_nearest_index(array, value):
    array = np.asarray(array)
    idx = (np.abs(array - value)).argmin()
    return idx

import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import pandas as pd
import glob
import re
import os
import matplotlib
import math
#import CoolProp.CoolProp as CP

from mpl_toolkits.mplot3d import Axes3D
#from scipy.stats import gaussian_kde
from decimal import Decimal
#from scipy.optimize import curve_fit
#from scipy.optimize import least_squares
#from scipy.optimize import leastsq
#from scipy import stats
#from scipy.interpolate import interp1d
#from scipy import interpolate
from matplotlib import rc
#from scipy.stats import linregress
#from scipy import special
import  fileinput
import copy
#from scipy.stats import linregress
from matplotlib import rc

#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
#rc('text', usetex=True)

plt.close('all')
scale_graph=0.7
setupMatplotlib(10*scale_graph,8*scale_graph)
ax1 = plt.subplot2grid((1,1), (0,0), colspan=1)

TX_1_slow = pd.read_csv('mfront_viscoplasticity_TX_1MPa_slow_out.dat',sep="\s+",skiprows=1,names=['time','Eeqv','Eeqv_p','Eeqv_v','S'])
TX_1 = pd.read_csv('mfront_viscoplasticity_TX_1MPa_out.dat',sep="\s+",skiprows=1,names=['time','Eeqv','Eeqv_p','Eeqv_v','S'])
TX_3 = pd.read_csv('mfront_viscoplasticity_TX_3MPa_out.dat',sep="\s+",skiprows=1,names=['time','Eeqv','Eeqv_p','Eeqv_v','S'])
TX_10 = pd.read_csv('mfront_viscoplasticity_TX_10MPa_out.dat',sep="\s+",skiprows=1,names=['time','Eeqv','Eeqv_p','Eeqv_v','S'])

lw=1
mks=1
ax1.plot(TX_1.Eeqv,TX_1.S/1e6,marker='',label=r'Creep',linestyle='-',linewidth=lw,markevery=10,markersize=mks,color='k')

ax1.plot(TX_3.Eeqv,TX_3.S/1e6,'b-')
ax1.plot(TX_10.Eeqv,TX_10.S/1e6,'g-')
ax1.plot(TX_1_slow.Eeqv,TX_1_slow.S/1e6,'r-')
plt.show()

exit()

sg=moose_1.saturation_gas[1:-1]
sw=moose_1.saturation_water[1:-1]
rg=moose_1.density_gas[1:-1]
rw=moose_1.density_water[1:-1]
temperature = moose_1.temperature[1:-1]
pressure = moose_1.pressure_gas[1:-1]

r = rg*sg+rw*sw
z = (rg*sg)/(rw*sw)
x = 1-(z*r-sw*rw)/(sg*rg-sw*rw)
plt.plot(temperature-273.15,r,'k-')

AS = CP.AbstractState("HelmholtzEOSMixtureBackend", "Water&CarbonDioxide") # HelmholtzEOSMixtureBackend, PengRobinsonBackend
molar_mass = np.zeros(55)
density = np.zeros(55)
temperature_exp = np.zeros(55)

for i in range(1,55):
	AS.set_mole_fractions([1- x[i], x[i]])
	AS.update(CP.PT_INPUTS,pressure[i],temperature[i])
	molar_mass[i] = AS.molar_mass()
	density[i]=AS.rhomass()
	temperature_exp[i]=temperature[i]

plt.plot(temperature_exp[1:55]-273.15,density[1:55],'r--')

plt.show()