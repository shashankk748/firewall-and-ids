#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

const char* ssid = "Mi";     
const char* password = "shashank123"; 


ESP8266WebServer server(80);

IPAddress staticIP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
// String phoneNumber = "REPLACE_WITH_YOUR_PHONE_NUMBER";
// String apiKey = "REPLACE_WITH_API_KEY";
bool loggedIn = false;
String adminUsername = "admin";
String adminPassword = "admin";
String blockedIP = "192.168.121.153";

int loginAttempts = 0;
const int maxLoginAttempts = 2;
WiFiManager wifiManager;
bool flag=true;

void handleRoot() {
  // IPAddress clientIP = server.client().remoteIP();
  IPAddress clientIP = server.client().remoteIP();

// Split the blockedIP string into individual IP addresses
  int startPos = 0;
  int endPos = blockedIP.indexOf(',');
  String ip;

  // Iterate through each IP address
  while (endPos != -1) {
    ip = blockedIP.substring(startPos, endPos);
    ip.trim();

    // Check if the client's IP matches any of the blocked IP addresses
    if (clientIP.toString() == ip) {
      server.sendHeader("Location", "/login", true);
      server.send(302, "text/plain", "");
      return;
    }

    // Move to the next IP address
    startPos = endPos + 1;
    endPos = blockedIP.indexOf(',', startPos);
  }

  // Check the last IP address in the string
  ip = blockedIP.substring(startPos);
  ip.trim();
  if (clientIP.toString() == ip) {
    server.sendHeader("Location", "/login", true);
    server.send(302, "text/plain", "");
    return;
  }

  IPAddress blockedIPAddress;
  if (flag == true && !blockedIPAddress.fromString(blockedIP)) {
    Serial.println("Invalid blockedIP address");
    server.send(500, "text/plain", "Internal Server Error");
    flag = true;
    return;
  }

  if (blockedIPAddress == clientIP) {
    server.send(403, "text/plain", "Access Forbidden");
    return;
  }



  if (loggedIn) {
    String page = "<!DOCTYPE html>";
page += "<html lang='en'>";
page += "<head>";
page += "<meta charset='UTF-8'>";
page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
page += "<title>Admin Panel</title>";
page += "<style>";
page += "body {";
page += "  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;";
page += "  background-color: #f7f7f7;";
page += "  display: flex;";
page += "  justify-content: center;";
page += "  align-items: center;";
page += "  height: 100vh;";
page += "  margin: 0;";
page += "}";
page += ".container {";
page += "  width: 320px;";
page += "  padding: 20px;";
page += "  background-color: #fff;";
page += "  border-radius: 10px;";
page += "  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);";
page += "}";
page += "h2 {";
page += "  text-align: center;";
page += "  margin-bottom: 20px;";
page += "  color: #333;";
page += "}";
page += "p {";
page += "  text-align: center;";
page += "  margin-bottom: 20px;";
page += "  color: #666;";
page += "}";
page += "form {";
page += "  text-align: center;";
page += "}";
page += "input[type='text'], input[type='submit'] {";
page += "  width: calc(100% - 22px);";
page += "  padding: 10px;";
page += "  margin: 10px 0;";
page += "  display: block;";
page += "  border: 1px solid #ccc;";
page += "  border-radius: 5px;";
page += "  box-sizing: border-box;";
page += "}";
page += "input[type='submit'] {";
page += "  background-color: #4CAF50;";
page += "  color: white;";
page += "  position: relative;";
page += "  border: none;";
page += "  width: 50%;";
page += "  left: 25%;";
page += "  border-radius: 5px;";
page += "  font-size: 14px;";
page += "  cursor: pointer;";
page += "  transition: background-color 0.3s ease;";
page += "}";
page += "input[type='submit']:hover {";
page += "  background-color: #45a049;";
page += "}";
page += "</style>";
page += "</head>";
page += "<body>";
page += "<div class='container'>";
page += "<h2>Welcome, Admin!</h2>";
page += "<p>Blocked IP: " + blockedIP + "</p>";
page += "<form action='/allow-ip' method='POST'>";
page += "<input type='text' name='ip' placeholder='IP Address to Allow'>";
page += "<input type='submit' value='Allow'>";
page += "</form>";
page += "<form action='/block-ip' method='POST'>";
page += "<input type='text' name='ip' placeholder='IP Address to Block'>";
page += "<input type='submit' value='Block'>";
page += "</form>";
page += "</div>";
page += "</body>";
page += "</html>";

    server.send(200, "text/html", page);
  } else {
    String loginPage = "<!DOCTYPE html>";
loginPage += "<html lang='en'>";
loginPage += "<head>";
loginPage += "<meta charset='UTF-8'>";
loginPage += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
loginPage += "<title>Login</title>";
loginPage += "<style>";
loginPage += "body {";
loginPage += "  font-family: Arial, sans-serif;";
loginPage += "  background-color: #f7f7f7;";
loginPage += "  display: flex;";
loginPage += "  justify-content: center;";
loginPage += "  align-items: center;";
loginPage += "  height: 100vh;";
loginPage += "  margin: 0;";
loginPage += "}";
loginPage += ".login-container {";
loginPage += "  width: 320px;";
loginPage += "  padding: 20px;";
loginPage += "  background-color: #fff;";
loginPage += "  border-radius: 10px;";
loginPage += "  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);";
loginPage += "}";
loginPage += "input[type='text'], input[type='password'], input[type='submit'] {";
loginPage += "  width: 100%;";
loginPage += "  padding: 10px;";
loginPage += "  margin: 10px 0;";
loginPage += "  display: block;";
loginPage += "  border: 1px solid #ccc;";
loginPage += "  border-radius: 5px;";
loginPage += "  box-sizing: border-box;";
loginPage += "}";
loginPage += "input[type='submit'] {";
loginPage += "  background-color: #4CAF50;";
loginPage += "  color: white;";
loginPage += "  border: none;";
loginPage += "  cursor: pointer;";
loginPage += "}";
loginPage += ".login-title {";
loginPage += "  text-align: center;";
loginPage += "  margin-bottom: 20px;";
loginPage += "}";
loginPage += "</style>";
loginPage += "</head>";
loginPage += "<body>";
loginPage += "<div class='login-container'>";
loginPage += "<h2 class='login-title'>Login</h2>";
loginPage += "<form action='/login' method='POST'>";
loginPage += "<input type='text' name='username' placeholder='Username'><br>";
loginPage += "<input type='password' name='password' placeholder='Password'><br>";
loginPage += "<input type='submit' value='Login'>";
loginPage += "</form>";
loginPage += "</div>";
loginPage += "</body>";
loginPage += "</html>";
server.send(200, "text/html", loginPage);

  }
}

void handleLogin() {
  if (server.hasArg("username") && server.hasArg("password")) {
    String username = server.arg("username");
    String password = server.arg("password");
    if (username == adminUsername && password == adminPassword) {
      loggedIn = true;
      loginAttempts = 0; 
      handleRoot();
    } else {
      loginAttempts++;
      if (loginAttempts >= maxLoginAttempts) {
        blockedIP = server.client().remoteIP().toString();
        server.send(401, "text/html", "Unauthorized");
      }
      handleRoot();
    }
  } else {
    server.send(400, "text/html", "Bad Request");
  }
}

void allowIP() {
  if (server.hasArg("ip")) {
    String ipToAllow = server.arg("ip");
    blockedIP.replace(ipToAllow + ",", ""); // Remove the IP from the list
    blockedIP.replace("," + ipToAllow, ""); // Remove the IP from the list
    blockedIP.replace(ipToAllow, ""); // Remove the IP if it's the only one
    flag=false;
    handleRoot();
  } else {
    server.sendHeader("Location", "/", true);   
    server.send(303);
  }
}
void blockIP() {
  if (loggedIn && server.hasArg("ip")) {
    String ipToBlock = server.arg("ip");
    if (blockedIP.length() == 0) {
      blockedIP = ipToBlock;
    } else {
      blockedIP += "," + ipToBlock;
    }
    flag=false;
    handleRoot();
  } else {
    server.sendHeader("Location", "/", true);   
    server.send(303);
  }
}

void setup() {
  Serial.begin(9600);
  delay(10);

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


