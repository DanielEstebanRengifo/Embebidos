//NO ES LA VERSION FINAL ES UN BORRADOR 
#include "StateMachineLib.h"
#include "DHT.h"
#include "AsyncTaskLib.h"
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Declaracion Pines
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
//----------------------------
int pinLed = 2;
int PinLuz = 34;
//---------------------------
float ValueTemperatura;
float ValueHumedad;
int ValueLuz;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Declaracion de funciones 
void readTHfunct (void);
void readLuzdfunct (void);
void LED (void);
void DisplayData (void);
//Retardos----------------
void TimeOut3(void);
void TimeOut5(void);
void TimeOut6(void);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Declaracion tareas 
AsyncTask readTHfunct_Task(3200, true, readTHfunct); //cambie de 2500 a 3200
AsyncTask readLuzTask(2500, true, readLuzdfunct);
AsyncTask ledTask(500, true, LED);
AsyncTask displayDataTask(2000, true, DisplayData);
//Retardos---------------
AsyncTask TimeOut3Task(3000, false, TimeOut3);
AsyncTask TimeOut5Task(5000, false, TimeOut5);
AsyncTask TimeOut6Task(6000, false, TimeOut6);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Create new StateMachine
//Estados
enum State
{
	Ambiental = 1,
	Luz = 2,
	Alarma = 3,
};
//Comandos Estados
enum Input
{
	singT= 0,
	singL = 1,
	singH = 2,
	Unknown = 3,
};
StateMachine stateMachine(3, 5);
Input input;
void setupStateMachine() {
  // Add transisiones
	stateMachine.AddTransition(Ambiental, Luz, []() { return input == singT; });
	stateMachine.AddTransition(Luz, Ambiental, []() { return input == singT; });

	stateMachine.AddTransition(Ambiental, Alarma, []() { return input == singH; });
	stateMachine.AddTransition(Luz, Alarma, []() { return input == singL; });

	stateMachine.AddTransition(Alarma, Ambiental, []() { return input == singT; });
  //--------------------------------------------------------------------------------
  stateMachine.SetOnEntering(Ambiental, InAmbiental);
	stateMachine.SetOnEntering(Luz, InLuz);
	stateMachine.SetOnEntering(Alarma,InAlarma);
  //--------------------------------------------------------------------------------
  //Despues se usara ?
  stateMachine.SetOnLeaving(Ambiental, OutAmbiental);
	stateMachine.SetOnLeaving(Luz, OutLuz);
	stateMachine.SetOnLeaving(Alarma, OutAlarma);
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() 
{
	Serial.begin(115200);
  dht.begin();
  pinMode(pinLed, OUTPUT);
  pinMode(PinLuz, INPUT); 
	Serial.println("Starting State Machine...");
	setupStateMachine();	
	Serial.println("Start Machine Started");
	// Initial state
	stateMachine.SetState(Ambiental, false, true);
  displayDataTask.Start();
}

void loop() {

  readTHfunct_Task.Update();
  readLuzTask.Update();
  ledTask.Update();
  displayDataTask.Update();

  TimeOut3Task.Update();
  TimeOut5Task.Update();
  TimeOut6Task.Update();



	stateMachine.Update();
  input=Unknown;
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Estados Entrada y salida 
//:Entrada
void InAmbiental(void)
{
	Serial.println("Ambiental_Entra");
  readTHfunct_Task.Start();
  TimeOut5Task.Start();
}
void InLuz(void)
{
	Serial.println("Luz_Entra");
  readLuzTask.Start();
  TimeOut3Task.Start();
}
void InAlarma(void)
{
	Serial.println("Alarma_Entra");
  TimeOut6Task.Start();
  ledTask.Start();
}
//:Salida--------------------------------
void OutAmbiental(void)
{
  readTHfunct_Task.Stop();
  TimeOut5Task.Stop();
}
void OutLuz(void)
{
  readLuzTask.Stop();
  TimeOut3Task.Stop();
}
void OutAlarma(void)
{
  ledTask.Stop();
  TimeOut6Task.Stop();
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Funciones lectura
void readTHfunct (void){
  ValueTemperatura = dht.readTemperature();
  ValueHumedad = dht.readHumidity();
  if (ValueTemperatura>24 && ValueHumedad>70){
    input = singH;
  }
}
void readLuzdfunct (void){
  ValueLuz = analogRead(PinLuz);
  if (ValueLuz>500){
    input = singL;
  }
}
void LED (void){
  digitalWrite(pinLed, HIGH); 
  delay(1000);                    
  digitalWrite(pinLed, LOW);   
  delay(1000); 
}
//--------------------------------------------
void DisplayData(void)
{
	Serial.print("Humidity: ");
  Serial.print(ValueHumedad);
  Serial.print("||");
  Serial.print("Temperature: ");
  Serial.print(ValueTemperatura);
  Serial.print("°C");
  Serial.print("||");
  Serial.print(ValueLuz);
  Serial.println("||");
}
//retardos-------------------------------------------
void TimeOut3 (void){
  input = singT;
}
void TimeOut5 (void){
  input = singT;
}
void TimeOut6 (void){
  input = singT;
}











