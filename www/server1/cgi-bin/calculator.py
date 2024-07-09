#!/usr/bin/env python3
import os
import sys
import re

def read_stdin(content_length):
	"""Read specified content length from stdin."""
	try:
		if content_length > 0:
			stdin_data = sys.stdin.buffer.read(content_length)
			return stdin_data.decode('utf-8')  # Convert binary data to string
		return ''
	except Exception as e:
		print(f"Error reading stdin: {str(e)}", file=sys.stderr)
		return ""

def extract_form_data(data):
	"""Extracts form data from the POST body using regular expressions."""
	results = {}
	patterns = {
		'leftOperand': r'leftOperand=([^&]+)',
		'operator': r'operator=([^&]+)',
		'rightOperand': r'rightOperand=([^&]+)'
	}

	for key, pattern in patterns.items():
		match = re.search(pattern, data)
		if match:
			results[key] = match.group(1).replace('+', ' ')  # Replace + with space for URL encoding

	return results

def main():
	content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	post_data = read_stdin(content_length)

	form_data = extract_form_data(post_data)
	left_operand = form_data.get('leftOperand')
	operator = form_data.get('operator')
	right_operand = form_data.get('rightOperand')

	if left_operand and operator and right_operand:
		try:
			left_operand = float(left_operand)
			right_operand = float(right_operand)
			if operator == '%2B':  # URL encoded '+'
				result = left_operand + right_operand
			elif operator == '-':
				result = left_operand - right_operand
			elif operator == '*':
				result = left_operand * right_operand
			elif operator == '%2F':  # URL encoded '/'
				result = left_operand / right_operand if right_operand != 0 else 'Error: Division by zero'
			else:
				result = 'Error: Invalid operator'
		except ValueError:
			result = 'Error: Invalid input'
	else:
		result = 'Error: Missing form data'

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
