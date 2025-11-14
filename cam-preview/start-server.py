from flask import Flask, Response, send_file, render_template_string
from picamera2 import Picamera2
from libcamera import Transform
import io
import cv2

app = Flask(__name__)
camera = Picamera2()
config = camera.create_video_configuration(main={"size": (640, 480)})
config["transform"] = Transform(vflip=True, hflip=True)
camera.configure(config)
camera.start()

def generate_frames():
    """Stream video frames continuously."""
    while True:
        frame = camera.capture_array()
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        _, buffer = cv2.imencode('.jpg', frame_rgb)
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + buffer.tobytes() + b'\r\n')

@app.route('/video_feed')
def video_feed():
    """Stream video feed."""
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/get_image')
def get_image():
    """Capture a high-quality image and send it to the client."""
    # Switch to high-quality still capture configuration
    camera.stop()
    config = camera.create_still_configuration()
    config["transform"] = Transform(vflip=True, hflip=True)
    camera.configure(config)
    camera.start()

    # Capture the high-quality image
    image_buffer = io.BytesIO()
    camera.capture_file(image_buffer, format='jpeg')
    image_buffer.seek(0)

    # Switch back to video streaming configuration
    camera.stop()
    config = camera.create_video_configuration(main={"size": (640, 480)})
    config["transform"] = Transform(vflip=True, hflip=True)
    camera.configure(config)
    camera.start()

    return send_file(image_buffer, mimetype='image/jpeg', as_attachment=True, download_name='high_quality_image.jpg')

@app.route('/')
def index():
    """Index route with embedded video feed."""
    return render_template_string('''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Camera Server</title>
    </head>
    <body>
        <h1>Camera Server</h1>
        <img src="/video_feed" alt="Live Video Feed" width="640" height="480">
        <br>
        <form action="/get_image" method="get">
            <button type="submit">Take HQ Picture</button>
        </form>
    </body>
    </html>
    ''')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
