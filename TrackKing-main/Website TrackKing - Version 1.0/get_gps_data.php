<?php
// Lade die Datenbankverbindungsinformationen aus db_config.php
require_once('db_config.php');

// Definiere deinen geheimen Schlüssel
$secretKey = '5zWq43!#420';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Überprüfen, ob der übergebene Schlüssel korrekt ist
    if (isset($_POST['key']) && $_POST['key'] === $secretKey) {
        // Überprüfen, ob latitude und longitude im POST-Daten vorhanden sind
        if (isset($_POST['latitude']) && isset($_POST['longitude']) && isset($_POST['speed']) && isset($_POST['altitude'])) {
            $latitude = $_POST['latitude'];
            $longitude = $_POST['longitude'];
            $speed = $_POST['speed'];
            $altitude = $_POST['altitude'];

            // Holen Sie sich den aktuellen Zeitstempel (Systemzeit)
            $currentTimestamp = date('Y-m-d H:i:s');

            // Verbindung zur Datenbank herstellen
            $conn = new mysqli($servername, $username, $password, $dbname);

            // Überprüfen, ob die Verbindung erfolgreich hergestellt wurde
            if ($conn->connect_error) {
                http_response_code(500); // Interner Serverfehler
                echo "Verbindung zur Datenbank fehlgeschlagen: " . $conn->connect_error;
                exit;
            }

            // SQL-Abfrage zum Einfügen der GPS-Koordinaten und des Zeitstempels in die Datenbank
            $sql = "INSERT INTO gps_coordinates (latitude, longitude, speed, altitude, timestamp_column) VALUES (?, ?, ?, ?, ?)";

            // Vorbereiten und Ausführen der SQL-Abfrage
            $stmt = $conn->prepare($sql);

            if ($stmt === false) {
                http_response_code(500); // Interner Serverfehler
                echo "SQL-Vorbereitung fehlgeschlagen: " . $conn->error;
                $conn->close();
                exit;
            }

            // Binden der Parameter und Ausführen der SQL-Abfrage
            $stmt->bind_param("dddds", $latitude, $longitude, $speed, $altitude, $currentTimestamp);

            if ($stmt->execute() === false) {
                http_response_code(500); // Interner Serverfehler
                echo "SQL-Ausfuehrung fehlgeschlagen: " . $stmt->error;
                $stmt->close();
                $conn->close();
                exit;
            }

            $stmt->close();
            $conn->close();

            // Geben Sie eine Erfolgsmeldung zurück
            echo "GPS-Koordinaten erfolgreich in die Datenbank eingefuegt.";

            // Nach dem Einfügen neuer Daten, rufe gps_data_remove_old.php auf, um alte Daten zu löschen
            $removeOldDataUrl = 'http://lets-stream.eu/gps_data_remove_old.php'; // Passe die URL entsprechend deiner Anwendung an

            // Verwende cURL, um die Datei aufzurufen (kann auch mit anderen Methoden gemacht werden)
            $ch = curl_init($removeOldDataUrl);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
            $response = curl_exec($ch);
            curl_close($ch);

            // Überprüfe die Antwort und handle entsprechend
            if ($response === false) {
                // Die Abfrage war nicht erfolgreich, handle den Fehler hier
                echo "\n Fehler beim Aufrufen von gps_data_remove_old.php: " . curl_error($ch);
            } else {
                // Die Abfrage war erfolgreich, handle die Antwort hier
                echo "\n Und Die Antwort von gps_data_remove_old.php lautet: " . $response;
            }
        } else {
            http_response_code(400); // Ungültige Anforderung
            echo "Ungueltige Anforderungsdaten: " . json_encode($_POST);
            }
    } else {
        http_response_code(401); // Nicht autorisiert
        echo "Nicht autorisiert";
        }
} else {
    http_response_code(405); // Methode nicht erlaubt
    echo "Method not allowed";
}
?>
