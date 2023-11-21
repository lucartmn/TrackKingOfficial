<!DOCTYPE html>
<html lang="en" data-bs-theme="auto">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Anmeldung - TrackKing</title>

    <link rel="apple-touch-icon" sizes="180x180" href="assets/favico/apple-touch-icon.png">
    <link rel="icon" type="image/png" sizes="32x32" href="assets/favico/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="assets/favico/favicon-16x16.png">
    <link rel="manifest" href="assets/favico/site.webmanifest">

    <link href="/docs/5.3/dist/css/bootstrap.min.css" rel="stylesheet">

    <style>
        body {
            background-color: #6495ED; /* Hellblau */
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }

        .form-signin-container {
            background-color: #000; /* Schwarz */
            color: #FFF; /* Weiß */
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
            text-align: center;
            width: 300px;
            position: relative; /* Hinzugefügt */
        }

        .form-signin {
            background-color: transparent; /* Transparent */
            color: #FFF; /* Weiß */
        }

        .form-signin h1 {
            font-size: 28px;
            color: #6495ED; /* Hellblau */
        }

        .form-signin .form-floating {
            margin-bottom: 10px;
        }

        .form-signin .form-floating input {
            width: 75%;
            padding: 8px;
            border: none;
            border-radius: 5px;
            font-size: 16px;
            background-color: #333; /* Dunkelgrau */
            color: #FFF; /* Weiß */
        }

        .form-signin .form-check {
            text-align: left;
            margin-top: 20px;
        }

        .form-signin .btn-primary {
            background-color: #6495ED; /* Hellblau */
            border: none;
            border-radius: 5px;
            padding: 10px;
            font-size: 18px;
            width: 50%;
            cursor: pointer;
        }

        .form-signin .mt-5 {
            margin-top: 1rem;
        }

        .logo-container {
            position: absolute;
            top: -200px;
            left: 50%;
            transform: translateX(-50%);
            z-index: 2;
        }

        .logo {
            width: 215px;
            height: 158px;
            border-radius: 10px;
        }
        
        /* CSS-Stile für Formularfelder */
        .form-signin .form-floating input {
            background-color: #333 !important; /* Dunkelgrau */
            color: #FFF !important; /* Weiß */
            border: none !important; /* Kein Rahmen */
            border-radius: 5px !important; /* Abgerundete Ecken */
        }

        /* CSS-Stile für Autofill-Felder in Chrome */
        .form-signin .form-floating input:-webkit-autofill,
        .form-signin .form-floating input:-webkit-autofill:focus,
        .form-signin .form-floating input:-webkit-autofill:hover,
        .form-signin .form-floating input:-webkit-autofill:active {
            transition: background-color 5000s ease-in-out 0s; /* Verzögere die Hintergrundänderung */
            -webkit-text-fill-color: #FFF !important; /* Textfarbe während Autofill */
        }

        /* CSS-Stile für Text im Autofill-Feld in Chrome */
        .form-signin .form-floating input::placeholder {
            color: #FFF !important; /* Textfarbe des Platzhalters während Autofill */
        }

        .error {
            color: red;
            bottom: 10px;
        }

    </style>
</head>
<body>
<div class="form-signin-container">
<?php
require_once("auth.php"); // Die externe PHP-Datei einbinden
    if (isset($login_error)) : ?>
        <p class="error"><?php echo $login_error; ?></p>
    <?php endif; ?>
  <div class="logo-container">
    <img class="logo" src="assets/images/logo.png" alt="Logo">
  </div>
  <form class="form-signin" method="post">
    <h1 class="h3 mb-3 fw-normal">Bitte anmelden</h1>
    <div class="form-floating">
      <input type="text" class="form-control" id="floatingInput" name="username" placeholder="Benutzername" required>
    </div>
    <div class="form-floating">
      <input type="password" class="form-control" id="floatingPassword" name="password" placeholder="Passwort" required>
    </div>
    <button class="btn btn-primary" type="submit">Anmelden</button>
    <p class="mt-5 mb-3 text-body-secondary">&copy; 2023</p>
  </form>
</div>
</body>
</html>
