import time
import os
from datetime import datetime
from pathlib import Path

from picamera2 import Picamera2
from picamera2.encoders import H264Encoder
from picamera2.outputs import FfmpegOutput

def main():
    # Create "recordings" directory next to this script
    base_dir = Path(__file__).resolve().parent
    recordings_dir = base_dir / "recordings"
    recordings_dir.mkdir(exist_ok=True)

    # Timestamped output filename
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_file = recordings_dir / f"recording_{timestamp}.mkv"

    print(f"Saving video to: {output_file}")

    # Initialize camera
    picam2 = Picamera2()
    video_config = picam2.create_video_configuration(
        main={"size": (1920, 1080)},
        controls={"FrameDurationLimits": (33333, 33333)}  # 30 FPS
    )
    picam2.configure(video_config)

    # Set up encoder and output
    encoder = H264Encoder(bitrate=8_000_000)
    output = FfmpegOutput(str(output_file))

    # Start recording
    picam2.start()
    picam2.start_recording(encoder, output)
    print("Recording started. Press Ctrl+C to stop.")

    try:
        while True:
            time.sleep(1)
            os.sync()  # flush data to disk
    except KeyboardInterrupt:
        print("\nStopping recording...")
    finally:
        picam2.stop_recording()
        picam2.stop()
        print(f"Recording saved to: {output_file}")

if __name__ == "__main__":
    main()
