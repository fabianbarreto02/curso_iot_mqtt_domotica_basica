// Proyecto basado en el el proyecto Hello_word titulado 
// esp32_ioticos_broker_hello_world, por la plataforma 
// ioticos disponible en el repositorio 
// https://github.com/ioticos/esp32_ioticos_broker_hello_world/blob/master/src/main.cpp
// y adaptado al seminario de Domotica basica con Tecnologia IoT
// presentado por el Ing. Fabian Barreto docente 
// tiempo completo de la Unisangil


// Importar de libreas necesarias para el proyecto
#include <Arduino.h>
#include <WiFi.h>  // Libreria de conexión WIFI
#include <PubSubClient.h> // Libreria para configuración y conexión de la comunicación MQTT
#include "DHT.h"  // Libreria para el modulo de temperatura y humedad DHT11

// Definición de Pines digitales necesario y configuración de sensorer DHT11
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Definición  pines Sensro de proximidad
const int trigPin = 2;
const int echoPin = 5;

// Definición de  variables para caltulo de distanción y temperatura 
long duration;
int distance;
String stringapublicar; 
String temperatura;


//*********** Comunicación MQTT CONFIGURACIÓN **************
//**************************************
const char *mqtt_server = "ioticos.org"; // host de conexión al broker o URL del Servidor 
const int mqtt_port = 1883; // Puerto de coneción 
const char *mqtt_user = "MCi5y3W4gM2p038"; // Usuario 
const char *mqtt_pass = "PVok4vVgerhptvV"; // Contraseña 
const char *root_topic_subscribe = "CiyVn6Ykbe6eD7i/input"; // Topico de subscripción 
const char *root_topic_publish = "CiyVn6Ykbe6eD7i/output"; // Topico para publicar 
const char *root_topic_publish_temperatura = "CiyVn6Ykbe6eD7i/temperatura"; // Topico para manejo de temperatura


//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "FAMILIA BARRETO SANCHEZ";
const char* password =  "GERMAN2018BARRETO";



//**************************************
//*********** Variables GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[57]; // Longitud del mensaje general
char msgTem[20];// Longitud del mensaje  de temperatura



//************************
//** Definición de F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length); // Función para convertir los mensajes recibidos 
void reconnect();  //  Función para reconexión a broker 
void setup_wifi(); // Función para conexión a la red WIFI 


// Función Setup principar del programa
void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); // Definición de servidor broker Mqtt
  client.setCallback(callback); // Defición de la función Callback para recibir mensajes 

  /// Iniciación de sensores
  pinMode(trigPin, OUTPUT); // Establece el trigPin como una salida
  pinMode(echoPin, INPUT); // Establece el echoPin como una entrada
  dht.begin(); // Inicia el funcionamiento del sensor DHT11
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
    
    // Creación de sting para enviar 
    stringapublicar = "Distancia: " + String(distance) + " Cm; "+"Humedad: "+ h+"%;"+ "Temperatura: " + t +"°C";
    temperatura = "Temperatura: " + String(t) +"°C";

    
    // No se pueden enviar un string al broker, se convierte en un char array
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
//***    CONEXION Y RECONECIÓN MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID ojo si son varios clientes definir diferentes clienId
    String clientId = "CLIENTE_ID_H_W_";
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
// Funcion para recibir mensajes del broker
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
