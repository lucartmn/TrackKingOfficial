<?php
// Lade die Datenbankverbindungsinformationen aus db_config.php
require_once('db_config.php');

// Verbindung zur Datenbank herstellen
$conn = new mysqli($servername, $username, $password, $dbname);

// Überprüfen, ob die Verbindung erfolgreich hergestellt wurde
if ($conn->connect_error) {
    http_response_code(500); // Interner Serverfehler
    echo json_encode(["error" => "Verbindung zur Datenbank fehlgeschlagen: " . $conn->connect_error]);
    exit;
}

// SQL-Abfrage zum Abrufen der neuesten GPS-Koordinaten aus der Datenbank
$sql = "SELECT latitude, longitude, speed, altitude, timestamp_column FROM gps_coordinates ORDER BY nr DESC LIMIT 1";
$result = $conn->query($sql);

if ($result === false) {
    http_response_code(500); // Interner Serverfehler
    echo json_encode(["error" => "Datenbankabfrage fehlgeschlagen: " . $conn->error]);
    $conn->close();
    exit;
}

if ($result->num_rows > 0) {
    $row = $result->fetch_assoc();
    $data = [
        "latitude" => $row["latitude"],
        "longitude" => $row["longitude"],
        "speed" => $row["speed"],
        "altitude" => $row["altitude"],
        "timestamp_column" => $row["timestamp_column"]
    ];
    echo json_encode($data);
} else {
    http_response_code(404); // Nicht gefunden
    echo json_encode(["error" => "Keine GPS-Koordinaten gefunden."]);
}

$conn->close();
?>