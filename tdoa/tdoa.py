# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
from numpy import linalg
import scipy.io as sio
import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial.transform import Rotation as R
import matplotlib as mpl


def hyperbola(a,b,c,t):
    x1 = a*np.cosh(t)
    y1 = b*np.sinh(t)
    x2 = -a*np.cosh(t)
    y2 = b*np.sinh(t)
    p1 = np.array([x1,y1]).T
    p2 = np.array([x2,y2]).T
    return [p1,p2]

def rotate(rotation,curve):
    r = R.from_euler("z",rotation)
    res = []
    for p in curve:
        v = np.array([*p,0])
        v = r.apply(v)[:2]
        res.append(v)
    return np.array(res)

def hyperbolic_curve(f1,f2,phi,bbox):
    Vs = 1500 # m/s
    a = (Vs*np.abs(phi))/2
    v = f1-f2
    translation = np.array([(f1[0]+f2[0])/2,(f1[1]+f2[1])/2])
    rotation = np.arctan2(v[1],v[0])
    c = np.linalg.norm(v)/2
    b = np.sqrt(c*c-a*a)
    diagbbox = np.linalg.norm(bbox[:,1])
    t1 = np.arccosh(diagbbox/a)
    t2 = np.arcsinh(diagbbox/b)
    tmin = np.min([t1,t2])
    t = np.linspace(-tmin,tmin,256)
    z1,z2 = hyperbola(a,b,c,t)
    return rotate(rotation,z1)+translation,rotate(rotation,z2)+translation

def _plot_vector(ax,a,b,color='w'):
    dx = b[0] - a[0]
    dy = b[1] - a[1]
    vec_ab_magnitude = np.sqrt(dx**2+dy**2)
    dx = dx / vec_ab_magnitude
    dy = dy / vec_ab_magnitude
    head_length = 0.025
    vec_ab_magnitude = vec_ab_magnitude - head_length
    ax.arrow(a[0], a[1], vec_ab_magnitude*dx, vec_ab_magnitude*dy, 
        head_width=head_length, 
        head_length=head_length,
        width = head_length * 0.1,
        color = color
    )

def plot_vector(ax,a,b,color='w'):
    _plot_vector(ax,a,a+b,color)

def plot_hyperbolic_curve(ax,f1,f2,a,bbox,alpha=1):
    z1,z2 = hyperbolic_curve(f1,f2,a,bbox)
    ax.plot(*z1.T,alpha=alpha)
    ax.plot(*z2.T,alpha=alpha)

def annotate_sensor(ax,name,f1):
    ax.text(f1[0],f1[1],name,ha='center', va='center')

# %%

sim_data = sio.loadmat('sim_data.mat')
sensor_data = sim_data['sensor_data']
sensor_pos = sim_data['mask'].T
source_pos = sim_data['source_pos'].T
grid_x_vec = sim_data['grid_x_vec']
grid_y_vec = sim_data['grid_y_vec']
grid_t_vec = sim_data['grid_t_vec'][0]

# %%
fig,axs = plt.subplots(5,1,sharex=True)
fig.tight_layout()
fig.set_figheight(7)
for i,zipped in enumerate(zip(axs,sensor_data)):
    ax,data = zipped
    ax.plot(data)
    ax.set_title(f'sensor {i}')


# %%

phi = 10e-6
bbox = np.array([
    [np.min(grid_x_vec),np.max(grid_x_vec)],
    [np.min(grid_y_vec),np.max(grid_y_vec)]
])
s1 = np.array([-0.1,0.1])
s2 = np.array([0.3,-0.3])

fig,ax = plt.subplots()
fig.set_figheight(10)
fig.set_figwidth(10)
ax.grid(color="0.2")
ax.set_xlim(bbox[0])
ax.set_ylim(bbox[1])

plot_hyperbolic_curve(ax,s1,s2,phi,bbox)
for i,pos in enumerate(sensor_pos):
    annotate_sensor(ax,f's{i}',pos)

# %%

#fig,ax = plt.subplots(2,1)
#fig.set_figheight(10)
#fig.set_figwidth(10)
#ax[0].grid(color="0.2")
#ax[1].grid(color="0.2")

correlation = []
dt = grid_t_vec[1]-grid_t_vec[0]
for i in range(5):
    for j in range(2,5):
        if j <= i:
            continue
        sensor1_data = sensor_data[i]
        sensor2_data = sensor_data[j]
        corr = np.correlate(sensor1_data,sensor2_data,mode="same")
        #ax[0].plot(corr)
        #ax[1].plot(grid_t_vec,sensor1_data)
        #ax[1].plot(grid_t_vec,sensor2_data)
        m = dt*(np.argmax(corr)-len(corr)/2.0)
        correlation.append((i,j,m))

bbox = np.array([
    [np.min(grid_x_vec),np.max(grid_x_vec)],
    [np.min(grid_y_vec),np.max(grid_y_vec)]
])

fig,ax = plt.subplots()
fig.set_figheight(10)
fig.set_figwidth(10)

points = []
for i,j,phi in correlation:
    s1 = sensor_pos[i]
    s2 = sensor_pos[j]
    plot_hyperbolic_curve(ax,s1,s2,phi,bbox,alpha=0.3)
    z1,z2 = hyperbolic_curve(s1,s2,phi,bbox)
    points.extend(z1)
    points.extend(z2)

for i,pos in enumerate(sensor_pos):
    annotate_sensor(ax,f's{i}',pos)

ax.set_xlim(bbox[0])
ax.set_ylim(bbox[1])

fig,ax = plt.subplots()
fig.set_figheight(10)
fig.set_figwidth(10)

points = np.array(points)
N = 10
ax.hist2d(*points.T,
    bins=[len(grid_x_vec)//N,len(grid_y_vec)//N],
    range=bbox,
)

for i,pos in enumerate(sensor_pos):
    annotate_sensor(ax,f's{i}',pos)
ax.set_xlim(bbox[0])
ax.set_ylim(bbox[1])


# %%
