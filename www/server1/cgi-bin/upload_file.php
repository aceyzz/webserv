<?php
$targetDir = "./www/server1/images/";
$logFile = "./debug.log";

function writeToLog($message) {
    global $logFile;
    file_put_contents($logFile, $message . "\n", FILE_APPEND);
}

writeToLog("Script started.");

echo("Content-type: text/html\n"); // Ajoutez cette ligne pour définir le Content-type

echo "<!DOCTYPE html><html><head><meta charset='utf-8'>\n";
echo "<style>
    body {
        font-family: Arial, sans-serif;
        background-color: #333;
        color: #fff;
    }
    h1 {
        color: #0099cc;
    }
</style>";
echo "</head>\n<body>";

// Lire les données depuis stdin
$stdin = fopen('php://stdin', 'r');
$rawPostData = '';
while ($chunk = fread($stdin, 8192)) {
    $rawPostData .= $chunk;
}
fclose($stdin);

writeToLog("Raw POST data received. Length: " . strlen($rawPostData));

// Extraire le nom du fichier
$filename = '';
if (preg_match('/Content-Disposition: form-data; name="fileToUpload"; filename="([^"]+)"/', $rawPostData, $matches)) {
    $filename = $matches[1];
    writeToLog("Filename found: " . $filename);
} else {
    writeToLog("Filename not found.");
}

if ($filename) {
    // Supprimer les 4 premières lignes et la dernière ligne
    $lines = explode("\r\n", $rawPostData);
    for ($i = 0; $i < 4; $i++) {
        array_shift($lines);
    }
    array_pop($lines);
    
    // Rejoindre les lignes restantes pour obtenir le contenu du fichier
    $fileContent = implode("\r\n", $lines);
    
    // Créer le fichier et y écrire le contenu
    $targetFile = $targetDir . basename($filename);
    if (file_put_contents($targetFile, $fileContent) !== false) {
        echo "<h1>Le fichier a été téléchargé avec succès : $filename</h1>";
        writeToLog("File uploaded successfully: " . $filename);
    } else {
        echo "<h1>Erreur lors du téléchargement du fichier : $filename</h1>";
        writeToLog("Error uploading file: " . $filename);
    }
} else {
    echo "<h1>Aucun fichier n'a été téléchargé.</h1>";
    writeToLog("No files uploaded.");
}

echo "</body></html>";
writeToLog("Script ended.");
?>
