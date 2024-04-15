from flask import Flask, render_template, request
from flask_cors import CORS

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})  # Allowing all origins here for testing

@app.route('/')
def index():
    app.logger.info(f"Request to main index from {request.remote_addr}")
    return render_template('index.html')

@app.after_request
def after_request(response):
    app.logger.info("Headers set on response:")
    for name, value in response.headers:
        app.logger.info(f"{name}: {value}")
    return response

if __name__ == '__main__':
    app.run(debug=True)