#include <Wire.h>
#include <Adafruit_GFX.h>
// INI CAMBIO #include <Adafruit_SSD1306.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
  #define TFT_CS         D2 //4
  #define TFT_RST        D0 //16
  #define TFT_DC         D1 //5
// FIN CAMBIO

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <PubSubClient.h>
// CAMBIO #include <DHT.h>
#include <DHT11.h>
DHT11 dht11(D3); 
const int LDRPin = A0; 
int temperature = 0;
int humidity = 0;
// Definiciones
int conta = 0;

// // Ancho de la pantalla (en pixeles)
// #define SCREEN_WIDTH 128
// // Alto de la pantalla (en pixeles)
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels
// // Pin del sensor de temperatura y humedad
// #define DHTPIN 2
// // Tipo de sensor de temperatura y humedad
// //#define DHTTYPE DHT11
// Intervalo en segundo de las mediciones
#define MEASURE_INTERVAL 2
// Duración aproximada en la pantalla de las alertas que se reciban
#define ALERT_DURATION 60
 

// Declaraciones

// // Sensor DHT
// DHT dht(DHTPIN, DHTTYPE);
// // Pantalla OLED
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
// Cliente WiFi
WiFiClient net;
// Cliente MQTT
PubSubClient client(net);


// Variables a editar TODO

// WiFi
// Nombre de la red WiFi
const char ssid[] = "DiegoSteel_A33"; // TODO cambiar por el nombre de la red WiFi
// Contraseña de la red WiFi
const char pass[] = "DiegoMau"; // TODO cambiar por la contraseña de la red WiFi

//Conexión a Mosquitto
#define USER "leon14" // TODO Reemplace UsuarioMQTT por un usuario (no administrador) que haya creado en la configuración del bróker de MQTT.
const char MQTT_HOST[] = "13.218.221.163"; // TODO Reemplace ip.maquina.mqtt por la IP del bróker MQTT que usted desplegó. Ej: 192.168.0.1
const int MQTT_PORT = 8082;
const char MQTT_USER[] = USER;
//Contraseña de MQTT
const char MQTT_PASS[] = "qetu1357"; // TODO Reemplace ContrasenaMQTT por la contraseña correpondiente al usuario especificado.

//Tópico al que se recibirán los datos
// El tópico de publicación debe tener estructura: <país>/<estado>/<ciudad>/<usuario>/out
const char MQTT_TOPIC_PUB[] = "pais/estado/ciudad/" USER "/out"; //TODO Reemplace el valor por el tópico de publicación que le corresponde.
// El tópico de suscripción debe tener estructura: <país>/<estado>/<ciudad>/<usuario>/in
const char MQTT_TOPIC_SUB[] = "pais/estado/ciudad/" USER "/in"; //TODO Reemplace el valor por el tópico de suscripción que le corresponde.

// Declaración de variables globales

// Timestamp de la fecha actual.
time_t now;
// Tiempo de la última medición
long long int measureTime = millis();
// Tiempo en que inició la última alerta
long long int alertTime = millis();
// Mensaje para mostrar en la pantalla
String alert = "";
// Valor de la medición de temperatura
// float temp;
// // Valor de la medición de la humedad
// float humi;

/**
 * Conecta el dispositivo con el bróker MQTT usando
 * las credenciales establecidas.
 * Si ocurre un error lo imprime en la consola.
 */
void mqtt_connect()
{
  //Intenta realizar la conexión indefinidamente hasta que lo logre
  while (!client.connected()) {
    
    Serial.print("MQTT connecting ... ");
    
    if (client.connect(MQTT_USER, MQTT_USER, MQTT_PASS)) {
      
      Serial.println("connected.");
      client.subscribe(MQTT_TOPIC_SUB);
      
    } else {
      
      Serial.println("Problema con la conexión, revise los valores de las constantes MQTT");
      int state = client.state();
      Serial.print("Código de error = ");
      alert = "MQTT error: " + String(state);
      Serial.println(state);
      
      if ( client.state() == MQTT_CONNECT_UNAUTHORIZED ) {
        ESP.deepSleep(0);
      }
      
      // Espera 5 segundos antes de volver a intentar
      delay(5000);
    }
  }
}

/**
 * Publica la temperatura y humedad dadas al tópico configurado usando el cliente MQTT.
 */
void sendSensorData(int temperatura, int humedad) { //void sendSensorData(float temperatura, float humedad) {
  String data = "{";
  data += "\"temperatura\": "+ String(temperatura, 1) +", ";
  data += "\"humedad\": "+ String(humedad, 1);
  data += "}";
  char payload[data.length()+1];
  data.toCharArray(payload,data.length()+1);
  
client.publish(MQTT_TOPIC_PUB, payload);
}


/**
 * Lee la temperatura del sensor DHT, la imprime en consola y la devuelve.
 */
 // NO SE USARÁ
// float readTemperatura() {
  
//   // Se lee la temperatura en grados centígrados (por defecto)
//   float t = dht.readTemperature();
  
//   Serial.print("Temperatura: ");
//   Serial.print(t);
//   Serial.println(" *C ");
  
//   return t;
// }

/**
 * Lee la humedad del sensor DHT, la imprime en consola y la devuelve.
 */
 // NO SE USARÁ
// float readHumedad() {
//   // Se lee la humedad relativa
//   float h = dht.readHumidity();
  
//   Serial.print("Humedad: ");
//   Serial.print(h);
//   Serial.println(" %\t");

//   return h;
// }

/**
 * Verifica si las variables ingresadas son números válidos.
 * Si no son números válidos, se imprime un mensaje en consola.
 */
bool checkMeasures(int t, int h) {
  // Se comprueba si ha habido algún error en la lectura
    if (isnan(t) || isnan(h)) {
      Serial.println("Error obteniendo los datos del sensor DHT11");
      return false;
    }
    return true;
}

/**
 * Vincula la pantalla al dispositivo y asigna el color de texto blanco como predeterminado.
 * Si no es exitosa la vinculación, se muestra un mensaje en consola.
 */
// void startDisplay() {
//   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
//     Serial.println(F("SSD1306 allocation failed"));
//     for(;;);
//   }
//   display.setTextColor(SSD1306_WHITE);
// }

/**
 * Imprime en la pantallaa un mensaje de "No hay señal".
 */
void displayNoSignal() {
  // display.clearDisplay();
  
  // display.setTextSize(3);
  // display.setCursor(10, 10);
  // display.println("No hay señal");
  
  // display.display();
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 70);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.println("No hay señal");
}

/**
 * Agrega a la pantalla el header con mensaje "IOT Sensors" y en seguida la hora actual
 */
void displayHeader() {
  //display.setTextSize(2);
  tft.setTextSize(2);
  long long int milli = now + millis() / 1000;
  struct tm* tinfo;
  tinfo = localtime(&milli);
  String hour = String(asctime(tinfo)).substring(11, 19);
  
  String title = "IOT Sensors  " + hour;
  //display.println(title);
  tft.println(title);
}

/**
 * Agrega los valores medidos de temperatura y humedad a la pantalla.
 */
void displayMeasures() {
  //tft.fillScreen(0);
  // tft.setCursor(30, 30);
  // tft.println("                  ");
  // tft.println("                  ");
  // tft.println("                  ");
  //tft.setCursor(30, 30);
  tft.setTextSize(2);
  tft.println("");
  tft.print("T: ");
  tft.print(temperature);
  tft.print("    ");
  tft.print("H: ");
  tft.print(humidity);
  tft.println("");
}

/**
 * Agrega el mensaje indicado a la pantalla.
 * Si el mensaje es OK, se busca mostrarlo centrado.
 */
void displayMessage(String message) {
  
  //display.setTextSize(2);
  tft.setTextSize(2);
  tft.println("\nMsg:");
  
  //display.setTextSize(3);
  tft.setTextSize(2);
  
  if (message.equals("OK")) {
    tft.println("    " + message); 
  } else {
    //display.setTextSize(3);
    tft.setTextSize(2);
    tft.println("");
    tft.println("");
    tft.println(message); 
  }
}

/**
 * Muestra en la pantalla el mensaje de "Connecting to:" 
 * y luego el nombre de la red a la que se conecta.
 */
void displayConnecting(String ssid) {
  //display.clearDisplay();
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_BLUE);
 tft.setCursor(30, 30);
  tft.setTextSize(2);
  tft.println("Connecting to:\n");
  tft.println(ssid);
  
  //display.display();
}

/**
 * Verifica si ha llegdo alguna alerta al dispositivo.
 * Si no ha llegado devuelve OK, de lo contrario retorna la alerta.
 * También asigna el tiempo en el que se dispara la alerta.
 */
String checkAlert() {
  String message = "OK";
  
  if (alert.length() != 0) {
    message = alert;
    if ((millis() - alertTime) >= ALERT_DURATION * 1000 ) {
      alert = "";
      alertTime = millis();
     }
  }
  return message;
}

/**
 * Función que se ejecuta cuando llega un mensaje a la suscripción MQTT.
 * Construye el mensaje que llegó y si contiene ALERT lo asgina a la variable 
 * alert que es la que se lee para mostrar los mensajes.
 */
void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  String data = "";
  for (int i = 0; i < length; i++) {
    data += String((char)payload[i]);
  }
  Serial.print(data);
  if (data.indexOf("ALERT") >= 0) {
    alert = data;
  }
}

/**
 * Verifica si el dispositivo está conectado al WiFi.
 * Si no está conectado intenta reconectar a la red.
 * Si está conectado, intenta conectarse a MQTT si aún 
 * no se tiene conexión.
 */
void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Checking wifi");
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      // REEMPLAZAR displayNoSignal();
      delay(10);
    }
    Serial.println("connected");
  }
  else
  {
    if (!client.connected())
    {
      mqtt_connect();
    }
    else
    {
      client.loop();
    }
  }
}

/**
 * Imprime en consola la cantidad de redes WiFi disponibles y
 * sus nombres.
 */
void listWiFiNetworks() {
  int numberOfNetworks = WiFi.scanNetworks();
  Serial.println("\nNumber of networks: ");
  Serial.print(numberOfNetworks);
  for(int i =0; i<numberOfNetworks; i++){
      Serial.println(WiFi.SSID(i));
 
  }
}

/**
 * Inicia el servicio de WiFi e intenta conectarse a la red WiFi específicada en las constantes.
 */
void startWiFi() {
  
  WiFi.hostname(USER);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  Serial.println("(\n\nAttempting to connect to SSID: ");
  Serial.print(ssid);
  //Intenta conectarse con los valores de las constantes ssid y pass a la red Wifi
  //Si la conexión falla el node se dormirá hasta que lo resetee
  while (WiFi.status() != WL_CONNECTED)
  {
    if ( WiFi.status() == WL_NO_SSID_AVAIL ) {
      Serial.println("\nNo se encuentra la red WiFi ");
      Serial.print(ssid);
      WiFi.begin(ssid, pass);
    } else if ( WiFi.status() == WL_WRONG_PASSWORD ) {
      Serial.println("\nLa contraseña de la red WiFi no es válida.");
    } else if ( WiFi.status() == WL_CONNECT_FAILED ) {
      Serial.println("\nNo se ha logrado conectar con la red, resetee el node y vuelva a intentar");
      WiFi.begin(ssid, pass);
    }
    Serial.print(".");
    delay(1000);
  }
  Serial.println("connected!");
}

/**
 * Consulta y guarda el tiempo actual con servidores SNTP.
 */
void setTime() {
  //Sincroniza la hora del dispositivo con el servidor SNTP (Simple Network Time Protocol)
  Serial.print("Setting time using SNTP");
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < 1510592825) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  //Una vez obtiene la hora, imprime en el monitor el tiempo actual
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

/**
 * Configura el servidor MQTT y asigna la función callback para mensajes recibidos por suscripción.
 * Intenta conectarse al servidor.
 */
void configureMQTT() {
  //Llama a funciones de la librería PubSubClient para configurar la conexión con Mosquitto
  client.setServer(MQTT_HOST, MQTT_PORT);
  
  // Se configura la función que se ejecutará cuando lleguen mensajes a la suscripción
  client.setCallback(receivedCallback);
  
  //Llama a la función de este programa que realiza la conexión con Mosquitto
  mqtt_connect();
}

/**
 * Verifica si ya es momento de hacer las mediciones de las variables.
 * si ya es tiempo, mide y envía las mediciones.
 */
void measure() {
  if ((millis() - measureTime) >= MEASURE_INTERVAL * 1000 ) {
    Serial.print("\nMidiendo variables...");
    measureTime = millis();
    
    // temp = readTemperatura();
    // humi = readHumedad();
    int result = dht11.readTemperatureHumidity(temperature, humidity);    // Attempt to read the temperature and humidity values from the DHT11 sensor.     
    
    // Se chequea si los valores son correctos
    if (checkMeasures(temperature, humidity)) {
      // Se envían los datos
      sendSensorData(temperature, humidity); 
    }
  }
  // conta ++;
  // Serial.println(conta);
}

/////////////////////////////////////
//         FUNCIONES ARDUINO       //
/////////////////////////////////////

void setup() {
  Serial.begin(115200);

  listWiFiNetworks();

  //startDisplay();
  tft.init(240, 320);           // Init ST7789 240x240
  tft.setRotation(1);
  tft.fillScreen(0);
  displayConnecting(ssid);

  startWiFi();

  //dht.begin();

  setTime();

configureMQTT(); 
}

void loop() {

 checkWiFi();

  String message = checkAlert();
  //tft.print(conta);
  measure();
  // Serial.print("-");
  // Serial.print(temperature);
  // Serial.print(":");
  // Serial.print(humidity);
  // Serial.print("->");
  
//  display.clearDisplay();
    //tft.fillScreen(ST77XX_BLACK);
    
 // display.setCursor(0, 0);
    // tft.setCursor(10,100);
    // tft.println("k");
  tft.fillScreen(0);
  tft.setCursor(10, 50);
  displayHeader();
  tft.setCursor(10, 100);
  displayMeasures();
  tft.setCursor(10, 160);
  displayMessage(message);


  //display.display();
}
