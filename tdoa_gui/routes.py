"""Routes for parent Flask app."""
from flask import current_app as app
from flask import render_template
from flask_sock import Sock
import queue

sock = Sock(app)
queues = dict()

@sock.route('/tx<int:transmitter_id>')
def echo(ws,transmitter_id):
    while True:
        data = ws.receive()
        print("ws receive:",transmitter_id,len(data))
        queues.setdefault(transmitter_id,queue.Queue()).put_nowait(data)

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
