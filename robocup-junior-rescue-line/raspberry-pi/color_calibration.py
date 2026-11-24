import cv2
import numpy as np
from sklearn.cluster import KMeans

def nothing(x):
    pass  # Funzione vuota per le trackbar

def create_trackbars():
    cv2.namedWindow("Trackbars GreenHSV")
    cv2.createTrackbar("Lower-H", "Trackbars GreenHSV", 43, 179, nothing)
    cv2.createTrackbar("Lower-S", "Trackbars GreenHSV", 97, 255, nothing)
    cv2.createTrackbar("Lower-V", "Trackbars GreenHSV", 40, 255, nothing)
    cv2.createTrackbar("Upper-H", "Trackbars GreenHSV", 78, 179, nothing)
    cv2.createTrackbar("Upper-S", "Trackbars GreenHSV", 182, 255, nothing)
    cv2.createTrackbar("Upper-V", "Trackbars GreenHSV", 182, 255, nothing)

    cv2.namedWindow("Trackbars BlackHSV")
    cv2.createTrackbar("Lower-H", "Trackbars BlackHSV", 0, 179, nothing)
    cv2.createTrackbar("Lower-S", "Trackbars BlackHSV", 0, 255, nothing)
    cv2.createTrackbar("Lower-V", "Trackbars BlackHSV", 17, 255, nothing)
    cv2.createTrackbar("Upper-H", "Trackbars BlackHSV", 180, 179, nothing)
    cv2.createTrackbar("Upper-S", "Trackbars BlackHSV", 49, 255, nothing)
    cv2.createTrackbar("Upper-V", "Trackbars BlackHSV", 130, 255, nothing)

    cv2.namedWindow("Trackbars GreenLAB")
    cv2.createTrackbar("L Min", "Trackbars GreenLAB", 130, 255, nothing)
    cv2.createTrackbar("L Max", "Trackbars GreenLAB", 189, 255, nothing)
    cv2.createTrackbar("A Min", "Trackbars GreenLAB", 64, 255, nothing)
    cv2.createTrackbar("A Max", "Trackbars GreenLAB", 153, 255, nothing)
    cv2.createTrackbar("B Min", "Trackbars GreenLAB", 125, 255, nothing)
    cv2.createTrackbar("B Max", "Trackbars GreenLAB", 191, 255, nothing)

    cv2.namedWindow("Trackbars BlackLAB")
    cv2.createTrackbar("L Min", "Trackbars BlackLAB", 0, 255, nothing)
    cv2.createTrackbar("L Max", "Trackbars BlackLAB", 127, 255, nothing)
    cv2.createTrackbar("A Min", "Trackbars BlackLAB", 20, 255, nothing)
    cv2.createTrackbar("A Max", "Trackbars BlackLAB", 144, 255, nothing)
    cv2.createTrackbar("B Min", "Trackbars BlackLAB", 92, 255, nothing)
    cv2.createTrackbar("B Max", "Trackbars BlackLAB", 255, 255, nothing)

def get_trackbar_valuesHSV(window_name):
    lower_h = cv2.getTrackbarPos("Lower-H", window_name)
    lower_s = cv2.getTrackbarPos("Lower-S", window_name)
    lower_v = cv2.getTrackbarPos("Lower-V", window_name)
    upper_h = cv2.getTrackbarPos("Upper-H", window_name)
    upper_s = cv2.getTrackbarPos("Upper-S", window_name)
    upper_v = cv2.getTrackbarPos("Upper-V", window_name)
    return (np.array([lower_h, lower_s, lower_v]), np.array([upper_h, upper_s, upper_v]))

def get_trackbar_valuesLAB(window_name):
    L_min = cv2.getTrackbarPos("L Min", window_name)
    L_max= cv2.getTrackbarPos("L Max", window_name)
    A_min = cv2.getTrackbarPos("A Min", window_name)
    A_max= cv2.getTrackbarPos("A Max", window_name)
    B_min = cv2.getTrackbarPos("B Min", window_name)
    B_max = cv2.getTrackbarPos("B Max", window_name)
    return (np.array([L_min, A_min, B_min ]), np.array([L_max, A_max, B_max]))

def apply_kmeans(image, k=2):
    pixels = image.reshape(-1, 3)
    kmeans = KMeans(n_clusters=k)
    kmeans.fit(pixels)
    clustered = kmeans.labels_.reshape(image.shape[:2])
    return clustered, kmeans.cluster_centers_


def main():
    create_trackbars()

    cap = cv2.VideoCapture(0)

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Errore nella lettura della telecamera.")
            break

        frame = cv2.resize(frame, (640, 480))

        # Conversione in HSV
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        # Ottieni i valori dei trackbar per il verde e il nero
        lower_green, upper_green = get_trackbar_valuesHSV("Trackbars GreenHSV")
        lower_black, upper_black = get_trackbar_valuesHSV("Trackbars BlackHSV")
        print(f"lower_greenHSV: {lower_green} ")
        print(f"upper_greenHSV: {upper_green} ")
        print(f"lower_blackHSV: {lower_black} ")
        print(f"upper_blackHSV: {upper_black} ")
        # Maschere per il verde e il nero HSV
        mask_greenHSV = cv2.inRange(hsv, lower_green, upper_green)
        mask_blackHSV = cv2.inRange(hsv, lower_black, upper_black)
        print("                               ")
        #Conversione in LAB
        lab = cv2.cvtColor(frame, cv2.COLOR_BGR2LAB)
        #Ottieni i valori dei trackbar per il verede e il nero in LAB
        lower_green, upper_green = get_trackbar_valuesLAB("Trackbars GreenLAB")
        lower_black, upper_black = get_trackbar_valuesLAB("Trackbars BlackLAB")
        print(f"lower_greenLAB: {lower_green} ")
        print(f"upper_greenLAB: {upper_green} ")
        print(f"lower_blackLAB: {lower_black} ")
        print(f"upper_blackLAB: {upper_black} ")
        # Maschere per il verde e il nero LAB
        mask_greenLAB = cv2.inRange(lab, lower_green, upper_green)
        mask_blackLAB = cv2.inRange(lab, lower_black, upper_black)


        # Combina le maschere Green HSV e LAB
        combined_mask_green = cv2.bitwise_and(mask_greenHSV, mask_greenLAB)
        refined_result_green = cv2.bitwise_and(frame, frame, mask=combined_mask_green)

        # 3. Applica K-Means per segmentazione avanzata Green
        clustered, centers = apply_kmeans(refined_result_green, k=2)
        clustered_visual_green = np.zeros_like(frame)
        for i, center in enumerate(centers.astype('uint8')):
            clustered_visual_green[clustered == i] = center

        # Converti clustered_visual_green in scala di grigi
        clustered_visual_green_gray = cv2.cvtColor(clustered_visual_green, cv2.COLOR_BGR2GRAY)

        # Converti l'immagine in una maschera binaria
        _, clustered_visual_green_binary = cv2.threshold(clustered_visual_green_gray, 1, 255, cv2.THRESH_BINARY)

        # Ora trova i contorni sull'immagine binaria
        contours_green, _ = cv2.findContours(clustered_visual_green_binary, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)


        # Combina le maschere Black HSV e LAB
        combined_mask_black = cv2.bitwise_and(mask_blackHSV, mask_blackLAB)
        refined_result_black = cv2.bitwise_and(frame, frame, mask=combined_mask_black)

        # 3. Applica K-Means per segmentazione avanzata Black
        clustered, centers = apply_kmeans(refined_result_black, k=2)
        clustered_visual_black = np.zeros_like(frame)
        for i, center in enumerate(centers.astype('uint8')):
          clustered_visual_black[clustered == i] = center

          # Converti clustered_visual_green in scala di grigi
          clustered_visual_black_gray = cv2.cvtColor(clustered_visual_black, cv2.COLOR_BGR2GRAY)

          # Converti l'immagine in una maschera binaria
          _, clustered_visual_black_binary = cv2.threshold(clustered_visual_black_gray, 1, 255, cv2.THRESH_BINARY)

        # Mostra i risultati
        cv2.imshow("Frame", frame)
        cv2.imshow("HSV Mask green", mask_greenHSV)
        cv2.imshow("LAB Mask green", mask_greenLAB)
        cv2.imshow("HSV Mask black", mask_blackHSV)
        cv2.imshow("LAB Mask black", mask_blackLAB)
        cv2.imshow("Result green", refined_result_green)
        cv2.imshow("Result black", refined_result_black)
        cv2.imshow("final visual green", clustered_visual_green)
        cv2.imshow("final visual black", clustered_visual_black)


        key = cv2.waitKey(1) & 0xFF
        if key == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()