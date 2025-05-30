#define okButton 0
#define ledOutput 14
#define lm35Pin  5
#define BUTTON_QTY 1

byte buttonPin[BUTTON_QTY] = {42};

unsigned long previousDebounceTimeButtonHIGH[BUTTON_QTY];
unsigned long previousDebounceTimeButtonLOW[BUTTON_QTY];

int adcValue;
float voltage;
float temperatureC;

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


    // Espera o usuario precionar o botão para iniciar a aplicação.
    Serial.println("Precione o botão para continuar!");
    while(checkButton(okButton));

    Serial.println("Finalizando o Setup!");
    delay(1000);
}

void loop()
{
    adcValue = analogRead(lm35Pin);
    voltage = adcValue * (3.3 / 4095.0);
    temperatureC = voltage * 100.0;

    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
    // Deverá enviar essas informações para o zabix

    if(temperatureC > 50)
    {
        Serial.println("             ATENCAO!");
        Serial.println("TEMPERATURA ACIMA DOS NIVEIS NORMAIS!");
        digitalWrite(ledOutput, HIGH);
        // Deverá enviar um alerta para alguem cadastrado ou coisa do genero
    }
    else
        digitalWrite(ledOutput, LOW);
}