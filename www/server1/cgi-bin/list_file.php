<?php
$dir = "../upload/";
$files = scandir($dir);
echo "<ul>";
foreach ($files as $file) {
    if ($file != "." && $file != "..") {
        echo "<li><a href='".$dir.$file."' download>".$file."</a></li>";
    }
}
echo "</ul>";
?>
