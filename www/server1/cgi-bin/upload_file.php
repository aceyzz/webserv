<?php
$targetDir = "./www/server1/images/";
echo("text/html\n"); // Ajoutez cette ligne pour définir le Content-type

echo "<!DOCTYPE html><html><head><meta charset='utf-8'>\n";
echo "<style>\n
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

if (isset($_FILES["fileToUpload"])) {
	$uploadedFile = $_FILES["fileToUpload"]["tmp_name"];
	$uploadedFileName = $_FILES["fileToUpload"]["name"];
	$extension = pathinfo($uploadedFileName, PATHINFO_EXTENSION);
	$uniqueId = uniqid();
	$targetFile = $targetDir . $uniqueId . "_" . $uploadedFileName;
	echo "<h1>Uploaded file: $uploadedFileName</h1>";
	if (move_uploaded_file($uploadedFile, $targetFile)) {
		echo "<h1>Le fichier a été téléchargé avec succès.</h1>";
	} else {
		echo "<h1>Erreur lors du téléchargement du fichier.</h1>";
	}
} else {
	echo "<h1>Aucun fichier n'a été téléchargé.</h1>";
}
echo "</body></html>";
?>
