import cv2
import numpy as np
import serial
import time
import mediapipe as mp

# Inizializza la comunicazione seriale con la scheda (es. Arduino)
ser = serial.Serial('COM9', 9600)
time.sleep(2)  # Attendi che la connessione si stabilizzi

# Inizializza MediaPipe Hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(min_detection_confidence=0.7, min_tracking_confidence=0.5)
mp_draw = mp.solutions.drawing_utils

# Soglie di base per considerare un dito piegato (valori iniziali)
BASE_THRESHOLD_THUMB = 0.165
BASE_THRESHOLD_INDEX = 0.158
BASE_THRESHOLD_MIDDLE = 0.158
BASE_THRESHOLD_RING = 0.158
BASE_THRESHOLD_PINKY = 0.158

# Distanze di controllo (in unità normalizzate)
MIN_HAND_DISTANCE = 0.1
MAX_HAND_DISTANCE = 0.3

def calculate_threshold(base_threshold, hand_distance, min_distance, max_distance):
    """Calcola la soglia dinamica basata sulla distanza della mano."""
    scale_factor = (hand_distance - min_distance) / (max_distance - min_distance)
    return base_threshold * (1 + scale_factor)

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Converti l'immagine BGR in RGB
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(frame_rgb)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Calcola la distanza tra la base del pollice e la base del mignolo
            thumb_base = hand_landmarks.landmark[1]
            pinky_base = hand_landmarks.landmark[17]
            hand_distance = np.sqrt((thumb_base.x - pinky_base.x) ** 2 + (thumb_base.y - pinky_base.y) ** 2)

            # Verifica se la distanza rientra nel range desiderato
            if MIN_HAND_DISTANCE <= hand_distance <= MAX_HAND_DISTANCE:
                mp_draw.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
                landmarks = hand_landmarks.landmark

                # Calcola la soglia dinamica per ogni dito
                thumb_threshold = calculate_threshold(BASE_THRESHOLD_THUMB, hand_distance, MIN_HAND_DISTANCE,
                                                      MAX_HAND_DISTANCE)
                index_threshold = calculate_threshold(BASE_THRESHOLD_INDEX, hand_distance, MIN_HAND_DISTANCE,
                                                      MAX_HAND_DISTANCE)
                middle_threshold = calculate_threshold(BASE_THRESHOLD_MIDDLE, hand_distance, MIN_HAND_DISTANCE,
                                                       MAX_HAND_DISTANCE)
                ring_threshold = calculate_threshold(BASE_THRESHOLD_RING, hand_distance, MIN_HAND_DISTANCE,
                                                     MAX_HAND_DISTANCE)
                pinky_threshold = calculate_threshold(BASE_THRESHOLD_PINKY, hand_distance, MIN_HAND_DISTANCE,
                                                      MAX_HAND_DISTANCE)

                # Calcola la posizione delle dita (usiamo le punte delle dita e il palmo)
                finger_positions = [
                    (landmarks[4], landmarks[0]),  # Pollice
                    (landmarks[8], landmarks[0]),  # Indice
                    (landmarks[12], landmarks[0]),  # Medio
                    (landmarks[16], landmarks[0]),  # Anulare
                    (landmarks[20], landmarks[0])  # Mignolo
                ]

                finger_states = []
                thresholds = [thumb_threshold, index_threshold, middle_threshold, ring_threshold, pinky_threshold]

                for (tip, base), threshold in zip(finger_positions, thresholds):
                    distance = np.sqrt((tip.x - base.x) ** 2 + (tip.y - base.y) ** 2)
                    if distance < threshold:
                        finger_states.append('1')  # Dito piegato
                    else:
                        finger_states.append('0')  # Dito aperto

                # Costruisci la stringa di uscita e aggiungi il carattere di fine riga
                output_string = ''.join(finger_states) + '\n'
                ser.write(output_string.encode())  # Invia la stringa con il carattere di fine riga
                print(f"Posizioni delle dita (dal pollice al mignolo): {output_string}")  # Stampa la stringa in uscita

                # Aggiungi un breve ritardo per evitare di inviare dati troppo velocemente
                time.sleep(0.05)

    cv2.imshow('Frame', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
ser.close()
