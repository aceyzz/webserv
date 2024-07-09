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

if filename:
    saved_file_name = save_file(filename, file_data)
    if saved_file_name:
        print(create_html_response(f"File uploaded successfully: {saved_file_name}"))
    else:
        print(create_html_response("Failed to save uploaded file.", False))
else:
    log_message("Filename not provided.")
    print(create_html_response("Filename not provided.", False))
