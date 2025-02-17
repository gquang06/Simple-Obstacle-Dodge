#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <LittleFS.h>

const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else {
    Serial.println("LittleFS mounted successfully");
  }
}

LiquidCrystal_I2C lcd(0x27, 16, 2); // my address LED is 0x27

// draw
byte bird[8] = { B00100, B01110, B01110, B01110, B01110, B01110, B01110, B00100 };
byte pipe[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };

// Game variables
int birdRow = 1;  // Bird position (0 or 1)
int birdCol = 2;  // Fixed column for bird
int pipeRow = 0;  
int pipeCol = 15; // Start pipe on the right
bool gameOver = false;

void UP() {
  Serial.println("Moving UP");
  birdRow = 0;
  lcd.setCursor(birdCol, birdRow);
  lcd.write(byte(0));
}

void DOWN() {
  Serial.println("Moving DOWN");
  birdRow = 1;
  lcd.setCursor(birdCol, birdRow);
  lcd.write(byte(0));
}

void setup() {

  initWiFi();
  initFS();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");

  server.on("/UP", HTTP_GET, [](AsyncWebServerRequest *request) {
    UP();
    request->send(LittleFS, "/index.html", "text/html", false);
  }); 

  server.on("/DOWN", HTTP_GET, [](AsyncWebServerRequest *request) {
    DOWN();
    request->send(LittleFS, "/index.html", "text/html", false);
  });

  server.begin();

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, bird);
  lcd.createChar(1, pipe);
}

void loop() {
  if (!gameOver) {
    lcd.clear();
        
    // Draw bird
    lcd.setCursor(birdCol, birdRow);
    lcd.write(byte(0));
        
    // Draw pipe
    lcd.setCursor(pipeCol, pipeRow);
    lcd.write(byte(1));
      
    // Move pipe left
    pipeCol--;

    // set pipe 
    if (pipeCol == 0)
      pipeRow = random(2);

    if (pipeCol < 0) pipeCol = 15; // Reset pipe
        
    // Check collision
    if (pipeCol == birdCol && birdRow == pipeRow) {
      gameOver = true;
      }
        
        delay(500); // Game speed
    }
    else {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Game Over!");
      delay(5000);
  }
}
