<?php
// Set the default timezone
date_default_timezone_set('UTC');

// Log file path
$logFile = './debug.log';

// Function to log messages to the debug log file
function logMessage($message) {
    global $logFile;
    $timestamp = date('Y-m-d H:i:s');
    file_put_contents($logFile, "[$timestamp] $message\n", FILE_APPEND);
}

// Function to create a dark mode HTML response
function createHtmlResponse($message, $success = true) {
    $status = $success ? "Success" : "Error";
    return "
    <!DOCTYPE html>
    <html lang=\"en\">
    <head>
        <meta charset=\"UTF-8\">
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
        <title>File Upload $status</title>
        <style>
            body {
                background-color: #121212;
                color: #ffffff;
                font-family: Arial, sans-serif;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                margin: 0;
            }
            .container {
                text-align: center;
                padding: 20px;
                border: 1px solid #333;
                border-radius: 10px;
                background-color: #1e1e1e;
            }
            .container h1 {
                color: #4caf50;
            }
            .container p {
                color: #ffffff;
            }
        </style>
    </head>
    <body>
        <div class=\"container\">
            <h1>$status</h1>
            <p>$message</p>
        </div>
    </body>
    </html>";
}

// Read the raw POST data from stdin
$rawData = file_get_contents('php://input');
logMessage("Raw POST data length: " . strlen($rawData));

// Get the boundary from the content type header
$contentType = getenv('CONTENT_TYPE');
if (preg_match('/boundary=(.*)$/', $contentType, $matches)) {
    $boundary = $matches[1];
    logMessage("Boundary found: $boundary");

    // Split the raw data by the boundary
    $parts = preg_split('/--' . preg_quote($boundary, '/') . '/', $rawData);
    foreach ($parts as $part) {
        if (empty($part)) {
            continue;
        }

        // Parse each part
        if (preg_match('/Content-Disposition:.*name="fileToUpload"; filename="(.*)"/', $part, $matches)) {
            $filename = $matches[1];
            logMessage("Filename found: $filename");

            // Extract the file data
            $fileDataStart = strpos($part, "\r\n\r\n") + 4;
            $fileDataEnd = strrpos($part, "\r\n");
            $fileData = substr($part, $fileDataStart, $fileDataEnd - $fileDataStart);

            // Generate a new file name
            $timestamp = time();
            $newFileName = "user_{$timestamp}_{$filename}";
            $destination = './www/server1/images/' . $newFileName;

            // Save the file
            if (file_put_contents($destination, $fileData) !== false) {
                logMessage("File uploaded successfully: $newFileName");
                echo createHtmlResponse("File uploaded successfully: $newFileName");
                exit;
            } else {
                logMessage("Failed to save uploaded file.");
                echo createHtmlResponse("Failed to save uploaded file.", false);
                exit;
            }
        }
    }
    logMessage("No valid file part found in the request.");
    echo createHtmlResponse("No valid file part found in the request.", false);
} else {
    logMessage("No boundary found in the content type.");
    echo createHtmlResponse("No boundary found in the content type.", false);
}
?>
