Author: PETIT Alloys
# Serveur_Web_Sockets
Il s'agit d'un serveur web écrit en C qui répond aux requêtes d'un navigateur grâce à des sockets en AF_INET TCP.
Ce serveur stocke dans un fichier local (log_file.txt) toutes les requêtes reçues (l’adresse de la machine du client, la date et le nom du fichier demandé).
Ce serveur écoute sur 2 ports donnés en argument sur la ligne de commande. Il répond normalement aux requêtes http sur le premier port, et retourne son fichier de log si un client s’y connecte par le second port. 

Ce programme ne prend pas en considération les proxys, ainsi si on utilise un navigateur avec un proxy, le serveur ne répondra pas.

-Compiler le fichier serveur.c

-L'exécuter avec en 1er argument le numéro de port pour répondre aux requêtes, et en second argument le numéro de port pour envoyer le fichier de logs
Exemple: ./serveur 7000 7001

Si on veut se connecter au serveur, et que son port est 7000, il faut taper la ligne suivante dans le navigateur:
http://localhost:7000/

Si on veut par exemple demander un fichier file.txt:
http://localhost:7000/file.txt
