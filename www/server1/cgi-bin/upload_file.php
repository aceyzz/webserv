<?php
$targetDir = "../upload/";
$targetFile = $targetDir . basename($_FILES["fileToUpload"]["name"]);

if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $targetFile)) {
    echo "Le fichier a été téléchargé avec succès.";
} else {
    echo "Erreur lors du téléchargement du fichier.";
}
?>
