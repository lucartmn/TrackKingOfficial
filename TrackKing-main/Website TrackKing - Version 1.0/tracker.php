<?php
// Initialisieren Sie die Sitzung
session_start();

// Überprüfen, ob der Benutzer angemeldet ist
if (!isset($_SESSION["loggedin"]) || $_SESSION["loggedin"] !== true) {
    // Umleitung zum Login
    header("Location: login.php");
    exit;
}

// Logout-Logik
if (isset($_GET["logout"]) && $_GET["logout"] == "true") {
    // Sitzung zerstören
    session_destroy();
    $_SESSION = array(); // Optionale: Alle Sitzungsvariablen löschen
    header("Location: login.php");
    exit;
}
?>
<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tracker - TrackKing</title>
    <link rel="apple-touch-icon" sizes="180x180" href="assets/favico/apple-touch-icon.png">
    <link rel="icon" type="image/png" sizes="32x32" href="assets/favico/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="assets/favico/favicon-16x16.png">
    <link rel="manifest" href="assets/favico/site.webmanifest">
    <script src="https://code.jquery.com/jquery-3.6.4.min.js"></script>
    <style>
        /* CSS für die durchgezogene schwarze Leiste oben am Rand */
        .navbar {
            background-color: #000; /* Schwarz */
            color: #FFF;
            padding: 10px 0;
            position: fixed !important;
            top: 0;
            left: 0;
            right: 0;
            z-index: 1000; /* Stellen Sie sicher, dass die Navbar über der Karte liegt */
            display: flex;
            justify-content: space-between; /* Abstand zwischen den Elementen aufteilen */
            align-items: center; /* Vertikal zentrieren */
        }

        /* CSS für das Navbar-Logo */
        .navbar-logo {
            width: 50px; /* Breite des Logos anpassen */
            height: auto; /* Automatische Anpassung der Höhe */
            margin-right: 10px;  /* Abstand zwischen dem Logo und dem Schriftzug */
        }

        /* CSS für den Navbar-Titel */
        .navbar-title {
            font-size: 24px; /* Schriftgröße anpassen */
            font-weight: bold; /* Fett gedruckt */
        }

        /* CSS für die Navbar-Buttons */
        .navbar-right {
            display: flex;
            align-items: center; /* Vertikal zentrieren */
        }

         /* CSS für die Navbar-Logo & Titel */
        .navbar-left {
            display: flex;
            align-items: center; /* Vertikal zentrieren */
        }
        
        .trackmode-button,
        .toggle-button,
        .logout-button,
        .center-button {
            background-color: #6495ED; /* Hellblau */
            color: #FFF;
            border: none;
            border-radius: 5px;
            padding: 5px 15px;
            margin-right: 10px; /* Abstand zwischen den Buttons */
            cursor: pointer;
        }

        /* Weitere CSS-Stile für den Inhalt der geschützten Seite */
        .content {
            margin-top: 50px; /* Platz unter der Leiste schaffen */
        }

        body {
            background-color: #6495ED; /* Hellblau */
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
        }

        .container {
            background-color: #000; /* Schwarz */
            border-radius: 10px;
            padding: 40px; /* Erhöhte Innenabstände */
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
            text-align: center;
            color: #FFF;
            width: 400px; /* Erhöhte Breite des Containers */
            margin: 0 auto; /* Horizontal zentrieren */
        }

        .container h1 {
            font-size: 32px;
        }

        .container p {
            font-size: 18px;
        }

        .form-group {
            margin-bottom: 20px;
        }

        .form-group label {
            display: block;
            font-size: 20px;
        }

        #map {
            width: 100%;
            height: calc(100vh - 60px); /* 45px als Puffer für Navbar */
            margin-top: 60px; /* Platz für Navbar */
            z-index: 1;
        }

        /* CSS-Klasse für den schwarzen Container */
        .black-container {
            background-color: #000; /* Setze die Hintergrundfarbe auf Schwarz */
            color: #fff; /* Setze die Textfarbe auf Weiß oder eine passende Kontrastfarbe */
            padding: 10px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
            position: absolute;
            bottom: 15px;
            left: 15px;
            z-index: 2;
        }

        /* Neuer CSS-Stil für den deaktivierten Button */
        .toggle-button:disabled {
            background-color: #A9A9A9; /* Grau */
            cursor: not-allowed; /* Ändern Sie den Cursor für deaktivierte Buttons */
        }

        /* Neuer CSS-Stil für den deaktivierten Button */
        .trackmode-button:disabled {
            background-color: #A9A9A9; /* Grau */
            cursor: not-allowed; /* Ändern Sie den Cursor für deaktivierte Buttons */
        }

        .custom-navbar-spacing li {
            margin-bottom: 10px; /* Ändere den Abstand nach Bedarf */
        }

    </style>
    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.7.1/dist/leaflet.css" />
    <!-- Bootstrap CSS -->
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css">
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js"></script>
</head>
<body>
    <!-- Bootstrap Navbar -->
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
        <a class="navbar-brand" href="#">
            <img src="assets/images/logo.png" alt="Logo" class="navbar-logo">
            <span class="navbar-title">TrackKing</span>
        </a>
        <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNav" aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
        </button>
        <div class="collapse navbar-collapse" id="navbarNav">
            <ul class="navbar-nav ml-auto custom-navbar-spacing">
                <li class="nav-item">
                    <button class="trackmode-button" onclick="toggleButton('trackmode_button')">Track Mode</button>
                </li>
                <li class="nav-item">
                    <button class="toggle-button" onclick="toggleButton('on_off_button')">Tracker an/aus</button>
                </li>
                <li class="nav-item">
                    <button class="center-button" onclick="centerMap()">Karte zentrieren</button>
                </li>
                <li class="nav-item">
                    <button class="logout-button" onclick="logout()">Logout</button>
                </li>
            </ul>
        </div>
    </nav>
    </div>

    <div class="black-container">
        <!-- Hier kannst du den Inhalt deines Containers platzieren -->
        <p>Letztes Update vor: <span id="TimeValue"></span></p>
        <p>Geschwindigkeit: <span id="speedValue"></span> m/s</p>
        <p>Höhe: <span id="AltitudeValue"></span> Meter</p>
    </div>

    <div id="map"></div>

    <script src="https://unpkg.com/leaflet@1.7.1/dist/leaflet.js"></script>

    <script>
        var map; // Variable zur globalen Definition der Karte
        var marker; // Variable zur globalen Definition des Markers
        var isButtonOn; // Variable zur globalen Definition des Buttons

        // JavaScript-Funktion, um den Logout durchzuführen
        function logout() {
            if (confirm("Möchten Sie sich abmelden?")) {
                window.location.href = "tracker.php?logout=true";
            }
        }

        function toggleButton(button_name) {
            // Hier können Sie zusätzlichen Code ausführen, wenn der Button getoggelt wird

            if (button_name === "on_off_button") {
            isButtonOn = !isButtonOn;
            var buttonText = isButtonOn ? "Tracker ausschalten" : "Tracker einschalten";
            $(".toggle-button").text(buttonText);
            }

            // AJAX-Anfrage an toggle_button.php senden
            $.ajax({
                url: 'toggle_button.php',
                type: 'POST',
                data: { button_name : button_name},
                dataType: 'json', // Erwarteter Datentyp der Antwort
                success: function(response) {
                    // Hier können Sie zusätzlichen Code ausführen, wenn die Anfrage erfolgreich ist
                    console.log("Neuer Status = " + button_name + " : " + response.buttonStatus);
                },

                error: function(error) {
                    console.error("Fehler beim Ändern des Button-Status:", error);
                }
            });

            // Fügen Sie hier Ihren Toggle-Code hinzu, wenn der Button ein- oder ausgeschaltet wird
            if (button_name === "on_off_button") {
                if (isButtonOn) {
                // Hier Code für den eingeschalteten Zustand einfügen
                console.log("Button on/off ist eingeschaltet");
            } else {
                // Hier Code für den ausgeschalteten Zustand einfügen
                console.log("Button on/off ist ausgeschaltet");
            }

                // Deaktivieren Sie den Button für 5 Sekunden
                $(".toggle-button").prop('disabled', true);
                setTimeout(function () {
                    // Aktivieren Sie den Button nach 5 Sekunden
                    $(".toggle-button").prop('disabled', false);
                }, 5000);

              }

              if (button_name === "trackmode_button") {
                startTimer(300);
            }

        }

        // Beispiel für JavaScript-Code zum Starten des Timers
        function startTimer(durationInSeconds) {
            remainingTime = durationInSeconds;

            timerInterval = setInterval(function () {
                remainingTime--;

                $(".trackmode-button").text('Track Mode (' + formatTime(remainingTime) + ')');
                $(".trackmode-button").prop('disabled', true);
                $(".toggle-button").prop('disabled', true);

                if (remainingTime <= 0) {
                    clearInterval(timerInterval);
                    $(".trackmode-button").text('Track Mode');
                    $(".trackmode-button").prop('disabled', false);
                    $(".toggle-button").prop('disabled', false);

                    // AJAX-Anfrage an toggle_button.php senden
                    $.ajax({
                        url: 'toggle_button.php',
                        type: 'POST',
                        data: { button_name : 'trackmode_button'},
                        dataType: 'json', // Erwarteter Datentyp der Antwort
                        success: function(response) {
                            // Hier können Sie zusätzlichen Code ausführen, wenn die Anfrage erfolgreich ist
                            console.log("Neuer Status = trackmode_button : " + response.buttonStatus);
                        },

                        error: function(error) {
                            console.error("Fehler beim Ändern des Button-Status:", error);
                        }
                    });

                }
            }, 1000);
        }

        function formatTime(seconds) {
            var minutes = Math.floor(seconds / 60);
            var remainingSeconds = seconds % 60;

            if (remainingSeconds < 10) {
                remainingSeconds = '0' + remainingSeconds;
            }

            return minutes + ':' + remainingSeconds;
        }

        // JavaScript-Funktion, um die Karte auf die Marker-Koordinaten zu zentrieren
        function centerMap() {
            if (marker) {
                var latitude = parseFloat(marker.getLatLng().lat);
                var longitude = parseFloat(marker.getLatLng().lng);
                map.setView([latitude, longitude], 15); // Karte auf Marker-Koordinaten zentrieren
            }
        }

        // JavaScript-Funktion, um den Marker zu aktualisieren und die Karte beim ersten Laden auf den Marker zu zentrieren
        function updateMarker() {
            var xhr = new XMLHttpRequest();
            xhr.open('POST', 'get_gps_data_from_db.php', true); // Neuer PHP-Endpunkt
            xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    var data = JSON.parse(xhr.responseText);
                    var latitude = parseFloat(data.latitude);
                    var longitude = parseFloat(data.longitude);
                    if (!marker || marker.getLatLng().lat !== latitude || marker.getLatLng().lng !== longitude) {
                        if (!marker) {
                            marker = L.marker([latitude, longitude]).addTo(map).bindPopup('GPS Tracker Standort');
                            marker.openPopup();
                            // Beim ersten Laden der Seite zentrieren
                            map.setView([latitude, longitude], 15);
                        } else {
                            marker.setLatLng([latitude, longitude]); // Marker aktualisieren
                        }
                    }
                }
            };
            xhr.send();
        }

        // JavaScript-Funktion, um die Geschwindigkeitsanzeige zu aktualisieren
        function updateSpeedAndAltitude() {
            var xhr = new XMLHttpRequest();
            xhr.open('POST', 'get_gps_data_from_db.php', true); // Neuer PHP-Endpunkt
            xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    var data = JSON.parse(xhr.responseText);
                    var speed = parseFloat(data.speed);
                    var altitude =parseFloat(data.altitude);
                    if (!isNaN(speed) || !isNaN(altitude)) {
                        if (speed < 1) {
                            document.getElementById("speedValue").textContent = 0;
                        } else {
                            document.getElementById("speedValue").textContent = speed;
                        }
                        document.getElementById("AltitudeValue").textContent = altitude;
                    } else {
                        console.error("Ungültiger Speed-Wert: ", data);
                    }
                }
            };
            xhr.send();
        }

        function updateTime() {
        var xhr = new XMLHttpRequest();
        xhr.open('POST', 'get_gps_data_from_db.php', true); // Neuer PHP-Endpunkt
        xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
        xhr.onreadystatechange = function () {
            if (xhr.readyState === 4 && xhr.status === 200) {
                var data = JSON.parse(xhr.responseText);
                var timestamp = data.timestamp_column; // Der Zeitstempel aus der Datenbank (im Format yyyy-MM-dd HH:mm:ss)

                if (timestamp) {
                    var currentTime = new Date();
                    var databaseTime = new Date(timestamp);
                    var timeDifference = currentTime - databaseTime;

                    var seconds = Math.floor((timeDifference / 1000) % 60);
                    var minutes = Math.floor((timeDifference / 1000 / 60) % 60);
                    var hours = Math.floor((timeDifference / (1000 * 60 * 60)) % 24);
                    var days = Math.floor(timeDifference / (1000 * 60 * 60 * 24));

                    var formattedTime = "";

                    if (days > 0) {
                        formattedTime += days + " Tage ";
                    }

                    if (hours > 0) {
                        formattedTime += hours + " Stunden ";
                    }

                    if (minutes > 0) {
                        formattedTime += minutes + " Minuten ";
                    }

                    formattedTime += seconds + " Sekunden";

                    document.getElementById("TimeValue").textContent = formattedTime;
                } else {
                    console.error("Ungültiger Zeitstempel: ", data);
                }
            }
        };
        xhr.send();
    }

        // Warten, bis das Fenster vollständig geladen ist
        window.onload = function () {
            // Initialisieren Sie die Karte
            map = L.map('map').setView([0, 0], 15); // Anfangsposition der Karte

            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
            }).addTo(map);

            // Rufen Sie die updateMarker-Funktion auf, um den Marker einmal beim Laden zu aktualisieren
            updateMarker();

            // Aktualisieren Sie den Speed beim laden der Seite
            updateSpeedAndAltitude();

            // Aktualisieren Sie die Zeit beim laden der Seite
            updateTime();

            // Aktualisieren Sie den Marker alle 3 Sekunden
            setInterval(updateMarker, 3000);
            // Aktualisieren Sie die Geschwindigkeit und Höhe jede Sekunde
            setInterval(updateSpeedAndAltitude, 1000)
            // Aktualisieren Sie die Zeit jede Sekunde
            setInterval(updateTime, 1000)
        };

        document.addEventListener("DOMContentLoaded", function() {
            $(".trackmode-button").prop('disabled', true);

            // AJAX-Anfrage, um den aktuellen Button-Status abzurufen
            $.ajax({
                url: 'get_button_status.php', // Ersetzen Sie 'get_button_status.php' durch den tatsächlichen Dateinamen oder Pfad
                type: 'POST',
                data: { button_name: 'on_off_button'},
                dataType: 'text',
                success: function(response) {
                    // Hier können Sie zusätzlichen Code ausführen, wenn die Anfrage erfolgreich ist
                    var buttonText = response == 1 ? "Tracker ausschalten" : "Tracker einschalten";
                    $(".toggle-button").text(buttonText);
                },
                error: function(xhr, status, error) {
                console.error("Fehler beim Abrufen des Button-Status:", xhr.responseText); // Änderung hier
                console.error("Status:", status);
                console.error("Fehlerobjekt:", error);
            }
            });

             
            // AJAX-Anfrage, um den Trackmode-Button Timer zu überprüfen
            $.ajax({
                url: 'get_button_status.php',
                type: 'POST',
                data: { button_name: 'trackmode_button'},
                dataType: 'text',
                success: function(response) {
                    // Hier können Sie zusätzlichen Code ausführen, wenn die Anfrage erfolgreich ist
                    if (response == 1) {

                        // AJAX-Anfrage, um den Trackmode-Button Timer zu überprüfen
                        $.ajax({
                            url: 'get_button_time.php',
                            type: 'POST',
                            data: { button_name: 'trackmode_button'},
                            dataType: 'text',
                            success: function(response) {
                                // Hier können Sie zusätzlichen Code ausführen, wenn die Anfrage erfolgreich ist
                                if (response > 0) {
                                    startTimer(response);
                                    $(".toggle-button").prop('disabled', true);
                                } else {
                                    $(".trackmode-button").text('Track Mode');
                                    $(".trackmode-button").prop('disabled', false)
                                    $(".toggle-button").prop('disabled', false)

                                    // AJAX-Anfrage an toggle_button.php senden
                                    $.ajax({
                                        url: 'toggle_button.php',
                                        type: 'POST',
                                        data: { button_name : 'trackmode_button'},
                                        dataType: 'json', // Erwarteter Datentyp der Antwort
                                        success: function(response) {
                                            // Hier können Sie zusätzlichen Code ausführen, wenn die Anfrage erfolgreich ist
                                            console.log("Neuer Status = trackmode_button : " + response.buttonStatus);
                                        },

                                        error: function(error) {
                                            console.error("Fehler beim Ändern des Button-Status:", error);
                                        }
                                    });

                                }
                            },
                            error: function(xhr, status, error) {
                            console.error("Fehler beim Abrufen des Button-Status:", xhr.responseText);
                            console.error("Status:", status);
                            console.error("Fehlerobjekt:", error);
                            }
                        });

                    } else {
                        $(".trackmode-button").prop('disabled', false);
                        $(".toggle-button").prop('disabled', false);
                    }
                },
                error: function(xhr, status, error) {
                console.error("Fehler beim Abrufen des Button-Status:", xhr.responseText); 
                console.error("Status:", status);
                console.error("Fehlerobjekt:", error);
                }
            });
        });
    </script>
</body>
</html>