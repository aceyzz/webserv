#!/bin/bash

# Indicate that the content returned is HTML
echo "Content-type: text/html"
echo ""

# Utilize curl to retrieve the price of Bitcoin from the CoinGecko API
response=$(curl -s "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd")

# Extract the price of Bitcoin using grep and cut
price=$(echo $response | grep -o '"usd":[0-9]*' | cut -d ':' -f 2)

# Generate the HTML page with modern styling
echo "<!DOCTYPE html>"
echo "<html>"
echo "<body>"
echo "<div class='container'>"
echo "<div class='content'>"
echo "<h1>Current Bitcoin Price</h1>"
echo "<h2>\$${price}</h2>"
echo "</div>"
echo "</div>"
echo "</body>"
echo "</html>"
