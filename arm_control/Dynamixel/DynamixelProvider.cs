using System;
using System.IO;
using System.IO.Ports;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using AgaveRobotics.Robotis;

namespace AgaveRobotics.Providers
{
    public class DynamixelProvider
    {

        // Declare the event using EventHandler<T>
        public event EventHandler<StatusErrorEventArgs> RaiseStatusErrorEvent;

        // Serial Port
        private SerialPort sPort;

        private String portname;
        private int baudrate;
        private StatusPacket statusPacket;
        private InstructionPacket instructionPacket;






        public DynamixelProvider()
        { 
            // Grab the port and baud from a configuration file
        }



        // Open Port
        protected void OpenPort(String portName, int baudRate)
        {
            // TODO: BetterS Serial port constructor validation

            // Validate the port name
            // port name look like COM1... COMnn
            if ((portName != null) && (portName.Contains("COM") && portName.Length > 3))
            {
                portname = portName;
            }

            // Create and open a serial port instance
            sPort = new SerialPort(portname, baudRate);
            sPort.Open();

            // wire up the event handler
            sPort.DataReceived += new SerialDataReceivedEventHandler(SerialPortDataReceived);

        }


        // Close Port
        protected void ClosePort()
        {
            // Close the port
            sPort.Close();
            
            //Unsubscribe to the serial port data received event
            sPort.DataReceived -= SerialPortDataReceived;
        }


        // Data Recieved handler
        private void SerialPortDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                byte[] buff = new byte[sPort.BytesToRead];

                int result = sPort.Read(buff, 0, sPort.BytesToRead);

                // Unit test error handler.
                //buff[4] = (byte)StatusError.Checksum;

                //Create a new status packet and wire up the event handler
                //TODO:  First we need to make sure that the data recived is a complete packet.  If not then we'll need to buffer up the bytes

                StatusPacket sp = new StatusPacket(buff);
                sp.RaiseStatusErrorEvent += HandleStatusError;

                if (sp.IsStatusErrors())
                {
                    // Do something useful here like resend the instruction packet
                }

                // Fire a status packet received event

            }
            catch (Exception)
            {
                //
            }
        }

        void HandleStatusError(Object sender, StatusErrorEventArgs e)
        {
            // Refire the status error event
            OnRaiseStatusErrorEvent(e);
        }


        // Wrap event invocations inside a protected virtual method
        // to allow derived classes to override the event invocation behavior
        protected virtual void OnRaiseStatusErrorEvent(StatusErrorEventArgs e)
        {
            // Make a temporary copy of the event to avoid possibility of
            // a race condition if the last subscriber unsubscribes
            // immediately after the null check and before the event is raised.
            EventHandler<StatusErrorEventArgs> handler = RaiseStatusErrorEvent;

            // Event will be null if there are no subscribers
            if (handler != null)
            {
                // Use the () operator to raise the event.
                handler(this, e);
            }



        }


        #region Properties
        public String Portname
        {
            get { return portname; }
            set { portname = value; }
        }

        public int Baudrate
        {
            get { return baudrate; }
            set { baudrate = value; }
        }

        public StatusPacket StatusPacket
        {
            get { return statusPacket; }
        }

        public InstructionPacket InstructionPacket
        {
            get { return instructionPacket; }
        }
        #endregion
        

    }// DynamixelProvider
}// AgaveRobotics.Providers
