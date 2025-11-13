# yolo_from_mjpeg.py
import cv2
import time
import torch
from ultralytics import YOLO

STREAM_URL = "http://192.168.1.161:5000/video_feed"  # your existing stream
MODEL_NAME = "yolov8n.pt"        # fast + light; try yolov8s.pt for better accuracy
CONF_THRESH = 0.4
IOU_THRESH  = 0.45

# Heuristic for "obstacle":
AREA_FRAC_OBSTACLE = 0.06        # box area ≥ 6% of frame
CENTER_FRAC = 0.40               # center box is 40% of frame width/height

# Optional: only treat these classes as obstacles (comment out to allow all)
OBSTACLE_CLASSES = {
    "person","bicycle","car","motorcycle","bus","truck","train",
    "dog","cat","chair","bench","potted plant","bottle","backpack"
}

def main():
    print(f"Opening stream: {STREAM_URL}")
    cap = cv2.VideoCapture(STREAM_URL)

    if not cap.isOpened():
        raise RuntimeError(
            "Could not open stream. If this is not MJPEG, or requires auth, "
            "OpenCV may fail. Try opening the URL in a browser to verify."
        )

    model = YOLO(MODEL_NAME)
    use_cuda = torch.cuda.is_available()
    fps_ema, t_prev = 0.0, time.time()

    while True:
        ok, frame = cap.read()
        if not ok or frame is None:
            print("Frame read failed… retrying")
            time.sleep(0.05)
            continue

        h, w = frame.shape[:2]

        # (Optional) downscale for speed; comment out if you want full-res
        # frame = cv2.resize(frame, (w//2, h//2))
        # h, w = frame.shape[:2]

        results = model.predict(
            frame,
            device=0 if use_cuda else "cpu",
            conf=CONF_THRESH,
            iou=IOU_THRESH,
            verbose=False
        )

        annotated = frame.copy()
        obstacle = False

        # Center region
        cx0, cy0 = int((1 - CENTER_FRAC) / 2 * w), int((1 - CENTER_FRAC) / 2 * h)
        cx1, cy1 = int((1 + CENTER_FRAC) / 2 * w), int((1 + CENTER_FRAC) / 2 * h)
        cv2.rectangle(annotated, (cx0, cy0), (cx1, cy1), (255, 255, 255), 1)

        for r in results:
            boxes = getattr(r, "boxes", None)
            names = getattr(r, "names", {})
            if boxes is None:
                continue

            for b in boxes:
                cls_id = int(b.cls[0])
                cls_name = names.get(cls_id, str(cls_id))
                conf = float(b.conf[0])
                x1, y1, x2, y2 = map(int, b.xyxy[0])

                # If class filter in use, skip non-obstacles
                if OBSTACLE_CLASSES and cls_name not in OBSTACLE_CLASSES:
                    continue

                # Draw
                cv2.rectangle(annotated, (x1, y1), (x2, y2), (0, 255, 0), 2)
                cv2.putText(
                    annotated, f"{cls_name} {conf:.2f}",
                    (x1, max(y1 - 6, 10)),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2
                )

                # Heuristics
                box_area = (x2 - x1) * (y2 - y1)
                if box_area / (w * h) >= AREA_FRAC_OBSTACLE:
                    obstacle = True
                # intersects center region?
                if not (x2 < cx0 or x1 > cx1 or y2 < cy0 or y1 > cy1):
                    obstacle = True

        # FPS
        t_now = time.time()
        fps = 1.0 / max(1e-6, (t_now - t_prev))
        fps_ema = 0.9 * fps_ema + 0.1 * fps
        t_prev = t_now

        status = "OBSTACLE!" if obstacle else "clear"
        color = (0, 0, 255) if obstacle else (0, 200, 0)

        cv2.putText(annotated, f"Status: {status}", (10, 24),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, color, 2)
        cv2.putText(annotated, f"FPS: {fps_ema:.1f}", (10, 50),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

        cv2.imshow("YOLO on Pi MJPEG stream", annotated)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
