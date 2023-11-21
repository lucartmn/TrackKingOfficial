<?php
session_start();

$login_error = ""; // Initialisieren Sie die Fehlermeldung

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    require_once("db_config.php");

    $username = $_POST["username"];
    $password = $_POST["password"];

    // Passwort aus der Datenbank abrufen
    $password_sql = "SELECT password FROM benutzer WHERE username = '$username'";
    $password_result = $conn->query($password_sql);

    if ($password_result->num_rows == 1) {
        $row = $password_result->fetch_assoc();
        $hashed_password = $row["password"];

        // Passwort überprüfen
        if (password_verify($password, $hashed_password)) {
            // Anmeldedaten sind korrekt, leite den Benutzer zur geschützten Seite weiter
            $_SESSION["loggedin"] = true;
            header("Location: tracker.php");
            exit;
        } else {
            // Anmeldedaten sind falsch, zeige Fehlermeldung
            $login_error = "Benutzername oder Passwort falsch!";
        }
    } else {
        // Benutzer existiert nicht
        $login_error = "Benutzername existiert nicht.";
    }

    $conn->close();
}
?>      