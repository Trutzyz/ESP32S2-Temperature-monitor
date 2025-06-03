#include <WiFi.h>
#include <PubSubClient.h>

#define okButton 0
#define ledOutput 14
#define lm35Pin  5
#define BUTTON_QTY 1

// Configuração Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

// Configuração do broker MQTT (IP da sua Raspberry Pi)
const char* mqtt_server = "192.168.1.100";

byte buttonPin[BUTTON_QTY] = {42};

unsigned long previousDebounceTimeButtonHIGH[BUTTON_QTY];
unsigned long previousDebounceTimeButtonLOW[BUTTON_QTY];

int adcValue;
float voltage;
float temperatureC;

// Cliente Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Função de conexão Wi-Fi
void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("Conectando-se a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi conectado.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

// Função de reconexão MQTT
void reconnect()
{
    while (!client.connected()) {
        Serial.print("Tentando conexão MQTT...");
        if (client.connect("ESP32Client")) {
            Serial.println("conectado!");
            // client.subscribe("topico/teste"); // Se quiser escutar algo
        } else {
            Serial.print("falhou, rc=");
            Serial.print(client.state());
            Serial.println(" tentando novamente em 5 segundos.");
            delay(5000);
        }
    }
}

void setup()
{
    // Iniciando a inpresão serial
    Serial.begin(115200);
    Serial.println("Iniciando o Setup!");
    
    // Inicializando os pinos de input
    pinMode(buttonPin[okButton], INPUT_PULLUP);
    Serial.println("[+] Pino de botao iniciados.");

    // Iniciando os pinos de output
    pinMode(ledOutput, OUTPUT);
    digitalWrite(ledOutput, LOW);
    Serial.println("[+] Pino do LED inicializado.");

    // Configura ADC
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);

    // Conectando no Wi-Fi
    //setup_wifi();

    // Configurando servidor MQTT
    //client.setServer(mqtt_server, 1883);

    // Espera o usuario precionar o botão para iniciar a aplicação.
    Serial.println("Precione o botão para continuar!");
    digitalWrite(ledOutput, HIGH);
    while(checkButton(okButton));
    digitalWrite(ledOutput, LOW);

    Serial.println("Finalizando o Setup!");
}

void loop()
{
    //if (!client.connected())
        //reconnect();
    //client.loop();
    adcValue = analogRead(lm35Pin);
    voltage = adcValue * (3.3 / 4095.0);
    temperatureC = voltage * 100.0;

    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");

    // Publica a temperatura no tópico "sensor/temperatura"
    char tempString[8];
    dtostrf(temperatureC, 1, 2, tempString);
    //client.publish("sensor/temperatura", tempString);

    if(temperatureC > 50)
    {
        Serial.println("             ATENCAO!");
        Serial.println("TEMPERATURA ACIMA DOS NIVEIS NORMAIS!");
        digitalWrite(ledOutput, HIGH);
        //client.publish("sensor/alerta", "ALERTA: Temperatura Alta!");
    }
    else
        digitalWrite(ledOutput, LOW);
}
