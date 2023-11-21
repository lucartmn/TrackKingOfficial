<?php
ini_set('display_errors', 1);
error_reporting(E_ALL);

$servername = "localhost"; // MySQL Servername
$username = "track-king"; // MySQL Benutzername
$password = "Bmk0MkDarIlgIkTz"; // MySQL Passwort
$dbname = "track-king"; // MySQL Datenbankname

// Verbindung zur MySQL-Datenbank herstellen
$conn = new mysqli($servername, $username, $password, $dbname);

// Überprüfen, ob die Verbindung erfolgreich hergestellt wurde
if ($conn->connect_error) {
    die("Verbindung zur MySQL-Datenbank fehlgeschlagen: " . $conn->connect_error);
}
?>
