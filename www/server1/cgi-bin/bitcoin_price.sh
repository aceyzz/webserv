#!/bin/bash

# Indicate that the content returned is HTML
echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
echo ""

# Utilize curl to retrieve the price of Bitcoin from the CoinGecko API
response=$(curl -s "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd")

# Extract the price of Bitcoin using grep and cut
price=$(echo $response | grep -o '"usd":[0-9]*' | cut -d ':' -f 2)

# Generate the HTML page with modern styling
echo "<!DOCTYPE html>"
echo "<html>"
echo "<head>"
echo "<style>"
echo "body { background-color: #222; color: #fff; font-family: Arial, sans-serif; }"
echo ".container { display: flex; justify-content: center; align-items: center; height: 100vh; }"
echo ".content { text-align: center; }"
echo "h1 { font-size: 36px; margin-bottom: 20px; }"
echo "h2 { font-size: 24px; }"
echo "</style>"
echo "</head>"
echo "<body>"
echo "<div class='container'>"
echo "<div class='content'>"
echo "<h1>Current Bitcoin Price</h1>"
echo "<h2>\$${price}</h2>"
echo "</div>"
echo "</div>"
echo "</body>"
echo "</html>"
