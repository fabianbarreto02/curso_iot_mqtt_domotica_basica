// Proyecto basado en el el proyecto Hello_word titulado 
// esp32_ioticos_broker_hello_world, por la plataforma 
// ioticos disponible en el repositorio 
// https://github.com/ioticos/esp32_ioticos_broker_hello_world/blob/master/src/main.cpp
// y adaptado al Curso de Domotica basica con Tecnologia IoT
// presentado por el Ing. Fabian Barreto docente 
// tiempo completo  UNISANGIL


// Importar de libreas necesarias para el proyecto
#include <Arduino.h>
#include <WiFi.h>  // Libreria de conexión WIFI
#include <PubSubClient.h> // Libreria para configuración y conexión de la comunicación MQTT
#include "DHT.h"  // Libreria para el modulo de temperatura y humedad DHT11

// Definición de Pines digitales necesario y configuración de sensorer DHT11
#define DHTPIN 4 // Definición del Pin lectura sensor 
#define DHTTYPE DHT11 // Configuración del tipo sensor

DHT dht(DHTPIN, DHTTYPE); // Definición del constructor 


//*********** Comunicación MQTT CONFIGURACIÓN **************
//**************************************
const char *mqtt_server = "broker.emqx.io"; // host de conexión al broker o URL del Servidor 
const int mqtt_port = 1883; // Puerto de coneción 
const char *root_topic_subscribe = "Led/esp32"; // Topico de subscripción 
const char *root_topic_publish = "Temperatura/public_esp32"; // Topico para manejo de temperatura


//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "UNISANGIL YOPAL";
const char* password =  "";



//**************************************
//*********** Variables GLOBALES   ***************
//**************************************
WiFiClient espClient; // Definición del constructor para el internet 
PubSubClient client(espClient); // Definición del constructor para el broker

int ledpin =2; // Definimos el pin para el led interndo de la esp32

int var = 0; // variable auxiliar 
int ledval = 0; // valor incial del led apagado
char datos[40]; // variable para almacenar los mensajes
String resultS = ""; // Variable para los resultados 




//************************
//** Definición de F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length); // Función para convertir los mensajes recibidos 
void reconnect();  //  Función para reconexión a broker 
void setup_wifi(); // Función para conexión a la red WIFI 


// Función Setup, se ejecuta una vez en el programa
void setup() {
  pinMode(ledpin,OUTPUT); // Definición de puerto como salida para prender el led
  Serial.begin(9600); // Se inicia la comunicación serial
  /// Iniciación de sensores
  dht.begin(); // Inicia el funcionamiento del sensor DHT11
  delay(100);// Tiempo para Inicios de los proceso 
  setup_wifi(); // Metodo para conectarse a la red WIFI
  client.setServer(mqtt_server, mqtt_port); // Definición de servidor broker Mqtt
  client.setCallback(callback); // Defición de la función Callback para recibir mensajes 

  
}

void loop() {
  // Funcion para conectar al broker mqtt
  if (!client.connected()) {
    reconnect(); // Función de reconección al broker
  }

  Serial.print("Datos desde el Cliente PC: "); // Imprime en consola la información enviada desde el cliente pc
  Serial.println(resultS);

  if (client.connected()){

    if(var == 0)
    {
      digitalWrite(ledpin,LOW); // Apaga el led cuando recibe un cero desde el pc
    }
     else if (var == 1) 
    {
      digitalWrite(ledpin,HIGH);// Prende el led  cuando recibe un uno desdel pc
    }

    //Lectura de temperatura y humedad
    float hum = dht.readHumidity();
    float tem = dht.readTemperature();
    
    // Creación de sting para enviar 

     Serial.println("Datos del Sensor DHT11: Temperatura: "  + String(tem) +"°C, Humedad: "+ String(hum) + "%");

     String tem_string = String(tem); // Convierte los datos en string
     String hum_string = String(hum);

     sprintf(datos, "Valor Temperatura: %s °C, Valor Humedad: %s" ,tem_string,hum_string ); // Concatenar las mediciones de temperatura y humedad 
     client.publish(root_topic_publish,datos); // publica los datos en el broker
     delay(2000); // Tiempo de espera de 2 segundos
  }
  // Función loop para el envio de datos
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

  // Inicia la conección al wifi

  WiFi.begin(ssid, password);

// Espera a que se conecte a la red
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
// se conecta a la red
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
    Serial.print("Intentando conectarse MQTT...");
    
    // Intentamos conectar
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
      // Error de conexión 
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
  // Imprime en consola el mensaje recibido desde el pc
  String incoming = "";
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
 
 char payload_string[length + 1];
  
  int resultI;
// Organiza la información resibida 
  memcpy(payload_string, payload, length);
  payload_string[length] = '\0';
  resultI = atoi(payload_string);
  var = resultI;

  resultS = "";
  // Concatena caractera caracter el mensaje recibido 
  for (int i=0;i<length;i++) {
    resultS= resultS + (char)payload[i];
  }
  Serial.println();
}
