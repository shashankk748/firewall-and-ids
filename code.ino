#include <WhatabotAPIClient.h>


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
// #include <WhatabotAPIClient.h>

const char* ssid = "Mi";     // Your WiFi SSID
const char* password = "shashank123"; // Your WiFi password
#define WHATABOT_API_KEY "ba44fa77-4b96-4455-a3e2-912a610ac0a4"
#define WHATABOT_CHAT_ID "9560180647"
#define WHATABOT_PLATFORM "whatsapp"

ESP8266WebServer server(80);

IPAddress staticIP(192, 168, 1, 100); // Set your desired static IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8); // Set your DNS server

bool loggedIn = false;
String adminUsername = "admin";
String adminPassword = "admin";
String blockedIP = "192.168.2.108"; // Initially blocked IP

int loginAttempts = 0;
const int maxLoginAttempts = 2; // Maximum allowed login attempts
WiFiManager wifiManager;
WhatabotAPIClient whatabotClient(WHATABOT_API_KEY, WHATABOT_CHAT_ID, WHATABOT_PLATFORM);


void handleRoot() {
  // Get the client's IP address
 // Get the client's IP address
  IPAddress clientIP = server.client().remoteIP();
  
  // Convert blockedIP string to IPAddress object
  IPAddress blockedIPAddress;
  if (!blockedIPAddress.fromString(blockedIP)) {
    Serial.println("Invalid blockedIP address");
    server.send(500, "text/plain", "Internal Server Error");
    return;
  }

  // Check if the client's IP address is blocked
  if (blockedIPAddress == clientIP) {
    server.send(403, "text/plain", "Access Forbidden");
    return;
  }

  // Check if the client's IP address is the same as the stored IP address
  if (loggedIn) {
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
    loginPage += "  display: flex;";
    loginPage += "  justify-content: center;";
    loginPage += "  align-items: center;";
    loginPage += "  height: 100vh;";
    loginPage += "  margin: 0;";
    loginPage += "}";
    loginPage += "form {";
    loginPage += "  background-color: #f2f2f2;";
    loginPage += "  padding: 20px;";
    loginPage += "  border-radius: 10px;";
    loginPage += "}";
    loginPage += "input[type='text'], input[type='password'], input[type='submit'] {";
    loginPage += "  width: 100%;";
    loginPage += "  padding: 10px;";
    loginPage += "  margin: 5px 0;";
    loginPage += "  display: inline-block;";
    loginPage += "  border: 1px solid #ccc;";
    loginPage += "  border-radius: 5px;";
    loginPage += "  box-sizing: border-box;";
    loginPage += "}";
    loginPage += "input[type='submit'] {";
    loginPage += "  background-color: #4CAF50;";
    loginPage += "  color: white;";
    loginPage += "  border: none;";
    loginPage += "}";
    loginPage += "</style>";
    loginPage += "</head>";
    loginPage += "<body>";
    loginPage += "<form action='/login' method='POST'>";
    loginPage += "<h2>Login</h2>";
    loginPage += "<input type='text' name='username' placeholder='Username'><br>";
    loginPage += "<input type='password' name='password' placeholder='Password'><br>";
    loginPage += "<input type='submit' value='Login'>";
    loginPage += "</form>";
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
      loginAttempts = 0; // Reset login attempts on successful login
      handleRoot();
    } else {
      loginAttempts++;
      if (loginAttempts >= maxLoginAttempts) {
        // Block the IP address and send WhatsApp message
        blockedIP = server.client().remoteIP().toString();
        whatabotClient.sendMessageREST("Login attempts exceeded from IP: " + blockedIP);
        server.send(401, "text/html", "Unauthorized");
      }
      handleRoot();
    }
  } else {
    server.send(400, "text/html", "Bad Request");
  }
}

void allowIP() {
  if (loggedIn && server.hasArg("ip")) {
    String ipToAllow = server.arg("ip");
    // Remove the allowed IP address from the list of blocked IPs
    if (blockedIP == ipToAllow) {
      blockedIP = "";
    }
    handleRoot();
  } else {
    server.sendHeader("Location", "/", true);   // Redirect to login page
    server.send(303);
  }
}


void blockIP() {
  if (loggedIn && server.hasArg("ip")) {
    String ipToBlock = server.arg("ip");
    // Check if blockedIP is empty
    if (blockedIP.length() == 0) {
      blockedIP = ipToBlock;
    } else {
      // Append the blocked IP address to the list of blocked IPs
      blockedIP += "," + ipToBlock;
    }
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
  // wifiManager.autoConnect(AP_SSID, AP_PASS);
  whatabotClient.begin();
  whatabotClient.onMessageReceived(onMessageReceived); 
  whatabotClient.onServerResponseReceived(onServerResponseReceived);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/allow-ip", HTTP_POST, allowIP);
  server.on("/block-ip", HTTP_POST, blockIP);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  whatabotClient.loop(); 
}
void onServerResponseReceived(String message) {
  Serial.println(message); 
}

void onMessageReceived(String message) {
  Serial.println(message);
  whatabotClient.sendMessageWS("Pong: " + message);
}
             