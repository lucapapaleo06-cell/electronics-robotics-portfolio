# Vision-Controlled Robotic Hand

A webcam watches my hand and a 3D-printed robotic hand with 5 servos copies which fingers are open or bent, in real time.

## How it works

1. **`python/hand_tracking_serial.py`** reads the webcam with **OpenCV** and finds the 21 hand landmarks with **MediaPipe Hands**.
2. For each finger it measures the distance between the fingertip and the wrist. If the distance is below a threshold, the finger counts as bent. The thresholds scale with the apparent hand size, so detection works at different distances from the camera.
3. It sends a 5-character string over serial at 9600 baud, thumb to pinky (`1` = bent, `0` = open), for example `01100\n`.
4. **`arduino/robotic_hand/robotic_hand.ino`** reads the string and moves one servo per finger (pins 3, 5, 6, 9, 10).

## Hardware

- Arduino (any board with 5 PWM pins)
- 5 × hobby servos, one per finger
- USB webcam

## Software

Python 3, `opencv-python`, `mediapipe`, `pyserial`, `numpy`

```bash
pip install opencv-python mediapipe pyserial numpy
python python/hand_tracking_serial.py
```

Change the serial port (`COM9`) in the script to match your Arduino.

## Status

Prototype.
