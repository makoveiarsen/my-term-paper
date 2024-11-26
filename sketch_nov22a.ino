#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "astra55";           // Ім'я Wi-Fi мережі
const char* password = "04100101";      // Пароль Wi-Fi мережі
const char* serverUrl = "https://back.virodip.click/api/list_users";  // URL сервера

int ledPin = 2;  // GPIO, до якого підключено світлодіод
std::vector<bool> previousStates;  // Динамічний масив для збереження станів користувачів

void setup() {
  Serial.begin(115200);

  // Налаштування піну для світлодіода
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Світлодіод вимкнено

  // Підключення до Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Підключення до Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi підключено");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFi.status();
    WiFi.status();
    WiFi.status();
    // Відправляємо GET-запит до сервера
    http.begin(serverUrl);
    http.addHeader("Authorization", "ESP32 ZiwOkdVU1JF3ODS6aBJk-alwyauevFogwStcC6Qac");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      // Отримуємо JSON-дані у вигляді рядка
      String jsonResponse = http.getString();
      Serial.println("Отримана відповідь від сервера:");
      Serial.println(jsonResponse);  // Додаємо лог

      // Обробка JSON-даних
      DynamicJsonDocument doc(2048);  // Розмір буфера (коригуйте для великої кількості даних)
      DeserializationError error = deserializeJson(doc, jsonResponse);

      if (error) {
        Serial.print("Помилка парсингу JSON: ");
        Serial.println(error.c_str());
      } else {
        // Отримуємо масив користувачів
        JsonArray users = doc["users"];
        int userCount = users.size();

        // Перевірка розміру масиву
        if (previousStates.size() != userCount) {
          Serial.println("Зміна кількості користувачів. Оновлюємо попередній стан.");
          previousStates.resize(userCount, false);  // Оновити розмір масиву
        }

        bool hasChanged = false;  // Змінна для перевірки змін

        // Перебір користувачів
        for (int i = 0; i < userCount; i++) {
          bool atHome = users[i]["at_home"];  // Стан користувача
          const char* email = users[i]["email"];
          int id = users[i]["id"];

          // Логування поточного стану
          Serial.print("Попередній стан для ");
          Serial.print(email);
          Serial.print(": ");
          Serial.println(previousStates[i] ? "Вдома" : "Не вдома");
          Serial.print("Новий стан для ");
          Serial.print(email);
          Serial.print(": ");
          Serial.println(atHome ? "Вдома" : "Не вдома");

          // Перевірка на зміну стану
          if (atHome != previousStates[i]) {
            Serial.print("Зміна стану для користувача: ");
            Serial.println(email);
            Serial.print("Попередній стан: ");
            Serial.println(previousStates[i] ? "Вдома" : "Не вдома");
            Serial.print("Новий стан: ");
            Serial.println(atHome ? "Вдома" : "Не вдома");

            // Засвітити індикатор
            digitalWrite(ledPin, HIGH);
            delay(250);  // Інтервал у 250 мс
            digitalWrite(ledPin, LOW);
            delay(250);  // Інтервал у 250 мс
            digitalWrite(ledPin, HIGH);
            delay(250);  // Інтервал у 250 мс
            digitalWrite(ledPin, LOW);
            delay(250);  // Інтервал у 250 мс
            digitalWrite(ledPin, HIGH);
            delay(250);  // Інтервал у 250 мс
            digitalWrite(ledPin, LOW);

            // Оновлюємо стан
            previousStates[i] = atHome;
            hasChanged = true;  // Якщо є зміна, ми вже маємо відправити сигнал
          }
        }

        // Якщо є будь-які зміни, ми миготимо індикатором
        if (hasChanged) {
          Serial.println("Стан змінено, індикатор мігає.");
        }
      }
    } else {
      Serial.print("Помилка HTTP-запиту: ");
      Serial.println(httpResponseCode);
    }

    // Завершуємо HTTP-запит
    http.end();
  } else {
    Serial.println("Wi-Fi не підключено");
  }

  // Затримка між запитами
  delay(5000);  // 5 секунд
}
