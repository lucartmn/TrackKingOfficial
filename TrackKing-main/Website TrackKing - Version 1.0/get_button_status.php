<?php
// Datenbankverbindung herstellen (basierend auf Ihrer Konfiguration)
include 'db_config.php';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Überprüfen, ob der erforderliche Parameter button_name im POST vorhanden ist
    if (isset($_POST['button_name'])) {
        $buttonName = $_POST['button_name'];

        // SQL-Abfrage, um den letzten Wert für den angegebenen Button-Namen zu erhalten
        $sql = "SELECT button_status FROM buttons WHERE button_name = '$buttonName' ORDER BY nr DESC LIMIT 1";
        $result = $conn->query($sql);

        if ($result) {
            if ($result->num_rows > 0) {
                $row = $result->fetch_assoc();
                $currentStatus = $row['button_status'];

                // Den aktuellen Button-Status als String zurückgeben
                echo "$currentStatus";
            } else {
                echo "Der Button '$buttonName' existiert nicht in der Datenbank.";
            }
        } else {
            echo "Die Datenbank hat folgenden Fehler geworfen: " . $conn->error;
        }
    } else {
        echo "Parameter 'button_name' fehlt im POST.";
    }
} else {
    echo "Ungültige Anfrage. Es werden nur POST-Anfragen akzeptiert.";
}

$conn->close();
?>
