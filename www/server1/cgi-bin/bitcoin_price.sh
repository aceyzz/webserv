#!/bin/bash

# Indiquer que le contenu retourné est du HTML
echo "Content-type: text/html"
echo ""

# Utiliser curl pour récupérer le prix du Bitcoin depuis l'API de CoinGecko
response=$(curl -s "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd")

# Extraire le prix du Bitcoin en utilisant grep et cut
price=$(echo $response | grep -o '"usd":[0-9]*' | cut -d ':' -f 2)

# Afficher le prix dans un format HTML
echo "<div>Prix actuel du Bitcoin : \$${price}</div>"

