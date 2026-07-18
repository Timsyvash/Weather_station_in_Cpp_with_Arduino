#include <DHT.h>
#include <Adafruit_NeoPixel.h>

#define DHTPIN 2 // Пін, до якого підключено датчик DHT
// РОЗКОМЕНТУЙТЕ ВАШ ТИП ДАТЧИКА:
#define DHTTYPE DHT11 // Якщо у вас DHT 11
// #define DHTTYPE DHT22   // Якщо у вас DHT 22 (AM2302)

#define LED_PIN 6    // Пін даних RGB матриці
#define NUMPIXELS 64 // Кількість світлодіодів (8x8)

DHT dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Спрощені кольори для матриці
uint32_t red, green, blue, yellow, off;

// Бітові маски для іконок 8x8 (1 - світлодіод горить, 0 - вимкнений)
const byte happyFace[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100};

const byte sadFace[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100};

// Функція для малювання іконки за маскою
void drawIcon(const byte icon[8], uint32_t color)
{
  pixels.clear();
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      // Перевіряємо кожен біт у байті рядка
      if (bitRead(icon[row], 7 - col))
      {
        // Вираховуємо індекс світлодіода (для стандартної стрічкової топології матриць)
        int pixelIndex = row * 8 + col;
        pixels.setPixelColor(pixelIndex, color);
      }
    }
  }
  pixels.show();
}

// Малюємо червоний знак "Х" якщо датчик відключився
void showError()
{
  pixels.clear();
  for (int i = 0; i < 8; i++)
  {
    pixels.setPixelColor(i * 8 + i, red);       // Головна діагональ
    pixels.setPixelColor(i * 8 + (7 - i), red); // Побічна діагональ
  }
  pixels.show();
}

void setup()
{
  Serial.begin(9600);
  dht.begin();
  pixels.begin();
  pixels.setBrightness(5); // Яскравість (0-255). Не ставте високу без зовнішнього живлення!

  // Ініціалізація кольорів
  red = pixels.Color(255, 0, 0);
  green = pixels.Color(0, 255, 0);
  blue = pixels.Color(0, 0, 255);
  yellow = pixels.Color(255, 200, 0);
  off = pixels.Color(0, 0, 0);
}

void loop()
{
  // Зчитування вологості та температури
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();

  // Перевірка чи зчитування успішне
  if (isnan(humidity) || isnan(tempC))
  {
    Serial.println("Помилка зчитування з датчика DHT!");
    showError(); // Показати червоний хрест у разі помилки
    delay(2000);
    return;
  }

  // Вивід в консоль для контролю
  Serial.print("Температура: ");
  Serial.print(tempC);
  Serial.print(" *C | Вологість: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Логіка відображення на матриці
  if (tempC >= 18 && tempC <= 25)
  {
    // Комфортна температура -> Зелений веселий смайлик
    drawIcon(happyFace, green);
  }
  else if (tempC > 25)
  {
    // Спекотно -> Червоний сумний смайлик
    drawIcon(sadFace, red);
  }
  else
  {
    // Холодно -> Синій сумний смайлик
    drawIcon(sadFace, blue);
  }

  delay(3000); // Оновлення кожні 3 секунди
}
