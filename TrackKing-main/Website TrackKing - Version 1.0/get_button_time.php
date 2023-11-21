<?php
// Datenbankverbindung herstellen (basierend auf Ihrer Konfiguration)
include 'db_config.php';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Überprüfen, ob der erforderliche Parameter button_name im POST vorhanden ist
    if (isset($_POST['button_name'])) {
        $buttonName = $_POST['button_name'];

        // Beispiel für den PHP-Code, um die verbleibende Zeit zu berechnen
        $currentTime = date('Y-m-d H:i:s');
        $sql = "SELECT click_timestamp FROM buttons WHERE button_name = '$buttonName' ORDER BY click_timestamp DESC LIMIT 1";
        $result = $conn->query($sql);

        if ($result) {
            if ($result->num_rows > 0) {
                $row = $result->fetch_assoc();
                $clickTimestamp = $row['click_timestamp'];

                // Berechne die verbleibende Zeit in Sekunden (5 Minuten - vergangene Zeit seit dem Klick)
                $remainingTime = 5 * 60 - (strtotime($currentTime) - strtotime($clickTimestamp));

                // Wenn die verbleibende Zeit positiv ist, starte den Timer
                if ($remainingTime > 0) {
                    // Starte den Timer mit der verbleibenden Zeit
                    echo "$remainingTime";
                }

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