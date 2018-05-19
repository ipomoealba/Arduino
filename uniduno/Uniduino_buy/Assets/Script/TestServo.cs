using UnityEngine;
using System.Collections;
using Uniduino;

public class TestServo : MonoBehaviour {
	
	public Arduino arduino;
	
	public int motorPin = 9;
	public int analogInput = 0;
	
	public int motorDegree = 0;
	public int targetDegree = 0;
	// Use this for initialization
	
	void Start () {
		
		arduino = Arduino.global;
		arduino.Setup( ConfigurePins );
		
		StartCoroutine( loop() );
	
	}
	
	// Update is called once per frame
	void ConfigurePins ()
	{
		arduino.queryCapabilities();
		
		arduino.pinMode( motorPin, PinMode.SERVO );
		arduino.pinMode( analogInput, PinMode.ANALOG );
		arduino.reportAnalog( analogInput, 1 );
		
	}
	
	IEnumerator loop()
	{
		while( true )
		{
			// read Analog, and decide what degree to go
			int val = arduino.analogRead( analogInput );
			float targetDegree = Mathf.Lerp( 0, 180, (float)val/1024.0f );
			
			
			// Control Motor
			// Can't go to the degree too fast, move 1 degree in 10 ms
			motorDegree += (int)Mathf.Max( -1, Mathf.Min( 1,( targetDegree - motorDegree ) ) );
			arduino.analogWrite( motorPin, motorDegree );
			
			yield return new WaitForSeconds( 0.1f );
		}
	}
}
