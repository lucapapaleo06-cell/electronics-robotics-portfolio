import cv2
import numpy as np
from sklearn.cluster import KMeans
import serial
import time





def main():

    cap = cv2.VideoCapture(0)

    # Configurazione della seriale
    arduino = serial.Serial('COM12', 115200, timeout=1)  # Cambia 'COM3' con la porta corretta
    time.sleep(2)  # Attendi l'inizializzazione di Arduino

    last_directions = []

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Errore nella lettura della telecamera.")
            break

        frame = cv2.resize(frame, (640, 480))

        # Conversione in HSV
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        # Ottieni i valori dei trackbar per il verde e il nero
        lower_green, upper_green = (np.array([43, 97, 40]), np.array([78, 182, 182]))
        lower_black, upper_black = (np.array([0, 0, 117]), np.array([180, 49, 130]))
        # Maschere per il verde e il nero HSV
        mask_greenHSV = cv2.inRange(hsv, lower_green, upper_green)
        mask_blackHSV = cv2.inRange(hsv, lower_black, upper_black)

        #Conversione in LAB
        lab = cv2.cvtColor(frame, cv2.COLOR_BGR2LAB)
        #Ottieni i valori dei trackbar per il verede e il nero in LAB
        lower_green, upper_green = (np.array([130, 64, 125]), np.array([189, 153, 191]))
        lower_black, upper_black = (np.array([0, 20, 92]), np.array([127, 144, 255]))
        # Maschere per il verde e il nero LAB
        mask_greenLAB = cv2.inRange(lab, lower_green, upper_green)
        mask_blackLAB = cv2.inRange(lab, lower_black, upper_black)


        # Combina le maschere Green HSV e LAB
        combined_mask_green = cv2.bitwise_and(mask_greenHSV, mask_greenLAB)
        refined_result_green = cv2.bitwise_and(frame, frame, mask=combined_mask_green)


        # Ora trova i contorni sull'immagine binaria
        contours_green, _ = cv2.findContours(combined_mask_green, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)


        # Combina le maschere Black HSV e LAB
        combined_mask_black = cv2.bitwise_and(mask_blackHSV, mask_blackLAB)
        refined_result_black = cv2.bitwise_and(frame, frame, mask=combined_mask_black)



        green_rectangles = []

        for contour in contours_green:
            area = cv2.contourArea(contour)
            if area > 450:
                x, y, w, h = cv2.boundingRect(contour)
                if y + h // 2 > 10:  # Considera solo i rettangoli più alti del centro del frame
                    green_rectangles.append((x, y, w, h))
                    cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        if green_rectangles:
            contours_black, _ = cv2.findContours(combined_mask_black, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            yellow_points = []
            directions = []
            for contour in contours_black:
                area = cv2.contourArea(contour)
                if area > 1500:  # Considera solo contorni significativi
                    # Approssima i contorni con un poligono
                    epsilon = 0.02 * cv2.arcLength(contour, True)
                    approx = cv2.approxPolyDP(contour, epsilon, True)

                    # Trova i punti più vicini al centro
                    center_frame = np.array([320, 240])
                    points_with_distance = []

                    for i in range(len(approx)):
                        p1 = approx[i][0]
                        p2 = approx[(i + 1) % len(approx)][0]
                        p3 = approx[(i + 2) % len(approx)][0]

                        # Calcola i vettori
                        v1 = p2 - p1
                        v2 = p3 - p2

                        # Calcola l'angolo
                        angle = np.arccos(
                            np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))
                        )

                        angle_deg = np.degrees(angle)

                        # Se l'angolo è circa 90 gradi, calcola la distanza dal centro
                        if 80 <= angle_deg <= 100:
                            distance = np.linalg.norm(p2 - center_frame)
                            points_with_distance.append((p2, distance))

                            # Ordina i punti per distanza e seleziona i due più vicini
                            points_with_distance.sort(key=lambda x: x[1])
                            closest_points = [p[0] for p in points_with_distance[:2]]

                            # Aggiungi i punti gialli se vicini ai rettangoli verdi
                            for point in closest_points:
                                for gx, gy, gw, gh in green_rectangles:
                                    g_center_x = gx + gw // 2
                                    g_center_y = gy + gh // 2
                                    if np.linalg.norm(np.array([g_center_x, g_center_y]) - point) < 100:  # Soglia di vicinanza
                                        yellow_points.append(tuple(point))
                                        cv2.circle(frame, tuple(point), 5, (0, 255, 255), -1)
            for gx, gy, gw, gh in green_rectangles:
                g_center_x = gx + gw // 2
                g_center_y = gy + gh // 2

                for point in yellow_points:
                    y_center_x, y_center_y = point
                    if g_center_y >= y_center_y:  # Considera solo i rettangoli sotto o alla stessa altezza
                        if g_center_x < y_center_x:
                            directions.append("S")  # Sinistra
                        elif g_center_x > y_center_x:
                            directions.append("D")  # Destra
                        else:
                            directions.append("A")  # Avanti
                    elif g_center_y < y_center_y:
                         directions.append("A")  # Avanti

            if directions.count("S") > 0 and directions.count("D") > 0:
                final_direction = "I"
            elif "S" in directions:
                final_direction = "S"
            elif "D" in directions:
                final_direction = "D"
            else:
                final_direction = "A"

            # Memorizza le ultime tre direzioni
            last_directions.append(final_direction)
            time.sleep(0.2)
            if len(last_directions) > 3:
                last_directions.pop(0)

            # Controlla se la stessa direzione è stata rilevata tre volte
            if (last_directions.count(last_directions[0]) == 3) and ( final_direction != 'A'):
                try:
                    # Invia il comando ad Arduino
                    command = last_directions[0] + "&"
                    arduino.write(command.encode())

                    # Attendi la risposta da Arduino
                    response = arduino.readline().decode().strip()
                    if response:
                        print(f"Arduino ha risposto: {response}")
                except Exception as e:
                    print(f"Errore di comunicazione con Arduino: {e}")
        else : final_direction="no verdi"




        # Mostra i risultati
        cv2.putText(frame, f"Direction: {final_direction}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
        cv2.imshow("Frame", frame)
        cv2.imshow("final visual green", refined_result_green)
        cv2.imshow("final visual black", refined_result_black)

        key = cv2.waitKey(1) & 0xFF
        if key == 27:
            break

    cap.release()
    cv2.destroyAllWindows()
    arduino.close()

if __name__ == "__main__":
    main()

