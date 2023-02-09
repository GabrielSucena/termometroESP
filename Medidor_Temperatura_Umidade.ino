// --- WIFI ---
#include <ESP8266WiFi.h>
const char* ssid = ""; // nome da rede
const char* password = ""; // senha da rede
WiFiClient nodemcuClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqtt_Broker = "mqtt.eclipseprojects.io";
const char* mqtt_ClientID = "termometro-sucena01";
PubSubClient client(nodemcuClient);
const char* topicoTemperatura = "lab-sucena/temperatura";
const char* topicoUmidade = "lab-sucena/umidade";

// --- DHT ---
#include <DHT.h>
#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
int umidade;
int temperatura;

// --- DISPLAY ---
#include <Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  configurarDisplay();
  conectarWifi();
  client.setServer(mqtt_Broker, 1883);
}

void loop() {
  if(!client.connected()){
    reconectarMQTT();
  }
  medirTemperaturaUmidade();
  publicarTemperaturaUmidadeTopico();
  mostrarTemperaturaUmidade();
}
// --- FUNÇÕES AUXILIARES ---

// --- CONFIGURAÇÃO E CONEXÃO WIFI
void conectarWifi(){
  delay(10);
  
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Conectando ");
  display.display();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
}

// --- RECONECTAR O CLIENTE MQTT ---
void reconectarMQTT(){
  while(!client.connected()){
    client.connect(mqtt_ClientID);
  }
}

// --- CONFIGURA O DISPLAY ---
void configurarDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();
}

// --- PUBLICA (MQTT) TEMPERATURA E UMIDADE ---
void publicarTemperaturaUmidadeTopico(){
  client.publish(topicoTemperatura, String(temperatura).c_str(), true); // true retem o ultimo dado no broker
  client.publish(topicoUmidade, String(umidade).c_str(), true);
}

// --- MEDIÇÃO DE TEMPERATURA E UMIDADE ---
void medirTemperaturaUmidade() {
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
  delay(5000);
}

// --- MOSTRA TEMPERATURA E UMIDADE ---
void mostrarTemperaturaUmidade() {
  mostrarMensagemNoDisplay("Temperatura", (temperatura), " C");
  mostrarMensagemNoDisplay("Umidade", (umidade), " %");
}

// --- EXIBE TEMPERATURA E UMIDADE NO DISPLAY ---
void mostrarMensagemNoDisplay(const char* texto1, int medicao, const char* texto2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(texto1);
  display.setTextSize(5);
  display.setCursor(20, 20);
  display.print(medicao);
  display.setTextSize(2);
  display.print(texto2);
  display.display();
  delay(2000);
}
