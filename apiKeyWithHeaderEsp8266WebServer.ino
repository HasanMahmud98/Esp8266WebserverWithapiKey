#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "";//Enter Name of the SSid
const char* password = ""; //Enter Pass

// API key for validation
const char* apiKey = "your_api_key_here";

// Web server instance on port 80
ESP8266WebServer server(80);

// Function to handle POST requests
void handlePostRequest() {
  // Check if the body has been received
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body not received");
    return;
  }

  // Log the headers received from the request
  Serial.println("Received Headers:");
  for (int i = 0; i < server.headers(); i++) {
    Serial.println(server.headerName(i) + ": " + server.header(i));
  }

  // Check for the "API-Key" header
  if (!server.hasHeader("API-Key")) {
    server.send(400, "text/plain", "API Key header missing");
    return;
  }

  // Retrieve and validate the API Key
  String receivedApiKey = server.header("API-Key");
  Serial.println("Received API Key: " + receivedApiKey);

  if (receivedApiKey != apiKey) {
    server.send(403, "text/plain", "Invalid API key");
    return;
  }

  // Get the POST body data
  String message = server.arg("plain");
  Serial.println("Received POST data: " + message);

  // Respond with success message
  server.send(200, "text/plain", "POST request received with valid API key. Data: " + message);
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Collect headers that you want to check for
  const char* headerKeys[] = {"Authorization", "If-None-Match", "API-Key"};
  server.collectHeaders(headerKeys, sizeof(headerKeys) / sizeof(headerKeys[0]));

  // Define route for POST request
  server.on("/post", HTTP_POST, handlePostRequest);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}
