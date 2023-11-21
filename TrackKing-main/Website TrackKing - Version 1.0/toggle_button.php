<?php
// Datenbankverbindung herstellen (basierend auf Ihrer Konfiguration)
include 'db_config.php';

// Hier weiteren Code einfügen, um sicherzustellen, dass der Benutzer eingeloggt ist

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Überprüfen, ob der erforderliche Parameter button_name im POST vorhanden ist
    if (isset($_POST['button_name'])) {
        $buttonName = $_POST['button_name'];

        // Aktuellen Button-Status aus der Datenbank abrufen oder Standardwert 0 nehmen
        $sql = "SELECT button_status FROM buttons WHERE button_name = '$buttonName' ORDER BY nr DESC LIMIT 1";
        $result = $conn->query($sql);

        $currentStatus = 0; // Standardwert
        if ($result->num_rows > 0) {
            $row = $result->fetch_assoc();
            $currentStatus = $row['button_status'];
        }

        // Neuen Button-Status invertieren (1 wird zu 0, 0 wird zu 1)
        $newStatus = $currentStatus == 1 ? 0 : 1;

        $currentTime = date('Y-m-d H:i:s');
        // Button-Status in der Datenbank aktualisieren
        $updateSql = "INSERT INTO buttons (button_name, button_status, click_timestamp) VALUES ('$buttonName', $newStatus, '$currentTime')";
        $conn->query($updateSql);

        // Alten Button-Status in der Datenbank löschen
        $deleteSql = "DELETE FROM buttons WHERE button_name = '$buttonName' AND button_status = $currentStatus";
        $conn->query($deleteSql);

        // Den neuen Button-Status als JSON zurückgeben
        echo json_encode(['buttonStatus' => $newStatus]);
    } else {
        echo json_encode(['error' => 'Parameter "button_name" fehlt im POST.']);
    }
} else {
    echo json_encode(['error' => 'Ungültige Anfrage. Es werden nur POST-Anfragen akzeptiert.']);
}

$conn->close();
?>
