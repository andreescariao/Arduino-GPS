/*_____________________________________________________________________________________________________________________

Projeto Arduino-GPS:
O Código consiste em Receber via satélite os dados de coordenadas do GPS e enviar para o Monitor Serial do Arduino.

*/_____________________________________________________________________________________________________________________


// Biblioteca que recebe pacote de informações do módulo através da softwareserial e facilita a compreensão (interpreta os dados) / é válida pra qualquer módulo
#include <TinyGPS.h>

// Cria um objeto GPS para o módulo
TinyGPS gps;

// Declaração de variáveis ( newData, Chars, sentences, failed, latitude, longitude)
bool newData = false;
unsigned long chars;
unsigned short sentences, failed;
float lat, lon;


// Logo após as definições de nomes, o fluxo de execução entrará na função setup e realizará as configurações iniciais. 
void setup()
{
  // Primeiramente, o GPS 6M e a comunicação serial serão inicializados. 
  Serial.begin(9600);  
  Serial.println("O GPS do Parahyasas aguarda pelo sinal dos satelites...");
  Serial.println();
}


// Na função loop, iniciaremos a apresentação da lógica de funcionamento dos dispositivos e controle completo do sistema. 
void loop()
{
  
  // Os dados do GPS são analisados em um intervalo de 1 segundo e são lidos pelo monitor serial
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      if (gps.encode(c)) 
        newData = true;  
    }
  }

  if (newData)  // Se newData for true
  {

   // Recebe as coordenadas do GPS e envia pro Monitor Serial
    gps.f_get_position(&lat, &lon);   
    Serial.print("Latitude: ");
    Serial.print(lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat, 6);
    Serial.print(" | Longitude: ");
    Serial.print(lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon, 6);

  }

  // Se o GPS parar de receber a coordenadas
  Serial.println(failed);
}
