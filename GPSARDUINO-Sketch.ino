//Desenvolvimento: André Escarião, Estudante de Engenharia Elétrica(UFCG), Processo Seletivo Parahyasas
//Projeto GPS e SD para Arduino:
//Código que faz comunicação serial com o módulo GPS e SD. Com a utilização de três bibliotecas <SoftwareSerial.h>, <TinyGPS.h> e <SD.h> a IDE recebe informações
//do módulo GPS - latitude, longitude, precisão da informação, dia, hora, altitude, velocidade e sentido de deslocamento - por meio de satélites e armazena no cartão SD.



//Biblioteca que já vem com o arduino que faz com que as portas digitais funcionem como porta serial
#include <SoftwareSerial.h> 
//Biblioteca que recebe pacote de informações do módulo através da softwareserial e facilita a compreensão (interpreta os dados) / é válida pra qualquer módulo
#include <TinyGPS.h>
//Biblioteca do Cartão SD para gravar na memória
#include <SD.h>  
#include <SPI.h>

//Instância um objeto "serial1" para fazer comunicação / definição de portas RX=10 e TX=11
SoftwareSerial serial1(10, 11);
//Insstância um objeto "gps1" para interpretação dos dados recebidos
TinyGPS gps1;

//Comunicação serial
void setup() {
   // velocidade do gps =9600
   serial1.begin(9600);
   //monitor serial (pode configurar outras velocidades)
   Serial.begin(9600);

   Serial.println("O GPS do Parahyasas aguarda pelo sinal dos satelites...");
}

//Uso do módulo
void loop() {
  bool recebido = false;

  //Verificar se tem alguma informação disponível ( se tiver, recebe do módulo e envia para o objeto "gps1" )
  while (serial1.available()) {
     char cIn = serial1.read();
     recebido = gps1.encode(cIn);
  }

//Se receber as informações por completo, ler as informações
  if (recebido) {
     Serial.println("----------------------------------------");
     
     //Declaração das variáveis: Latitude e Longitude
     long latitude, longitude;
     //Variável: idade da informação (quanto tempo faz que a informação foi gerada pelo satélite até chegar ao gps para saber a precisão da informação em milisegundos)
     unsigned long idadeInfo;
     
     //Função get_position que guarda as informações nas variáveis(latitude,longitude e idade da informação)
     gps1.get_position(&latitude, &longitude, &idadeInfo);     

     //Controle para Verificar se as variáveis(latitude,longitude,idadeInfo) estão válidas
     //Se as informações forem válidas, imprime os valores das variáveis no serial
     if (latitude != TinyGPS::GPS_INVALID_F_ANGLE) {
        Serial.print("Latitude: ");
        Serial.println(float(latitude) / 100000, 6);
     }

     if (longitude != TinyGPS::GPS_INVALID_F_ANGLE) {
        Serial.print("Longitude: ");
        Serial.println(float(longitude) / 100000, 6);
     }

     if (idadeInfo != TinyGPS::GPS_INVALID_AGE) {
        Serial.print("Idade da Informacao (ms): ");
        Serial.println(idadeInfo);
     }


     //Declaração de variáveis: Dia, Hora...
     int ano;
     byte mes, dia, hora, minuto, segundo, centesimo;
     
     //Função "crack_datetime" atualiza todas as variáveis
     gps1.crack_datetime(&ano, &mes, &dia, &hora, &minuto, &segundo, &centesimo, &idadeInfo);

     Serial.print("Data (GMT): ");
     Serial.print(dia);
     Serial.print("/");
     Serial.print(mes);
     Serial.print("/");
     Serial.println(ano);

     Serial.print("Horario (GMT): ");
     Serial.print(hora);
     Serial.print(":");
     Serial.print(minuto);
     Serial.print(":");
     Serial.print(segundo);
     Serial.print(":");
     Serial.println(centesimo);


     //Declaração da variável: Altitude
     float altitudeGPS;
     altitudeGPS = gps1.f_altitude();

     //Saber em que altitude está o gps ao nível do mar(cm)
     if ((altitudeGPS != TinyGPS::GPS_INVALID_ALTITUDE) && (altitudeGPS != 1000000)) {
        Serial.print("Altitude (cm): ");
        Serial.println(altitudeGPS);
     }


     //Declaração de variável: velocidade (OBS: km/h)
     float velocidade;
     //velocidade = gps1.speed();        //nós
     velocidade = gps1.f_speed_kmph();   //km/h
     //velocidade = gps1.f_speed_mph();  //milha/h
     //velocidade = gps1.f_speed_mps();  //milha/segundo

     Serial.print("Velocidade (km/h): ");
     Serial.println(velocidade, 2);  //Conversão de Nós para Km/h



     //Declaração de variável: sentito (em centesima de graus)
     unsigned long sentido;
     
     //Comando course guarda em variável sentido em quantos graus estou me deslocando
     sentido = gps1.course();

    //Saber em qual sentido estou(Norte,Sul,Leste,Oeste)
     Serial.print("Sentido (grau): ");
     Serial.println(float(sentido) / 100, 2);


     //Declaração de variáveis: satélites e precisão
     unsigned short satelites;
     unsigned long precisao;

     //Comando para reconhecer satélites
     satelites = gps1.satellites();
     //Comando para precisão da informação recebida dos satélites
     precisao =  gps1.hdop();

     if (satelites != TinyGPS::GPS_INVALID_SATELLITES) {
        Serial.print("Satelites: ");
        Serial.println(satelites);
     }

     if (precisao != TinyGPS::GPS_INVALID_HDOP) {
        Serial.print("Precisao (centesimos de segundo): ");
        Serial.println(precisao);
     }


     //Comando para saber distância entre dois pontos
     //float distancia_entre;
     //distancia_entre = gps1.distance_between(lat1, long1, lat2, long2);

     //Comando para saber do ponto1 para o ponto2 qual é o sentido do deslocamento em uma linha reta
     //float sentido_para;
     //sentido_para = gps1.course_to(lat1, long1, lat2, long2);

  }


  
}


//////////////////////////

/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/



File myFile;


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  //Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void loop() {
  // nothing happens after setup
}