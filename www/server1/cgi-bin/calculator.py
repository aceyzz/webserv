#!/usr/bin/env python3
import os
import sys
from urllib.parse import parse_qs

def read_stdin():
    try:
        input_stream = sys.stdin.read()  # Lire tout le contenu de stdin
        return input_stream
    except Exception as e:
        return ""

def main():
    # Lire les donnees du formulaire
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))

    if content_length > 0:
        post_data = read_stdin()

        if post_data:
            form = parse_qs(post_data)

            left_operand = form.get('leftOperand', [None])[0]
            operator = form.get('operator', [None])[0]
            right_operand = form.get('rightOperand', [None])[0]

            # Nettoyer les valeurs d'entree
            left_operand = left_operand.strip() if left_operand else None
            operator = operator.strip() if operator else None
            right_operand = right_operand.strip() if right_operand else None

            # Verifier que les operandes ne sont pas None
            if left_operand is None or operator is None or right_operand is None:
                result = 'Erreur : Donnees du formulaire manquantes'
            else:
                try:
                    # Conversion des operandes en nombres entiers ou flottants
                    left_operand = float(left_operand) if '.' in left_operand else int(left_operand)
                    right_operand = float(right_operand) if '.' in right_operand else int(right_operand)

                    # Calcul en fonction de l'operateur
                    if operator == '+':
                        result = left_operand + right_operand
                    elif operator == '-':
                        result = left_operand - right_operand
                    elif operator == '*':
                        result = left_operand * right_operand
                    elif operator == '/':
                        result = left_operand / right_operand if right_operand != 0 else 'Erreur : Division par zero'
                    else:
                        result = f'Erreur : Operateur invalide ({operator})'
                except ValueError:
                    result = 'Erreur : Valeur incorrecte'
        else:
            result = 'Erreur : Problème lors de la lecture des donnees'
    else:
        result = 'Erreur : Pas de donnees envoyees'

    # Affichage des resultats
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
            <div class="result">Resultat : {result}</div>
        </div>
    </body>
    </html>
    """)

if __name__ == "__main__":
    main()
