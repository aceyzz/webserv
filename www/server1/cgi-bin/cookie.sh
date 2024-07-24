#!/bin/bash
# Recupère le HTTP_COOKIE des envs, et le print
echo "text/html"
echo ""
if [ -z "$HTTP_COOKIE" ]; then
	echo "Pas de cookie pour toi"
else
	echo "$HTTP_COOKIE"
fi
