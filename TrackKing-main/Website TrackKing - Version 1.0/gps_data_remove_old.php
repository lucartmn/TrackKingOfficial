<?php
// Lade die Datenbankverbindungsinformationen aus db_config.php
require_once('db_config.php');

// Verbindung zur Datenbank herstellen
$conn = new mysqli($servername, $username, $password, $dbname);

// Ueberpruefen, ob die Verbindung erfolgreich hergestellt wurde
if ($conn->connect_error) {
    http_response_code(500); // Interner Serverfehler
    echo "Verbindung zur Datenbank fehlgeschlagen: " . $conn->connect_error;
    exit;
}

// Name der Datenbank und Tabelle
$databaseName = 'TrackKing';
$tableName = 'gps_coordinates';

// Berechne das Datum vor 7 Tagen
$cutOffDate = date('Y-m-d', strtotime("-7 days"));

// SQL-Abfrage zum Loeschen der alten GPS-Koordinaten
$sqlDeleteOldRecords = "DELETE FROM $tableName WHERE timestamp_column <= '$cutOffDate'";

if ($conn->query($sqlDeleteOldRecords) === true) {
    echo "Alte GPS-Koordinaten erfolgreich geloescht.";
} else {
    http_response_code(500); // Interner Serverfehler
    echo "Loeschen der alten GPS-Koordinaten fehlgeschlagen: " . $conn->error;
}

$conn->close();
?>
