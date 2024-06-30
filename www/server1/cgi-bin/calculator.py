#!/usr/bin/env python3
import os
import sys
from io import BytesIO
from urllib.parse import parse_qs

def main():
    # Simuler les variables d'environnement pour le test local
    os.environ['REQUEST_METHOD'] = 'POST'
    os.environ['QUERY_STRING'] = ''
    os.environ['CONTENT_TYPE'] = 'application/x-www-form-urlencoded'
    input_data = b'leftOperand=11&operator=%2B&rightOperand=5'
    os.environ['CONTENT_LENGTH'] = str(len(input_data))

    # Simuler l'entrée standard pour les données du formulaire
    sys.stdin = BytesIO(input_data)

    # Lire les données du formulaire
    content_length = int(os.environ['CONTENT_LENGTH'])
    post_data = sys.stdin.read(content_length).decode('utf-8')
    form = parse_qs(post_data)

    left_operand = form.get('leftOperand', [None])[0]
    operator = form.get('operator', [None])[0]
    right_operand = form.get('rightOperand', [None])[0]

    # Nettoyer les valeurs d'entrée
    left_operand = left_operand.strip() if left_operand else None
    operator = operator.strip() if operator else None
    right_operand = right_operand.strip() if right_operand else None

    # Vérifier que les opérandes ne sont pas None
    if left_operand is None or operator is None or right_operand is None:
        result = 'Erreur : Données du formulaire manquantes'
    else:
        try:
            # Conversion des opérandes en nombres entiers ou flottants
            left_operand = float(left_operand) if '.' in left_operand else int(left_operand)
            right_operand = float(right_operand) if '.' in right_operand else int(right_operand)

            # Calcul en fonction de l'opérateur
            if operator == '+':
                result = left_operand + right_operand
            elif operator == '-':
                result = left_operand - right_operand
            elif operator == '*':
                result = left_operand * right_operand
            elif operator == '/':
                result = left_operand / right_operand if right_operand != 0 else 'Erreur : Division par zéro'
            else:
                result = f'Erreur : Opérateur invalide ({operator})'
        except ValueError:
            result = 'Erreur : Valeur incorrecte'

    # Affichage des résultats
    print("Content-type: text/html\n")
    print(f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Calculator Result</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                background-color: #333333;
                color: #ffffff;
            }}
            h1 {{
                color: #ffffff;
                text-align: center;
            }}
            .result-container {{
                width: 300px;
                margin: 0 auto;
                padding: 20px;
                background-color: #222222;
                border: 1px solid #cccccc;
                border-radius: 5px;
                box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
            }}
            .result {{
                font-size: 24px;
                text-align: center;
                margin-bottom: 20px;
            }}
        </style>
    </head>
    <body>
        <div class="result-container">
            <h1>Calculator Result</h1>
            <div class="result">Résultat : {result}</div>
        </div>
    </body>
    </html>
    """)

if __name__ == "__main__":
    main()
