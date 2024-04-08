#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Mi";     // Your WiFi SSID
const char* password = "shashank123"; // Your WiFi password

ESP8266WebServer server(80);

IPAddress staticIP(192, 168, 1, 100); // Set your desired static IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8); // Set your DNS server

bool loggedIn = false;
String adminUsername = "admin";
String adminPassword = "admin";
String blockedIP = "192.168.1.10"; // Initially blocked IP

void handleRoot() {
  if (!loggedIn) {
    String page = "<!DOCTYPE html>";
    page += "<html lang='en'>";
    page += "<head>";
    page += "<meta charset='UTF-8'>";
    page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    page += "<title>Login</title>";
    page += "<style>";
    page += "body {";
    page += "  font-family: Arial, sans-serif;";
    page += "  display: flex;";
    page += "  justify-content: center;";
    page += "  align-items: center;";
    page += "  height: 100vh;";
    page += "  margin: 0;";
    page += "}";
    page += "form {";
    page += "  background-color: #f2f2f2;";
    page += "  padding: 20px;";
    page += "  border-radius: 10px;";
    page += "}";
    page += "input[type='text'], input[type='password'], input[type='submit'] {";
    page += "  width: 100%;";
    page += "  padding: 10px;";
    page += "  margin: 5px 0;";
    page += "  display: inline-block;";
    page += "  border: 1px solid #ccc;";
    page += "  border-radius: 5px;";
    page += "  box-sizing: border-box;";
    page += "}";
    page += "input[type='submit'] {";
    page += "  background-color: #4CAF50;";
    page += "  color: white;";
    page += "  border: none;";
    page += "}";
    page += "</style>";
    page += "</head>";
    page += "<body>";
    page += "<form action='/login' method='POST'>";
    page += "<h2>Login</h2>";
    page += "<input type='text' name='username' placeholder='Username'><br>";
    page += "<input type='password' name='password' placeholder='Password'><br>";
    page += "<input type='submit' value='Login'>";
    page += "</form>";
    page += "</body>";
    page += "</html>";
    server.send(200, "text/html", page);
  } else {
    String page = "<!DOCTYPE html>";
    page += "<html lang='en'>";
    page += "<head>";
    page += "<meta charset='UTF-8'>";
    page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    page += "<title>Admin Panel</title>";
    page += "<style>";
    page += "body {";
    page += "  font-family: Arial, sans-serif;";
    page += "  background-color: #f2f2f2;";
    page += "}";
    page += ".container {";
    page += "  width: 50%;";
    page += "  margin: 0 auto;";
    page += "  padding: 20px;";
    page += "}";
    page += "h2 {";
    page += "  text-align: center;";
    page += "}";
    page += "p {";
    page += "  text-align: center;";
    page += "}";
    page += "form {";
    page += "  text-align: center;";
    page += "}";
    page += "</style>";
    page += "</head>";
    page += "<body>";
    page += "<div class='container'>";
    page += "<h2>Welcome, Admin!</h2>";
    page += "<p>Blocked IP: " + blockedIP + "</p>";
    page += "<form action='/allow-ip' method='POST'>";
    page += "<input type='text' name='ip' placeholder='IP Address to Allow'><br>";
    page += "<input type='submit' value='Allow'>";
    page += "</form>";
    page += "<form action='/block-ip' method='POST'>";
    page += "<input type='text' name='ip' placeholder='IP Address to Block'><br>";
    page += "<input type='submit' value='Block'>";
    page += "</form>";
    page += "</div>";
    page += "</body>";
    page += "</html>";
    server.send(200, "text/html", page);
  }
}


void handleLogin() {
  if (server.hasArg("username") && server.hasArg("password")) {
    String username = server.arg("username");
    String password = server.arg("password");
    if (username == adminUsername && password == adminPassword) {
      loggedIn = true;
      handleRoot();
    } else {
      server.send(401, "text/html", "Unauthorized");
    }
  } else {
    server.send(400, "text/html", "Bad Request");
  }
}

void allowIP() {
  if (loggedIn && server.hasArg("ip")) {
    blockedIP = server.arg("ip");
    handleRoot();
  } else {
    server.sendHeader("Location", "/", true);   // Redirect to login page
    server.send(303);
  }
}

void blockIP() {
  if (loggedIn && server.hasArg("ip")) {
    blockedIP = server.arg("ip");
    // Here, you might want to add the blocked IP address to a list or perform any other necessary actions.
    handleRoot();
  } else {
    server.sendHeader("Location", "/", true);   // Redirect to login page
    server.send(303);
  }
}


void setup() {
  Serial.begin(9600);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/allow-ip", HTTP_POST, allowIP);
  server.on("/block-ip", HTTP_POST, blockIP);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}