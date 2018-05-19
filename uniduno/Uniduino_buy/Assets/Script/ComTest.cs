//Basic libraries 
using UnityEngine; 
using System.Collections; 
#region Using directives 
//Other libraries 
using System; 
using System.Collections.Generic; 
using System.ComponentModel;  
//Serial Port library. 
using System.IO.Ports; 
#endregion 
public class ComTest : MonoBehaviour { 
    
   //----- 
   // Some other variable definition code 
        // could be added in here. 
   //----- 
   private SerialPort sp; 
	public string portName="COM4";
	public byte b=0;
    
   // Use this method for initialization. 
   void Start () { 
      
      //----- 
      // Inialization of other variables in here. 
      //----- 
      // Port set up with some basic settings. 
                
                // was in Port COM1 with a baud rate of 9600. 
      sp = new SerialPort( portName 
                                   , 9600 
                                   , Parity.None 
                                   , 8 
                                   , StopBits.One); 
    
      // Open the port for communications 
      sp.Open(); 
           // Set read time out to 50 ms. 
                // This value might be too high actually. 
           sp.ReadTimeout = 50; 
      
   } 
   void OnApplicationQuit() { 
      // Closes the port - this does not 
                // seem to work for me. 
      // Perhaps it should be placed elsewhere. 
      sp.Close(); 
      
   } 
   // Update is called once per frame 
   void FixedUpdate() { 
      
              // Read serial port data 
         // Mono serial class doesn't implement serial events or give the correct 
          // response to no data available 
          // If there's no data it throws a timeout error so we catch that error and do nothing 
//      string tempS = ""; 
//        
//      try { 
//        byte tempB =  (byte) sp.ReadByte(); 
//         while (tempB != 255) { 
//            tempS += ((char) tempB); 
//            tempB = (byte) sp.ReadByte(); 
//         } 
//      } 
//      catch (Exception e) { 
//          
//      } 
//       if (tempS!="") { 
//           Debug.Log("serial out "+tempS); 
//         } 
    
//		string tempS = sp.ReadLine(); 
		byte[] i = new byte[1] { 0 };
		i[0]=b;
		sp.Write(i, 0, 1);
		
		//sp.WriteByte(i);
   } 
} 




