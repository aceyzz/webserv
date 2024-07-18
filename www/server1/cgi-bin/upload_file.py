#!/usr/bin/env python3
import os
import sys
import hashlib
import time
from datetime import datetime

def create_html_response(message, success=True):
    """Function to create a dark mode HTML response."""
    status = "Success" if success else "Error"
    return f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>File Upload {status}</title>
        <style>
            body {{
                background-color: #121212;
                color: #ffffff;
                font-family: Arial, sans-serif;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                margin: 0;
            }}
            .container {{
                text-align: center;
                padding: 20px;
                border: 1px solid #333;
                border-radius: 10px;
                background-color: #1e1e1e;
            }}
            .container h1 {{
                color: #4caf50;
            }}
            .container p {{
                color: #ffffff;
            }}
        </style>
    </head>
    <body>
        <div class="container">
            <h1>{status}</h1>
            <p>{message}</p>
        </div>
    </body>
    </html>
    """

def get_raw_post_data():
    """Function to read raw POST data from stdin in binary mode until EOF is encountered."""
    raw_data = b''
    while True:
        chunk = sys.stdin.buffer.read(4096)
        if not chunk:
            break
        raw_data += chunk

    return raw_data

def check_file_data(file_data):
	"""Function to check if the file data is empty."""
	if not file_data:
		print(create_html_response("No file data received.", False))
		sys.exit(1)
	"""Check if there is spaces inside the filename."""
	filename = os.getenv('FILENAME')
	if ' ' in filename:
		print(create_html_response("Filename cannot contain spaces.", False))
		sys.exit(1)
	"""Check the filetype if it a forbidden file type."""
	file_type = filename.split('.')[-1]
	forbidden_file_types = ['php', 'py', 'pl', 'sh', 'rb', 'cgi', 'java', 'jsp', 'asp', 'aspx', 'html', 'htm', 'shtml', 'xhtml', 'xml', 'css', 'js', 'json', 'htaccess', 'conf', 'htpasswd', 'log', 'bak', 'sql', 'sqlite', 'db', 'dbf', 'csv', 'ini', 'sh', 'bat', 'cmd', 'ps1', 'psm1', 'psd1', 'ps1xml', 'psc1', 'pssc', 'msh', 'msh1', 'msh2', 'mshxml', 'msh1xml', 'msh2xml', 'scf', 'lnk', 'inf', 'reg', 'dll', 'vbs', 'vbe', 'js', 'jse', 'ws', 'wsf', 'wsc', 'wsh', 'ps', 'psc1', 'psc2', 'msh', 'msh1', 'msh2', 'mshxml', 'msh1xml', 'msh2xml', 'scf', 'lnk', 'inf', 'reg', 'dll', 'vbs', 'vbe', 'js', 'jse', 'ws', 'wsf', 'wsc', 'wsh', 'ps', 'psc1', 'psc2', 'msh', 'msh1', 'msh2', 'mshxml', 'msh1xml', 'msh2xml', 'scf', 'lnk', 'inf', 'reg', 'dll', 'vbs', 'vbe', 'js', 'jse', 'ws', 'wsf', 'wsc', 'wsh', 'ps', 'psc1', 'psc2', 'msh', 'msh1', 'msh2', 'mshxml', 'msh1xml', 'msh2xml', 'scf', 'lnk', 'inf', 'reg', 'dll', 'vbs', 'vbe', 'js', 'jse', 'ws', 'wsf', 'wsc', 'wsh', 'ps', 'psc1', 'psc2']
	for file_type in forbidden_file_types:
		if file_type in filename:
			print(create_html_response("File type not allowed.", False))
			sys.exit(1)

def save_file(filename, file_data):
    """Function to save the file data to the specified directory."""
    new_file_name = f"user_{int(time.time())}_{filename}"
    destination = f'./www/server1/images/{new_file_name}'
    try:
        with open(destination, 'wb') as file:
            file.write(file_data)

        return new_file_name

    except IOError as e:
        return None

# Main script execution
filename = os.getenv('FILENAME')
file_data = get_raw_post_data()

check_file_data(file_data)

if filename:
    saved_file_name = save_file(filename, file_data)
    if saved_file_name:
        print(create_html_response(f"File uploaded successfully: {saved_file_name}"))
    else:
        print(create_html_response("Failed to save uploaded file.", False))
else:
    print(create_html_response("Filename not provided.", False))
