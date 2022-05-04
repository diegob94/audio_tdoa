import dash
from dash import dcc, html
import plotly.graph_objects as go
import dash_bootstrap_components as dbc
from dash_bootstrap_templates import load_figure_template
from dash.dependencies import Input, Output
import pybinn
import numpy as np
import queue

def init_dashboard(server,queues):
    load_figure_template("darkly")

    dash_app = dash.Dash(__name__, server=server, update_title=None, external_stylesheets=[dbc.themes.DARKLY])  # remove "Updating..." from title

    resolution = 20

    @dash_app.callback(Output('graph', 'extendData'), [Input('interval', 'n_intervals')])
    def update_data(n_intervals):
        transmitter_id = 1
        data = None
        if not transmitter_id in queues:
            return None
        try:
            data = queues[transmitter_id].get_nowait()
        except queue.Empty:
            pass
        try:
            data = pybinn.loads(data)
        except AttributeError:
            return None
        print("plot receive:",transmitter_id,np.array(data).shape)
        if data is not None and n_intervals is not None:
            #print(f"{n_intervals=} {n_intervals % resolution=} {data=}")
            index = n_intervals % resolution
            # tuple is (dict of new data, target trace index, number of points to keep)
            x = [i[0] for i in data]
            y = [i[1] for i in data]
            print(x[0],y[0])
            print(x[-1],y[-1])
            return dict(x=[x], y=[y]), [0], 100

    figure = go.Figure(data=[{'x': [], 'y': []}], layout=dict(
            xaxis=dict(range=[0, 100]),
            yaxis=dict(range=[-1, 1]),
            template='darkly',
        )
    )

    dash_app.layout = html.Div([
            dcc.Graph(id='graph', figure=figure),
            dcc.Interval(id="interval")
        ],style={'width':'60vw','margin':'auto'})

    return dash_app.server

