#!/usr/bin/env python3
# Genere une grosse reponse (20 Mo) sans dependre d'un gros fichier
# commite dans le repo. Sert a tester la resilience du serveur quand un
# client se deconnecte brutalement en plein envoi d'une reponse.
import sys

sys.stdout.write("Content-Type: text/plain\r\n\r\n")
sys.stdout.write("A" * 20000000)
