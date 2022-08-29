"""Routes for parent Flask app."""
from flask import current_app as app
from flask import render_template
from flask_sock import Sock
import queue

sock = Sock(app)
queues = dict()
connections = dict()
connection_counter = 0

@sock.route('/tx<int:transmitter_id>')
def echo(ws,transmitter_id):
    global connection_counter
    connection_id = connection_counter
    connection_counter += 1
    while True:
        data = ws.receive()
        print("ws receive:",transmitter_id,len(data))
        q = queue.Queue()
        if not transmitter_id in connections:
            # new connection
            queues[transmitter_id] = q
            connections[transmitter_id] = connection_id
        elif connection_id >= connections[transmitter_id]:
            # connection exist or newer
            q = queues[transmitter_id]
            connections[transmitter_id] = connection_id
        else:
            # connection exist and not newer -> disconnect
            break
        q.put_nowait(data)
    ws.close()

@app.route("/")
def home():
    """Landing page."""
    return render_template(
        "index.jinja2",
        title="Plotly Dash Flask Tutorial",
        description="Embed Plotly Dash into your Flask applications.",
        template="home-template",
        body="This is a homepage served with Flask.",
    )
