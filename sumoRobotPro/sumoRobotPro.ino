/*  IZQ                                    DER
  _________                             __________
  |  21   | <--- Atras                  |   18   | <--- Atras
  |  19   | <--- Adelante               |    5   | <--- Adelante
  |       |-----------------------------|        |
  |       |                             |        | 
  |_______|                             |________|
      |                                      |
      |                                      |
      |        4                    15       |
      | __     |                     |    __ |
      |_______ ^ ___________________ ^ ______|
               ^                     ^ 
               ^                     ^

*/



#define cantDatos 5
#define mediana int(cantDatos/2)

#define pinIzqAdel  19
#define pinIzqAtra  21
#define pinDerAdel   5
#define pinDerAtra  18
#define pinSenIzq    4
#define pinSenDer   15
#define pinStart    27
#define pinStop     26

int lecturasIzq[cantDatos];
int lecturasDer[cantDatos];
int salida = 100;
unsigned long tiempo = 0, tiempoAnterior = 0;
int tiempoDiscreto = 100;
int lectIzqAnt = 0, lectDerAnt = 0;
int pendienteIzq, pendienteDer; 

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


}

void frenar (){
  digitalWrite(pinIzqAdel, 1);
  digitalWrite(pinDerAdel, 1);
  digitalWrite(pinIzqAtra, 1);
  digitalWrite(pinDerAtra, 1);
}

void parar (){
  digitalWrite(pinIzqAdel, 0);
  digitalWrite(pinDerAdel, 0);
  digitalWrite(pinIzqAtra, 0);
  digitalWrite(pinDerAtra, 0);
}

void adelante(){
  digitalWrite(pinIzqAdel, 1);
  digitalWrite(pinDerAdel, 1);
  digitalWrite(pinIzqAtra, 0);
  digitalWrite(pinDerAtra, 0);
}

void atras(){
  digitalWrite(pinIzqAdel, 0);
  digitalWrite(pinDerAdel, 0);
  digitalWrite(pinIzqAtra, 1);
  digitalWrite(pinDerAtra, 1);
}

void derecha(){
  digitalWrite(pinIzqAdel, 0);
  digitalWrite(pinDerAdel, 1);
  digitalWrite(pinIzqAtra, 1);
  digitalWrite(pinDerAtra, 0);
}

void Izquierda(){
  digitalWrite(pinIzqAdel, 1);
  digitalWrite(pinDerAdel, 0);
  digitalWrite(pinIzqAtra, 0);
  digitalWrite(pinDerAtra, 1);
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



void leerSensores(){
 
  for(int i = 0; i < cantDatos ; i++){
    lecturasDer[i] = analogRead(pinSenDer);
    lecturasIzq[i] = analogRead(pinSenIzq);
  }

   quickSort(lecturasDer, 0, cantDatos-1);
   quickSort(lecturasIzq, 0, cantDatos-1);

  int lecturaIzq = (lecturasIzq[2]);
  int lecturaDer = (lecturasDer[2]);
  salida = lecturasDer-lecturasIzq;
  tiempo = millis();
  
  if(tiempo - tiempoAnterior > tiempoDiscreto){
    pendienteIzq = (lecturaIzq - lectIzqAnt)/tiempoDiscreto;
    
    lectIzqAnt = lecturaIzq;
    lectDerAnt = lecturaDer;
  
    tiempoAnterior = tiempo;
  }

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


void loop() {

  int lecturaStart = digitalRead(pinStart);
  int lecturaStop = digitalRead(pinStop);
  leerSensores();

  if(lecturaStart == 1 and lecturaStop == 1){
    adelante();
  }
  else{
    parar();
  }
  

  
  
}
