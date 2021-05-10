// Importar de libreas necesarias 
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// defines pins numbers
const int trigPin = 2;
const int echoPin = 5;

// defines variables
long duration;
int distance;
String stringapublicar; 
String temperatura;


//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "ioticos.org";
const int mqtt_port = 1883;
const char *mqtt_user = "MCi5y3W4gM2p038";
const char *mqtt_pass = "PVok4vVgerhptvV";
const char *root_topic_subscribe = "CiyVn6Ykbe6eD7i/input";
const char *root_topic_publish = "CiyVn6Ykbe6eD7i/output";
const char *root_topic_publish_temperatura = "CiyVn6Ykbe6eD7i/temperatura";


//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "FAMILIA BARRETO SANCHEZ";
const char* password =  "GERMAN2018BARRETO";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[57];
char msgTem[20];



//************************
//** Definición de F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  /// Iniciación de sensores
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  dht.begin();
}

void loop() {
  // Funcion para conectar al broker mqtt
  if (!client.connected()) {
    reconnect();
  }

  if (client.connected()){

    // Limpieza de los pines the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);
    
    // Establece el trigPin en estado ALTO durante 10 microsegundos
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Lee el echoPin, devuelve el tiempo de viaje de la onda de sonido en microsegundos
    duration = pulseIn(echoPin, HIGH);

    // Calculo de la distacia en cm
    distance= duration*0.034/2;


    //Lectura de temperatura y humedad
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    stringapublicar = "Distancia: " + String(distance) + " Cm; "+"Humedad: "+ h+"%;"+ "Temperatura: " + t +"°C";
    temperatura = "Temperatura: " + String(t) +"°C";

    
    // no se pueden enviar un string al broker, se convierte en un char array
    stringapublicar.toCharArray(msg,57);
    client.publish(root_topic_publish,msg);



    temperatura.toCharArray(msgTem,20);
    client.publish(root_topic_publish_temperatura,msgTem);
    delay(3000);
  }
  // Se envie la broker ojo con este funcion 
  client.loop();
}




//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID ojo si son varios clientes definir diferentes clienId
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX); // se genera un cliente id ramdom para evitar problemas 
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//*****************************
//***       CALLBACK        ***
//*****************************
// FUncion para recibir mensajes del broker
void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  // Recuperar caracter por caracter del mensaje recibido del broker
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje broker -> " + incoming);

}
