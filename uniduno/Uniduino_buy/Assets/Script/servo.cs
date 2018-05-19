using UnityEngine;
using System.Collections;
using Uniduino;
 
public class Servo : MonoBehaviour {
 
    public Arduino arduino;
 
    public int pin = 9;
 
    void Start( )
    {
        arduino = Arduino.global;
        arduino.Setup(ConfigurePins);
 
        StartCoroutine(loop());
    }
 
    void ConfigurePins( )
    {
        arduino.pinMode(pin, PinMode.SERVO);
    }
 
    IEnumerator loop()
    {
        while(true)
        {                  
            arduino.analogWrite(pin, 0);
            yield return new WaitForSeconds(1);
             
            arduino.analogWrite(pin, 3);
            yield return new WaitForSeconds(1);                
        }
    }      
}   