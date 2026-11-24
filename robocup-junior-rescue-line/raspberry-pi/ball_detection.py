import cv2
import numpy as np
import imutils
import time
import math

def nothing(x):
    pass

# ------------------------------------------------
# TRACKBAR HSV
# ------------------------------------------------
cv2.namedWindow("HSV Trackbars")
cv2.resizeWindow("HSV Trackbars", 400, 300)
cv2.createTrackbar("H_min", "HSV Trackbars", 0, 180, nothing)
cv2.createTrackbar("H_max", "HSV Trackbars", 180, 180, nothing)
cv2.createTrackbar("S_min", "HSV Trackbars", 0, 255, nothing)
cv2.createTrackbar("S_max", "HSV Trackbars", 20, 255, nothing)
cv2.createTrackbar("V_min", "HSV Trackbars", 180, 255, nothing)
cv2.createTrackbar("V_max", "HSV Trackbars", 255, 255, nothing)

# ------------------------------------------------
# TRACKBAR LAB
# ------------------------------------------------
cv2.namedWindow("LAB Trackbars")
cv2.resizeWindow("LAB Trackbars", 400, 300)
cv2.createTrackbar("L_min", "LAB Trackbars", 200, 255, nothing)
cv2.createTrackbar("L_max", "LAB Trackbars", 255, 255, nothing)
cv2.createTrackbar("A_min", "LAB Trackbars", 120, 255, nothing)
cv2.createTrackbar("A_max", "LAB Trackbars", 140, 255, nothing)
cv2.createTrackbar("B_min", "LAB Trackbars", 120, 255, nothing)
cv2.createTrackbar("B_max", "LAB Trackbars", 140, 255, nothing)

# Parametri cerchi
MIN_RADIUS = 10
MIN_SILVER_RATIO = 0.5

cap = cv2.VideoCapture(0)
time.sleep(2.0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = imutils.resize(frame, width=600)
    blurred = cv2.GaussianBlur(frame, (11, 11), 0)

    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
    lab = cv2.cvtColor(blurred, cv2.COLOR_BGR2LAB)

    # HSV Trackbar Values
    h_min = cv2.getTrackbarPos("H_min", "HSV Trackbars")
    h_max = cv2.getTrackbarPos("H_max", "HSV Trackbars")
    s_min = cv2.getTrackbarPos("S_min", "HSV Trackbars")
    s_max = cv2.getTrackbarPos("S_max", "HSV Trackbars")
    v_min = cv2.getTrackbarPos("V_min", "HSV Trackbars")
    v_max = cv2.getTrackbarPos("V_max", "HSV Trackbars")
    hsv_lower = np.array([h_min, s_min, v_min])
    hsv_upper = np.array([h_max, s_max, v_max])

    # LAB Trackbar Values
    l_min = cv2.getTrackbarPos("L_min", "LAB Trackbars")
    l_max = cv2.getTrackbarPos("L_max", "LAB Trackbars")
    a_min = cv2.getTrackbarPos("A_min", "LAB Trackbars")
    a_max = cv2.getTrackbarPos("A_max", "LAB Trackbars")
    b_min = cv2.getTrackbarPos("B_min", "LAB Trackbars")
    b_max = cv2.getTrackbarPos("B_max", "LAB Trackbars")
    lab_lower = np.array([l_min, a_min, b_min])
    lab_upper = np.array([l_max, a_max, b_max])

    mask_hsv = cv2.inRange(hsv, hsv_lower, hsv_upper)
    mask_lab = cv2.inRange(lab, lab_lower, lab_upper)

    mask_hsv = cv2.erode(mask_hsv, None, iterations=2)
    mask_hsv = cv2.dilate(mask_hsv, None, iterations=2)
    mask_lab = cv2.erode(mask_lab, None, iterations=2)
    mask_lab = cv2.dilate(mask_lab, None, iterations=2)

    combined_mask = cv2.bitwise_and(mask_hsv, mask_lab)
    combined_mask = cv2.erode(combined_mask, None, iterations=1)
    combined_mask = cv2.dilate(combined_mask, None, iterations=1)

    gray = cv2.cvtColor(blurred, cv2.COLOR_BGR2GRAY)
    circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, 1, 20,
                               param1=50, param2=30, minRadius=MIN_RADIUS, maxRadius=80)

    best_circle = None
    best_ratio = 0

    if circles is not None:
        circles = np.uint16(np.around(circles))
        for i in circles[0, :]:
            x, y, radius = i[0], i[1], i[2]
            circle_mask = np.zeros(combined_mask.shape, dtype=np.uint8)
            cv2.circle(circle_mask, (x, y), radius, 255, thickness=-1)
            region = cv2.bitwise_and(combined_mask, combined_mask, mask=circle_mask)
            silver_pixels = cv2.countNonZero(region)
            circle_area = math.pi * (radius ** 2)
            ratio = silver_pixels / float(circle_area)
            if ratio > MIN_SILVER_RATIO and ratio > best_ratio:
                best_circle = (x, y, radius, ratio)
                best_ratio = ratio

    if best_circle is not None:
        x, y, radius, ratio = best_circle
        cv2.circle(frame, (x, y), radius, (0, 0, 255), 2)
        cv2.circle(frame, (x, y), 2, (0, 0, 255), 3)
        cv2.putText(frame, f"Silver Ball: {ratio:.2f}", (x - 40, y - 20),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)
        print(f"Centro: ({x}, {y}), Rapporto: {ratio:.2f}")

    cv2.imshow("Frame", frame)
    cv2.imshow("HSV Mask", mask_hsv)
    cv2.imshow("LAB Mask", mask_lab)
    cv2.imshow("Combined Mask", combined_mask)

    key = cv2.waitKey(1) & 0xFF
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()
