#!/usr/bin/env python

import rospy
import numpy as np
import matplotlib.pyplot as plt
from std_msgs.msg import String
import threading
from matplotlib.colors import Normalize

# Variabile globale per la matrice dei dati
distance_matrix = np.zeros((8, 8))

def distance_callback(data):
    global distance_matrix
    
    distance_string = data.data
    distance_values = distance_string.strip().split(',')

    # Rimuovi valori vuoti dalla lista e converti in float
    distance_values = [float(value) for value in distance_values if value]

    if len(distance_values) != 64:  # Assicurati di avere 64 valori per una matrice 8x8
        rospy.logwarn("Invalid number of distance values received.")
        return

    distance_matrix = np.array(distance_values).reshape(8, 8)

def update_heatmap():
    while not rospy.is_shutdown():
        plt.clf()  # Pulisce la figura precedente
        plt.imshow(distance_matrix, cmap='cool', interpolation='nearest',vmin=30, vmax=200)
        plt.colorbar(label='Distance')
        plt.title('Heatmap of Distance Values')
        plt.xlabel('Column')
        plt.ylabel('Row')
        
         # Aggiungi numeri all'interno dei quadrati
        for i in range(distance_matrix.shape[0]):
            for j in range(distance_matrix.shape[1]):
                plt.text(j, i, f'{distance_matrix[i, j]:.0f}', ha='center', va='center', color='black')
                
        plt.pause(0.1)  # Aggiorna il grafico ogni 0.1 secondi

def listener():
    rospy.init_node('heatmap_node', anonymous=True)
    rospy.Subscriber('distance', String, distance_callback)

    t = threading.Thread(target=update_heatmap)
    t.start()

    rospy.spin()

if __name__ == '__main__':
    listener()


