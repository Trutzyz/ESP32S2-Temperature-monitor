#include <WiFi.h>
#include <PubSubClient.h>

#define okButton 0
#define ledOutput 14
#define lm35Pin  5
#define BUTTON_QTY 1

// Configuração Wi-Fi (Utilizar rede movel)
const char* ssid = "tron";
const char* password = "12345678";

// Configuração do broker MQTT
const char* mqtt_server = "192.168.88.146";

byte buttonPin[BUTTON_QTY] = {42};
byte timeOutCount = 0;

unsigned long previousDebounceTimeButtonHIGH[BUTTON_QTY];
unsigned long previousDebounceTimeButtonLOW[BUTTON_QTY];

int cycles;
int adcValue;
float voltage;
float temperatureC;
float temperatureMean;
char tempString[8];

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

    while (WiFi.status() != WL_CONNECTED) 
    {
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
    while (!client.connected() && timeOutCount < 5) 
    {
        Serial.print("Tentando conexão MQTT...");
        if (client.connect("ESP32Client")) 
            Serial.println("conectado!");
        else 
        {
            Serial.print("falhou, rc=");
            Serial.print(client.state());
            Serial.println(" tentando novamente em 5 segundos.");
            timeOutCount++;
            delay(5000);
        }
    }
}

void setup()
{
    // Iniciando a inpresão serial
    Serial.begin(115200);
    Serial.println("Iniciando o Setup!");

    Serial.println("SERVIDOR BROKER EM: " + String(mqtt_server));
    
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
    setup_wifi();

    // Configurando servidor MQTT
    client.setServer(mqtt_server, 1883);

    cycles = 0;
    temperatureMean = 0.0;
    
    // Espera o usuario precionar o botão para iniciar a aplicação.
    Serial.println("Precione o botão para continuar!");
    digitalWrite(ledOutput, HIGH);
    while(checkButton(okButton));
    digitalWrite(ledOutput, LOW);

    Serial.println("Finalizando o Setup!");
}

void loop()
{
    if (!client.connected() && timeOutCount <= 5)
        reconnect();
    
    if(checkButton(okButton))
    {
        Serial.println("Tentando reconectar ao broker!");
        reconnect();
    }

    client.loop();

    adcValue = analogRead(lm35Pin);
    voltage = adcValue * (3.3 / 4095.0);
    temperatureC = voltage * 100.0;

    Serial.print(temperatureC);

    if(cycles >= 1000)
    {
        Serial.print("Temperature: ");
        Serial.println(" °C");
        temperatureMean = temperatureMean / cycles;
        // Publica a temperatura no tópico "sensor/temperatura"
        dtostrf(temperatureMean, 1, 2, tempString);
        client.publish("sensor/temperatura", tempString);

        if(temperatureMean > 34)
        {
            Serial.println("             ATENCAO!");
            Serial.println("TEMPERATURA ACIMA DOS NIVEIS NORMAIS!");
            digitalWrite(ledOutput, HIGH);
            client.publish("sensor/alerta", "ALERTA: TEMPERATURA ACIMA DOS NIVEIS NORMAIS!");
            delay(1000);
        }
        else
            digitalWrite(ledOutput, LOW);
        cycles = 0;
    }
    else
    {
        temperatureMean = temperatureMean + temperatureC;
        cycles++;
    }

}
