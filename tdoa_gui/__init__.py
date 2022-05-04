"""Initialize Flask app."""
from flask import Flask

def init_app():
    """Construct core Flask application with embedded Dash app."""
    app = Flask(__name__, instance_relative_config=False)
    app.config['SOCK_SERVER_OPTIONS'] = {'ping_interval': 25}
    with app.app_context():
        # Import parts of our core Flask app
        from . import routes

        # Import Dash application
        from .dashboard import init_dashboard

        app = init_dashboard(app, routes.queues)

        return app
