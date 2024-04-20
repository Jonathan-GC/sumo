
/*  IZQ                                    DER
  _________                             __________
  |  19   | <--- Atras                  |   18   | <--- Atras
  |  21   | <--- Adelante               |    5   | <--- Adelante
  |       |-----------------------------|        |
  |       |                             |        | 
  |_______|                             |________|
      |                                      |
      |                                      |
      |        4                    15       |     --------
      | __     |                     |    __ |     |  14  |  Stop
      |_______ ^ ___________________ ^ ______|     |      |
               ^                     ^             |  12  |  Start
               ^                     ^             --------

*/



#define cantDatos 31
#define mediana int(cantDatos / 2)

#define pinIzqAdel 21
#define pinIzqAtra 19
#define pinDerAdel 5
#define pinDerAtra 18
#define pinSenIzq 4
#define pinSenDer 15
#define pinStart 12
#define pinStop 14

int lecturasIzq[cantDatos];
int lecturasDer[cantDatos];
int salida = 100;
unsigned long tiempo = 0, tiempoAnterior = 0, tiempoAnterior2 = 0;
int tiempoDiscreto = 20;

int lecturaIzq;
int lecturaDer; 
int lectIzqAnt = 0, lectDerAnt = 0;
int pendienteIzq, pendienteDer, pendienteIzqAnt, pendienteDerAnt;
int fluctuaciones = 0, ruido = false;
int minIzquierdo = 9999, maxIzquierdo = 0; 
int minDerecho = 9999, maxDerecho = 0;
int promedioDeMax, promedioDeMin;
int zonaExlusionIzquierdo = 1300, zonaExlusionDerecho = 1500;
void setup() {
  pinMode(pinIzqAdel, OUTPUT);
  pinMode(pinIzqAtra, OUTPUT);
  pinMode(pinDerAdel, OUTPUT);
  pinMode(pinDerAtra, OUTPUT);
  pinMode(pinSenIzq, INPUT);
  pinMode(pinSenDer, INPUT);
  pinMode(pinStart, INPUT);
  pinMode(pinStop, INPUT);
  Serial.begin(115200);

  rangoDelruido();
}

void frenar() {
  digitalWrite(pinIzqAdel, 1);
  digitalWrite(pinDerAdel, 1);
  digitalWrite(pinIzqAtra, 1);
  digitalWrite(pinDerAtra, 1);
}

void parar() {
  digitalWrite(pinIzqAdel, 0);
  digitalWrite(pinDerAdel, 0);
  digitalWrite(pinIzqAtra, 0);
  digitalWrite(pinDerAtra, 0);
}

void atras() {
  digitalWrite(pinIzqAdel, 1);
  digitalWrite(pinDerAdel, 1);
  digitalWrite(pinIzqAtra, 0);
  digitalWrite(pinDerAtra, 0);
}

void adelante() {
  digitalWrite(pinIzqAdel, 0);
  digitalWrite(pinDerAdel, 0);
  digitalWrite(pinIzqAtra, 1);
  digitalWrite(pinDerAtra, 1);
}

void derecha() {
  digitalWrite(pinIzqAdel, 0);
  digitalWrite(pinDerAdel, 1);
  digitalWrite(pinIzqAtra, 1);
  digitalWrite(pinDerAtra, 0);
}

void Izquierda() {
  digitalWrite(pinIzqAdel, 1);
  digitalWrite(pinDerAdel, 0);
  digitalWrite(pinIzqAtra, 0);
  digitalWrite(pinDerAtra, 1);
}


void rangoDelruido(){
  Serial.println("Analizando Ruido del sistema");
  //Prender el led de calibracion
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);

  //RECOGER 4000 MUESTRAS
  for (int i = 0; i < 8000; i++ ){
    
    filtrarData();

    // buscandoMinimos
    if (lecturaIzq < minIzquierdo)
      minIzquierdo = lecturaIzq;
    if (lecturaDer < minDerecho)
      minDerecho = lecturaDer;
    
    // buscandoMaximos
    if (lecturaIzq > maxIzquierdo)
      maxIzquierdo = lecturaIzq;
    if (lecturaDer > maxDerecho)
      maxDerecho = lecturaDer;

    
  }

  promedioDeMax = (maxIzquierdo + maxDerecho)/2;
  promedioDeMin = (minIzquierdo + minDerecho)/2;

  Serial.println("Izquierdo min/max\t\tDerecho min/max");
  Serial.print(minIzquierdo); Serial.print(" / "); Serial.print(maxIzquierdo);Serial.print("\t");
  Serial.print(minDerecho); Serial.print(" / "); Serial.print(maxDerecho);
  Serial.print("\t");
  Serial.print(promedioDeMin); Serial.print(" / "); Serial.println(promedioDeMax);


  digitalWrite(LED_BUILTIN, 0);

}

void swap(int arr[], int i, int j) {
  int temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

int partition(int arr[], int low, int high) {
  int pivot = arr[high];
  int i = low - 1;
  for (int j = low; j < high; j++) {
    if (arr[j] < pivot) {
      i++;
      swap(arr, i, j);
    }
  }
  swap(arr, i + 1, high);
  return i + 1;
}

void quickSort(int arr[], int low, int high) {
  if (low < high) {
    int pi = partition(arr, low, high);
    quickSort(arr, low, pi - 1);
    quickSort(arr, pi + 1, high);
  }
}


void filtrarData(){
  for (int i = 0; i < cantDatos; i++) {
    lecturasDer[i] = analogRead(pinSenDer);
    lecturasIzq[i] = analogRead(pinSenIzq);
  }

  quickSort(lecturasDer, 0, cantDatos - 1);
  quickSort(lecturasIzq, 0, cantDatos - 1);

  int *plecturaIzq = &lecturasIzq[mediana];
  int *plecturaDer = &lecturasDer[mediana];

  lecturaIzq = *plecturaIzq;
  lecturaDer = *plecturaDer;

}

void leerSensores() {
  
  //Leer los datos de los sensores
  filtrarData();

  // Extraer la diferencia de ambos senspores para saber como se debe comportar
  int difSensores = abs(lecturaIzq-lecturaDer);
  
  if (lecturaIzq > 2000 && lecturaDer>2000)
    salida = 0;
  else if(difSensores < (promedioDeMax-promedioDeMin)-50)
    salida = 0;
  else if (lecturaIzq > maxIzquierdo && lecturaIzq > lecturaDer)
    salida = -1;
  else if(lecturaIzq < minIzquierdo && lecturaIzq < lecturaDer)
    salida = -1;
  else if (lecturaDer > maxDerecho && lecturaDer > lecturaIzq)
    salida = 1;
  else if (lecturaDer < minDerecho && lecturaDer < lecturaIzq)
    salida = 1;
  else if (lecturaIzq > minIzquierdo and lecturaIzq < maxIzquierdo && lecturaDer > minDerecho and lecturaDer < maxDerecho )
    salida = 3000;
  
  

  //--------------------------------------------------------------
/*
  if( (lecturaIzq > 2000 and lecturaDer > 2000) and  abs(lecturaIzq - lecturaDer) < 100 ){
    salida = 0;
  }
  
  else if (lecturaIzq > lecturaDer + maxIzquierdo) {
      salida = -1;
  } 
  else if (lecturaDer > lecturaIzq + 200) {
      salida = 1;
  } 
  else{
    
      int acercamiento = lecturaIzq > zonaExlusionIzquierdo + 250 and lecturaDer > zonaExlusionDerecho + 250 or lecturaIzq < zonaExlusionIzquierdo and lecturaDer < zonaExlusionDerecho;
      if (acercamiento)
        salida = 0;
      else
        salida = 3000;
  }
*/


  Serial.print("I:0\tF:4095");
  Serial.print("\tIZQ:");
  Serial.print(lecturaIzq);
  Serial.print("\tDER:");
  Serial.print(lecturaDer);
  Serial.print("\tOUT:");
  Serial.print(salida);
  Serial.print("\t pendiente:");
  Serial.println(pendienteIzq);
  
}


void funcionPrincipal() {
  if (salida == 0) {
    adelante();
  } else if (salida == -1) {
    derecha();
  } else if (salida == 1) {
    Izquierda();
  } else {
    parar();
  }
}


void loop() {

  int lecturaStart = digitalRead(pinStart);
  int lecturaStop = digitalRead(pinStop);
  leerSensores();

  if (lecturaStart == 1 and lecturaStop == 1) {
    funcionPrincipal();
  } else {
    parar();
  }
}
