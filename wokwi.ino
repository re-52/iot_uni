#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#define LED_ROJO 14
#define LED_AZUL 12
#define BUZZER 27

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const String endpoint = "https://callback-iot.onrender.com/data";

LiquidCrystal_I2C lcd(0x27, 16, 2);

void sonidoCaidaMario() {
  for (int i = 1000; i > 100; i -= 20) {
    tone(BUZZER, i);
    delay(20);
  }
  noTone(BUZZER);
}

void mostrarTemperatura(float temp) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
  lcd.setCursor(0, 1);
  lcd.print(String(temp) + " C");
}

void controlarActuadores(float temp) {
  if (temp > 30.0) {
    digitalWrite(LED_ROJO, HIGH);
    digitalWrite(LED_AZUL, LOW);
    sonidoCaidaMario();
  } else {
    digitalWrite(LED_ROJO, LOW);
    digitalWrite(LED_AZUL, HIGH);
    noTone(BUZZER); 
  }
}




void setup() {
  Serial.begin(115200);

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi conectado!");
  delay(1000);
  lcd.clear();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(endpoint);

    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println("Respuesta JSON:");
      Serial.println(payload);

      DynamicJsonDocument doc(8192);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        for (int i = 0; i < doc.size(); i++) {
          if (doc[i].containsKey("temperature")) {
            float temp = doc[i]["temperature"];
            Serial.print("Temperatura encontrada: ");
            Serial.println(temp);

            mostrarTemperatura(temp);
            controlarActuadores(temp);

            delay(5000);
          }
        }
        Serial.println("Todos los objetos han sido procesados.");
      } else {
        Serial.print("Error al parsear JSON: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.println("Error en la solicitud HTTP");
    }

    http.end();
  } else {
    Serial.println("WiFi no conectado");
  }

  delay(10000);
}
