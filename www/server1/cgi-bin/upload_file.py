#!/usr/bin/env python3
import os
import sys
import time
import re
import hashlib
from datetime import datetime

# Set the log file path
log_directory = os.path.join(os.getcwd(), 'log')
os.makedirs(log_directory, exist_ok=True)
log_file = os.path.join(log_directory, 'cgi.log')

def log_message(message):
	"""Function to log messages to the debug log file."""
	timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
	with open(log_file, 'a+') as file:
		file.write(f'[{timestamp}] {message}\n')

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

def get_boundary(content_type):
    """Function to extract the boundary from the content type header."""
    match = re.search(r'boundary=(.*)$', content_type)
    if match:
        return match.group(1)
    return None

def parse_multipart_data(raw_data, boundary):
    """Function to parse multipart/form-data."""
    boundary = bytes('--' + boundary, 'utf-8')
    end_boundary = boundary + b'--'
    parts = raw_data.split(boundary)

    for part in parts:
        if not part.strip() or part.strip() == b'--':
            continue
        matches = re.search(rb'Content-Disposition: form-data; name="fileToUpload"; filename="([^"]+)"', part)
        if matches:
            filename = matches.group(1).decode('utf-8')
            part = re.sub(rb'Content-Disposition:.*filename="[^"]+"', b'', part)
            part = re.sub(rb'Content-Type:.*\s', b'', part, 1)
            file_content = part.strip()
            return filename, file_content
    return None, None

def calculate_md5(file_data):
    """Calculate the MD5 checksum of the given file data."""
    md5_hash = hashlib.md5()
    md5_hash.update(file_data)
    return md5_hash.hexdigest()

def save_file(filename, file_data):
    """Function to save the file data to the specified directory and check data integrity."""
    timestamp = int(time.time())
    new_file_name = f"user_{timestamp}_{filename}"
    destination = f'./www/server1/images/{new_file_name}'
    try:
        # Calculate MD5 before saving
        md5_before_saving = calculate_md5(file_data)
        log_message(f"MD5 before saving: {md5_before_saving}")

        with open(destination, 'wb') as file:
            file.write(file_data)
        
        # Read back the file and calculate MD5 to verify integrity after saving
        with open(destination, 'rb') as file:
            saved_data = file.read()
        md5_after_saving = calculate_md5(saved_data)
        log_message(f"MD5 after saving: {md5_after_saving}")

        # Check if MD5 checksums match
        if md5_before_saving == md5_after_saving:
            log_message(f"File uploaded successfully: {new_file_name}")
            return new_file_name
        else:
            log_message("MD5 mismatch: File may be corrupted.")
            return None

    except IOError as e:
        log_message(f"Failed to save uploaded file: {str(e)}")
        return None

# Main script execution
content_type = os.getenv('CONTENT_TYPE')
raw_data = get_raw_post_data()
log_message(f"Raw POST data length: {len(raw_data)}")

boundary = get_boundary(content_type)
if boundary:
    log_message(f"Boundary found: {boundary}")
    filename, file_data = parse_multipart_data(raw_data, boundary)

    if filename and file_data:
        saved_file_name = save_file(filename, file_data)
        if saved_file_name:
            print(create_html_response(f"File uploaded successfully: {saved_file_name}"))
        else:
            print(create_html_response("Failed to save uploaded file.", False))
    else:
        log_message("No valid file part found in the request.")
        print(create_html_response("No valid file part found in the request.", False))
else:
    log_message("No boundary found in the content type.")
    print(create_html_response("No boundary found in the content type.", False))

