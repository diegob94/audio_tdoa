addpath('./k_wave/k-Wave');

% create the computational grid
Lx = 1
Ly = 1
Nx = 256;      % number of grid points in the x (row) direction
Ny = 256;      % number of grid points in the y (column) direction
dx = Lx/Nx;    % grid point spacing in the x direction [m]
dy = Ly/Ny;    % grid point spacing in the y direction [m]
kgrid = kWaveGrid(Nx, dx, Ny, dy);
grid_x_vec = kgrid.x_vec;
grid_y_vec = kgrid.y_vec;
Lx = max(grid_x_vec)-min(grid_x_vec);
Ly = max(grid_y_vec)-min(grid_y_vec);
#kgrid.setTime(1,1)

% define the medium properties
medium.sound_speed = 1500*ones(Nx, Ny); % [m/s]
#medium.sound_speed(1:50, :) = 1800;     % [m/s]
medium.density = 1040;                  % [kg/m^3]

% define an initial pressure using makeDisc
source_pos = [0;0];
disc_x_pos = source_pos(1);             % [grid points]
disc_y_pos = source_pos(2);             % [grid points]
disc_radius = 8;                        % [grid points]
disc_mag = 3;                           % [Pa]
source.p0 = disc_mag*makeDisc(Nx, Ny, disc_x_pos, disc_y_pos, disc_radius);

% define sensor mask
N = 5;
sensor.mask = [rand(1,N)*Lx+min(grid_x_vec);rand(1,N)*Ly+min(grid_y_vec)];

% run the simulation
input_args = {'PlotLayout',true};
sensor_data = kspaceFirstOrder2D(kgrid, medium, source, sensor, input_args{:});

grid_t_vec = kgrid.t_array;

data_file = "sim_data.mat"
save("-7",strcat(data_file), "sensor_data")
save("-7",'-append', strcat(data_file), '-struct', "sensor", "mask")
save("-7",'-append', strcat(data_file), "source_pos")
save("-7",'-append', strcat(data_file), "grid_x_vec")
save("-7",'-append', strcat(data_file), "grid_y_vec")
save("-7",'-append', strcat(data_file), "grid_t_vec")

for i = [1:5]
    subplot (5, 1, i)
    plot(sensor_data(i,:))
endfor