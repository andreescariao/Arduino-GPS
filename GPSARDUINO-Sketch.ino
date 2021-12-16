/*
Desenvolvimento: André Escarião, Graduando em Engenharia Elétrica(UFCG)

Projeto Arduino-GPS:

O projeto grava dados referentes ao tempo e posições de locomoção de um dispositivo de uma localidade para outra.
Assim, o sistema possuirá dois botões. Um será utilizado para iniciar a gravação das posições e o segundo, será utilizado para finalizar as gravações.
Além disso, teremos um LED para sinalizar o funcionamento e gravação dos dados do GPS em tempo real.
Dessa forma, a IDE recebe informações do módulo GPS - latitude, longitude, data, hora, altitude, velocidade e sentido - por meio de satélites e armazena no Cartão Micro SD.
*/


// Biblioteca que já vem com o arduino que faz com que as portas digitais funcionem como porta serial
#include <SoftwareSerial.h>
// Biblioteca que recebe pacote de informações do módulo através da softwareserial e facilita a compreensão (interpreta os dados) / é válida pra qualquer módulo
#include <TinyGPS.h>
// Biblioteca do Cartão SD para gravar na memória
#include <SD.h>
// Biblioteca que complementa a biblioteca <SD.h>
#include <SPI.h>



// Criar duas portas de comunicação serial que representam os pinos Rx e Tx 
SoftwareSerial SerialGPS(4,3);     

// Definição dos objetos GPS e myFile para os módulos 
TinyGPS GPS;              
File myFile;

// Declaração das variáveis (latitude, longitude, altitude, velocidade, sentido, ano, mes, dia, hora, minuto, segundo)
bool controle = 0;
float lat, lon, alt, vel;
unsigned long sentido;
int ano;
byte mes, dia, hora, minuto, segundo;
byte pinoCS = 10; // Pin 10 para UNO
        
// Define nomes para os pinos de conexão dos botões (Inicia e Termina) e LED (Vermelho) 
#define INICIA 9
#define TERMINA 5
#define LEDVERMELHO 2



// Logo após as definições de nomes, o fluxo de execução entrará na função setup e realizará as configurações iniciais. 
void setup() {
  
  // Primeiramente, o GPS 6M e a comunicação serial serão inicializados. 
  SerialGPS.begin(9600);  
  Serial.begin(9600);     
 
  Serial.println("O GPS do Parahyasas aguarda pelo sinal dos satelites...");
  
  pinMode(pinoCS, OUTPUT); // Define o pinoSS como saida
  pinMode(LEDVERMELHO, OUTPUT);

    // Em seguida, será realizada a inicialização de comunicação do SD Card.
    // Caso ocorra alguma falha de comunicação com o Módulo SD Card, o sistema apresentará uma mensagem de erro no monitor serial.
    if (SD.begin())
    { 
      Serial.println("SD Card pronto para uso.");
    }
    else
    {
      Serial.println("Falha na inicialização do SD Card.");
      return;
    }
}



// Na função loop, iniciaremos a apresentação da lógica de funcionamento dos dispositivos e controle completo do sistema. 
void loop() 
{

    // O sistema irá realizar a leitura dos botões de Início e Término.
    bool BotaoInicia = digitalRead(INICIA);
    bool BotaoTermina = digitalRead(TERMINA);

    // Quando o usuário pressionar o Botão Iniciar, o fluxo de código entrará na condição a seguir.
    if(BotaoInicia == 1)
    {
      // Será atribuído 1 à variável controle. Essa variável será utilizada para liberar a gravação dos dados no Módulo SD Card.
      controle = 1;
      
      // Cria e abre o arquivo para escrita
      myFile = SD.open("GPS.txt", FILE_WRITE);
      
      // Determina intervalo de tempo de 1 segundo para escrita.
      delay(1000);

      // Escrevemos os nomes das variáveis a fim de formar as 11 colunas dos dados recebidos do GPS.
      myFile.print("Latitude");
      myFile.println("  Longitude");
      myFile.println("  Altitude");
      myFile.println("  Velocidade");
      myFile.println("  sentido(grau)");
      myFile.println("  dia");
      myFile.println("  mes");
      myFile.println("  ano");
      myFile.println("  hora");
      myFile.println("  minuto");
      myFile.println("  segundo");

    }

    // Após chegar ao local desejado, o usuário deverá apertar o botão de término, para finalizar a gravação dos dados de percurso.
    if(BotaoTermina == 1)
    {
      // Primeiramente, a variável controle será colocada para 0.
      controle = 0;

      // Desse modo, o sistema irá parar de gravar dados no cartão de memória.
      myFile.close();

      // Em seguida, o arquivo será fechado e o LED Vermelho será desligado, a fim de informar que o sistema parou de capturar dados do módulo GPS.
      digitalWrite(LEDVERMELHO, LOW);
    }

    // Agora, o fluxo entrará na condição de gravação dos dados do GPS no módulo.
    if(controle == 1)
    {
        // Após o GPS receber os dados, o sistema irá ligar o LED Vermelho. O LED irá sinalizar a coleta e gravação de dados no módulo SD Card.
        while (SerialGPS.available())
        {
          if (GPS.encode(SerialGPS.read())) 
          {
            digitalWrite(LEDVERMELHO, HIGH);
            
            // Posteriormente, é processado as posições de latitude e longitude...
            GPS.f_get_position(&lat, &lon);

            // É atribuido o valor da altitude...
            alt = GPS.f_altitude();

            // É atribuido o valor da velocidade...
            vel = GPS.f_speed_kmph();   //km/h

            // É atribuido o valor do sentido em graus...
            sentido = GPS.course();

            // É atribuido os valores das variáveis de tempo...
            GPS.crack_datetime(&ano, &mes, &dia, &hora, &minuto, &segundo);
            
            // Por fim, de acordo com as linhas de código a seguir, os dados serão escritos no arquivo de texto.
            myFile.print(lat  , 6);
            myFile.println(lon, 6);
            myFile.println(alt, 6);
            myFile.println(vel, 6);
            myFile.println(float(sentido) / 100 , 2);
            myFile.println(dia);
            myFile.println(mes);
            myFile.println(ano);
            myFile.println(hora);
            myFile.println(minuto);
            myFile.println(segundo);
            
            // A leitura e gravação é realizada a cada 1 segundo.
            delay(1000);
           }
        }
    }
}
