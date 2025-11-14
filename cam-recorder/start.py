import time
import os
from datetime import datetime
from pathlib import Path

from picamera2 import Picamera2, Transform
from picamera2.encoders import H264Encoder
from picamera2.outputs import FfmpegOutput
from libcamera import Transform   # <-- this is the correct import on your setup

def main():
    # Create "recordings" directory next to this script
    base_dir = Path(__file__).resolve().parent
    recordings_dir = base_dir / "recordings"
    recordings_dir.mkdir(exist_ok=True)

    # Timestamped output filename
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_file = recordings_dir / f"recording_{timestamp}.mkv"

    print(f"Saving video to: {output_file}")

    picam2 = Picamera2()

<<<<<<< HEAD
    # Apply horizontal + vertical flip (equivalent to 180° rotation)
    video_config = picam2.create_video_configuration(
        main={"size": (1920, 1080)},
        controls={"FrameDurationLimits": (33333, 33333)},  # ~30 FPS
        transform=Transform(hflip=1, vflip=1)
=======
    # Apply 180-degree rotation here
    video_config = picam2.create_video_configuration(
        main={"size": (1920, 1080)},
        controls={"FrameDurationLimits": (33333, 33333)},  # 30 FPS
        transform=Transform(rotation=180)
>>>>>>> 7b56793a673bdfd1f300356335ae8a9ef2f8e814
    )

    picam2.configure(video_config)

    encoder = H264Encoder(bitrate=8_000_000)
    output = FfmpegOutput(str(output_file))

    picam2.start()
    picam2.start_recording(encoder, output)

    print("Recording started. Press Ctrl+C to stop.")

    try:
        while True:
            time.sleep(1)
            os.sync()
    except KeyboardInterrupt:
        print("\nStopping recording...")
    finally:
        picam2.stop_recording()
        picam2.stop()
        print(f"Recording saved to: {output_file}")

if __name__ == "__main__":
    main()
