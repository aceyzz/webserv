#!/bin/bash

# Vérification du nombre d'arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <extension> <taille_mo>"
    exit 1
fi

# Récupération des arguments
extension=$1
taille_mo=$2

# Calcul de la taille en octets
taille_octets=$((taille_mo * 1024 * 1024))

# Nom du fichier
nom_fichier="fichier_cree.${extension}"

# Création du fichier avec la taille spécifiée dans le répertoire de travail actuel
dd if=/dev/zero of="$nom_fichier" bs=1 count=0 seek="$taille_octets" > /dev/null 2>&1

# Confirmation de la création du fichier
echo "Le fichier '${nom_fichier}' de ${taille_mo} Mo a été créé."

