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

// Function to read raw POST data from stdin
function getRawPostData() {
    $rawData = '';
    while ($chunk = fread(STDIN, 1024)) {
        $rawData .= $chunk;
    }
    return $rawData;
}

// Function to extract the boundary from the content type header
function getBoundary($contentType) {
    if (preg_match('/boundary=(.*)$/', $contentType, $matches)) {
        return $matches[1];
    }
    return null;
}

function parseMultipartData($rawData, $boundary) {
    $boundary = '--' . $boundary;
    $endBoundary = $boundary . '--';
    $parts = explode($boundary, $rawData);

    foreach ($parts as $part) {
        if (empty(trim($part)) || trim($part) == '--') {
            continue;
        }
        if (preg_match('/Content-Disposition: form-data; name="fileToUpload"; filename="([^"]+)"/', $part, $matches)) {
            $filename = $matches[1];
            $part = preg_replace('/Content-Disposition:.*filename="[^"]+"/', '', $part);
            $part = preg_replace('/Content-Type:.*\s/', '', $part, 1);
            $fileContent = trim($part);
            return [$filename, $fileContent];
        }
    }
    return [null, null];
}

// Function to save the file data to the specified directory
function saveFile($filename, $fileData) {
    $timestamp = time();
    $newFileName = "user_{$timestamp}_{$filename}";
    $destination = './www/server1/images/' . $newFileName;
    if (file_put_contents($destination, $fileData) !== false) {
        logMessage("File uploaded successfully: $newFileName");
        return $newFileName;
    } else {
        logMessage("Failed to save uploaded file.");
        return null;
    }
}

// Main script execution
$contentType = getenv('CONTENT_TYPE');
$rawData = getRawPostData();
logMessage("Raw POST data length: " . strlen($rawData));

$boundary = getBoundary($contentType);
if ($boundary) {
    logMessage("Boundary found: $boundary");
    list($filename, $fileData) = parseMultipartData($rawData, $boundary);

    if ($filename && $fileData) {
        $savedFileName = saveFile($filename, $fileData);
        if ($savedFileName) {
            echo createHtmlResponse("File uploaded successfully: $savedFileName");
            exit;
        } else {
            echo createHtmlResponse("Failed to save uploaded file.", false);
            exit;
        }
    } else {
        logMessage("No valid file part found in the request.");
        echo createHtmlResponse("No valid file part found in the request.", false);
        exit;
    }
} else {
    logMessage("No boundary found in the content type.");
    echo createHtmlResponse("No boundary found in the content type.", false);
    exit;
}
?>
