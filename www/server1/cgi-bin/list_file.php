#!/usr/bin/php-cgi
<?php
$dir = "./www/server1/images";
$base_url = "/../images/"; // Base URL relative to the web server root

if (is_dir($dir)) {
    echo "Content-type: text/html\r\n\r\n"; // Set the content type to HTML
    $files = scandir($dir);
    echo "<ul style='list-style-type: none;'>"; // Use unordered list without bullet points
    foreach ($files as $file) {
        if ($file != "." && $file != "..") {
			echo "<li><a href='".$base_url.$file."' target='_blank' style='text-decoration: none; color: white;'>".$file."</a></li>\n";
        }
    }
    echo "</ul>";
} else {
    echo "Content-type: text/plain\r\n\r\n"; // Set the content type to plain text
    echo "Directory does not exist.";
}
?>
