#include <Servo.h> //Die Servobibliothek wird aufgerufen. Sie wird benötigt, damit die Ansteuerung des Servos vereinfacht wird.


Servo servoblau; //Erstellt für das Programm ein Servo mit dem Namen „servoblau“



void setup()

{

  servoblau.attach(7); //Das Setup enthält die Information, dass das Servo an der Steuerleitung (gelb) mit Pin 8 verbunden wird. Hier ist natürlich auch ein anderer Pin möglich.

}



void loop()

{ //Im „loop“ wird über den write-Befehl „servoblau.write(Grad)“ das Servo angesteuert. Zwischen den einzelnen Positionen gibt es eine Pause, damit das Servo genug Zeit hat, die gewünschten Positionen zu erreichen.

  servoblau.write(0); //Position 1 ansteuern mit dem Winkel 0°

  delay(3000); //Das Programm stoppt für 3 Sekunden

  servoblau.write(90); //Position 2 ansteuern mit dem Winkel 90°

  delay(3000); //Das Programm stoppt für 3 Sekunden

  servoblau.write(180); //Position 3 ansteuern mit dem Winkel 180°

  delay(3000);//Das Programm stoppt für 3 Sekunden

  servoblau.write(20); //Position 4 ansteuern mit dem Winkel 20°

  delay(3000);//Das Programm stoppt für 3 Sekunden

}
