import dash
from dash import dcc, html
import numpy as np
import plotly.graph_objects as go
import dash_bootstrap_components as dbc
from dash_bootstrap_templates import load_figure_template
from server import SignalServerProcess
from dash.dependencies import Input, Output
import queue

load_figure_template("darkly")

app = dash.Dash(__name__, update_title=None, external_stylesheets=[dbc.themes.DARKLY])  # remove "Updating..." from title

resolution = 20

figure = go.Figure(data=[{'x': [], 'y': []}], layout=dict(
        xaxis=dict(range=[-100, 100]),
        yaxis=dict(range=[-2, 2]),
        template='darkly',
    )
)
app.layout = html.Div([
        dcc.Graph(id='graph', figure=figure), dcc.Interval(id="interval")
    ],style={'width':'60vw','margin':'auto'})

@app.callback(Output('graph', 'extendData'), [Input('interval', 'n_intervals')])
def update_data(n_intervals):
    global signal_server
    data = None
    try:
        data = signal_server.queue.get_nowait()
    except queue.Empty:
        pass
    if data is not None and n_intervals is not None:
        print(f"{n_intervals=} {n_intervals % resolution=} {data=}")
        index = n_intervals % resolution
        # tuple is (dict of new data, target trace index, number of points to keep)
        x = index
        y = data
        return dict(x=[[x]], y=[[y]]), [0], 10

if __name__ == '__main__':
    signal_server = SignalServerProcess()
    app.run_server()
    signal_server.stop()

