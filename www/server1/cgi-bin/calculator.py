#!/usr/bin/env python3
import cgi
import cgitb; cgitb.enable()  # Pour activer la sortie d'erreur détaillée CGI

def main():
    form = cgi.FieldStorage()
    left_operand = form.getvalue('leftOperand')
    operator = form.getvalue('operator')
    right_operand = form.getvalue('rightOperand')

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

    # Affichage des résultats
    print("Content-type: text/html\n")
    print(f"<html><body><h1>Résultat : {result}</h1></body></html>")

if __name__ == "__main__":
    main()
