using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Reflection;

namespace AgaveRobotics.Servos
{

    #region Status Error Enum
    /// <summary>
    /// Dynamixel status packet error enumeration
    /// </summary>
    [Flags]
    public enum StatusError : byte
    { 
        /// <summary>
        /// Instruction error flag
        /// </summary>
        [Description("Instruction Error")]
        Instruction = 0x40,

        /// <summary>
        /// Overload error flag
        /// </summary>
        [Description("Overload Error")]
        Overload = 0x20,

        /// <summary>
        /// Checksum error flag
        /// </summary>
        [Description("Checksum Error")]
        Checksum = 0x10,

        /// <summary>
        /// Range error flag
        /// </summary>
        [Description("Range Error")]
        Range = 0x08,

        /// <summary>
        /// Over heating error flag
        /// </summary>
        [Description("Overheating Error")]
        Overheating = 0x04,

        /// <summary>
        /// Angle limit error flag
        /// </summary>
        [Description("AngleLimit Error")]
        AngleLimit = 0x02,

        /// <summary>
        /// Input voltage error flag
        /// </summary>
        [Description("InputVoltage Error")]
        InputVoltage = 0x01
    }
    #endregion


    #region Class StatusPacket
    /// <summary>
    /// Provides custom constructors and methods to build a dynamixel status packet. 
    /// </summary>
    public class StatusPacketBuilder
    {
        /// <summary>
        /// Represents the method that will handle the status error of the StatusPacketBuilder object.
        /// </summary>
        public event EventHandler<StatusErrorEventArgs> RaiseStatusErrorEvent;

        /// <summary>
        /// Represents the method that will handle the status packet received event of the StatusPacketBuilder object.
        /// </summary>
        public event EventHandler<StatusPacketEventArgs> RaiseStatusPacketReceivedEvent;


        private List<byte> buffer = new List<byte>();


        #region Constructor

        public StatusPacketBuilder()
        {

        }

        #endregion


        #region DataReceived
        public void DataReceived(byte[] bytesReceived)
        {
            int startFrame = 0;
            int endFrame = 0;
            int bailout = 0;

            // Add the bytes received to the end of the buffer
            buffer.AddRange(bytesReceived);

            // Clean up the buffer.
            // The buffer should always start with 0xFF 0xFF
            startFrame = buffer.FindIndex(IsStartByte);
            // Remove all bytes starting at index 0 
            // to the first 0xFF
            if (startFrame > 0)
            {
                buffer.RemoveRange(0, startFrame + 1);
            }

            // The start frame should always start at index 0
            startFrame = buffer.FindIndex(IsStartByte);

            // The end frame will be 0 or the index of the 
            // last byte of the first status packet in the buffer.
            endFrame = GetEndFrameIndex(startFrame);
            
            // If the start frame and end frame buffer pointers
            // are the same (0) then we do not have a whole
            // status packet in the buffer
            while (startFrame != endFrame)
            {
                // Get the size of the status packet in the buffer
                int count = (endFrame - startFrame + 1);

                // Shadow copy the status packet from the buffer
                byte[] statusPacket = buffer.GetRange(startFrame, count).ToArray();

                // Remove the status packet from the buffer.
                buffer.RemoveRange(startFrame, count);

                // Initialize a status packet argument 
                // Which is passed to all listeners
                StatusPacketEventArgs statusPacketArgs = new StatusPacketEventArgs(statusPacket);

                // Raise the packet received event
                OnStatusPacketReceived(statusPacketArgs);

                // Check for errors
                if (statusPacketArgs.Error > 0)
                {
                    // Raise the Status error event
                    OnRaiseStatusErrorEvent(new StatusErrorEventArgs(statusPacket));
                }

                // Get the start and end pointers of the 
                // next status packet if one exists in the buffer
                startFrame = buffer.FindIndex(IsStartByte);
                endFrame = GetEndFrameIndex(startFrame);

                // Emergency bailout 
                if (bailout++ > 1000) { break; }
            }

            // Fire an application exception
            if (bailout >= 1000) 
            { 
                //buffer.Clear();
                throw new ApplicationException(String.Format("Runaway loop parsing the status packet buffer -> Dump buffer {0}",
                    BitConverter.ToString(buffer.ToArray())));
            }

        } // DataReceived
        #endregion
        
    
        #region GetEndFrameIndex
		private int GetEndFrameIndex(int startFrame)
        {
            int i = startFrame;
            int count = buffer.Count;
            byte lengthByte = 0;

            //  Check the next Byte for FF
            if (!IsOutOfRange(i + 1, count))
            {
                if (buffer[i + 1] == 0xFF)
                {
                    // Check if we have a length byte within range
                    // the length byte is 3 byte from the first start byte
                    if (!IsOutOfRange(i + 3, count))
                    {
                        lengthByte = buffer[i + 3];

                        // Ok now check for the end of the buffer
                        // length tells us how many more bytes to read 
                        // the last byte is the checksum.
                        if (!IsOutOfRange((i + 3 + lengthByte), count))
                        {
                            return i + 3 + lengthByte;
                        }
                    }
                }
            }
            // Well, we don't have whole status packet in the buffer
            return startFrame;
        }
	    #endregion
        

        #region IsOutOfRange
        // Check if the index we want to read exists in the buffer
        private static bool IsOutOfRange(int index, int count)
        {
            // Index is zero based so we add one
            index++;

            if (index <= count)
            {
                return false;
            }
            else 
            {
                return true;
            }
        }
        #endregion


        #region IsStartByte
        // Search predicate returns true if a byte equals 0xFF
        private static bool IsStartByte(byte b)
        {
            if (b == 0xFF)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        #endregion


        #region OnStatusPacketReceived
        protected virtual void OnStatusPacketReceived(StatusPacketEventArgs e)
        {
            // Make a temporary copy of the event to avoid the possibility of
            // a race condition if the last subscriber unsubscribes
            // immediately after the null check and before the event is raised.
            EventHandler<StatusPacketEventArgs> handler = RaiseStatusPacketReceivedEvent;

            // Event will be null if there are no subscribers
            if (handler != null)
            {
                // Use the () operator to raise the event.
                handler(this, e);
            }

        }
        #endregion
        

        #region OnRaiseStatusErrorEvent
        /// <summary>
        /// Wrap the event invocations inside a protected virtual method
        /// to allow derived classes to override the event invocation behavior
        /// </summary>
        /// <param name="e">StatusErrorEventArgs</param>
        protected virtual void OnRaiseStatusErrorEvent(StatusErrorEventArgs e)
        {
            // Make a temporary copy of the event to avoid the possibility of
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
        #endregion  
        
    }//StatusPacket



    #endregion


    #region Class StatusErrorEventArgs
    /// <summary>
    /// Dynamixel status error event event arguments.  Inherits from EventArgs
    /// </summary>
    public class StatusErrorEventArgs : EventArgs
    {
        //Members
        private StatusErrorArg[] error;

        #region Constructor
        /// <summary>
        /// Initializes a new instance of the StatusErrorArg class with the specified 
        /// Dynamixel status byte[] array.
        /// </summary>
        /// <param name="statusPacketBuffer"></param>
        public StatusErrorEventArgs(byte[] statusPacket)
        {
            // The error byte is in postion 5
            error = StatusErrorArg.ParseErrorByte(statusPacket[4]);
        }
        #endregion


        #region Properties
        /// <summary>
        /// Get status packet error byte.
        /// </summary>
        public StatusErrorArg[] Error
        {
            get { return error; }
        }

        #endregion
        
    }//StatusErrorEventArgs
    #endregion


    #region Class StatusErrorArg
    /// <summary>
    /// Dynamixel status error event arguments
    /// </summary>
    public class StatusErrorArg
    {
        // Members
        private StatusError error;
        private String description;


        #region Constructors

        // TODO: Add guts to the constructor
        /// <summary>
        /// Initializes a new instance of the StatusErrorArg class with the specified 
        /// Dynamixel status byte[] array.
        /// </summary>
        /// <param name="statusPacketBuffer">Dynamixel status packet</param>
        public StatusErrorArg(byte[] statusPacketBuffer)
        {
            // Default constructor
        }

        /// <summary>
        /// Initializes a new instance of the StatusErrorArg class with the specified 
        /// error byte and description
        /// </summary>
        /// <param name="Error"></param>
        /// <param name="Description"></param>
        private StatusErrorArg(StatusError Error, String Description)
        {
            this.error = Error;
            this.description = Description;
        }
        #endregion


        #region Parse Status Error Byte
        /// <summary>
        /// Convert the error byte returned in a Dynamixel status packet to an array of errors with descritions.
        /// Generally, one error is returned.  However, this method will handle a compound error byte.
        /// </summary>
        /// <param name="error">Error byte returned from a Dynamixel status packet</param>
        /// <returns>Complex StatusErrorType that contains an error byte and friendly description. </returns>
        public static StatusErrorArg[] ParseErrorByte(byte error)
        {
            List<StatusErrorArg> se = new List<StatusErrorArg>();

            if ((error & (byte)StatusError.AngleLimit) > 0)
            {
                se.Add(new StatusErrorArg(StatusError.AngleLimit, "Status Error"));
            }
            if ((error & (byte)StatusError.Checksum) > 0)
            {
                se.Add(new StatusErrorArg(StatusError.Checksum, "Checksum Error"));
            }
            if ((error & (byte)StatusError.InputVoltage) > 0)
            {
                se.Add(new StatusErrorArg(StatusError.InputVoltage, "Input Voltage Error"));
            }
            if ((error & (byte)StatusError.Instruction) > 0)
            {
                se.Add(new StatusErrorArg(StatusError.Instruction, "Instruction Error"));
            }
            if ((error & (byte)StatusError.Overheating) > 0)
            {
                se.Add(new StatusErrorArg(StatusError.Overheating, "Overheating Error"));
            }
            if ((error & (byte)StatusError.Overload) > 0)
            {
                se.Add(new StatusErrorArg(StatusError.Overload, "Overload Error"));
            }
            if ((error & (byte)StatusError.Range) > 0)
            {
                se.Add(new StatusErrorArg(StatusError.Range, "Range Error"));
            }

            return se.ToArray();
        }
        #endregion


        #region Properties
        /// <summary>
        /// Status packet error byte.
        /// </summary>
        public StatusError Error
        {
            get { return error; }
        }

        /// <summary>
        /// Status packet error description.
        /// </summary>
        public String Description
        {
            get { return description; }
        }
        #endregion

    }//StatusErrorArg
    #endregion


    #region StatusPacketEventArgs
    public class StatusPacketEventArgs : EventArgs
    {
        private byte[] statusPacket;


        public StatusPacketEventArgs(byte[] statusPacket)
        {
            this.statusPacket = statusPacket;


        }

        public byte[] StatusPacket
        {
            get { return statusPacket; }
            set { statusPacket = value; }
        }


        #region getParameters
        /// <summary>
        /// Get all pramater bytes from a status packet.  Parameters can be an empty byte array to all 50 register values.  It all
        /// depends on the instruction packet.
        /// </summary>
        /// <returns>Fills the Parameter read-only property</returns>
        private byte[] getParameters()
        {
            List<byte> parm = new List<byte>();

            if ((statusPacket != null) && (statusPacket.Length > 5))
            {
                for (int i = 5; i < statusPacket.Length - 1; i++)
                {
                    parm.Add(statusPacket[i]);
                }
            }
            return parm.ToArray();
        }
        #endregion
        


        #region Properties

        /// <summary>
        /// Status packet start byte
        /// </summary>
        public byte StartByte1
        {
            get { return statusPacket[0]; }
        }

        /// <summary>
        /// Status packet start byte
        /// </summary>
        public byte StartByte2
        {
            get { return statusPacket[1]; }
        }

        /// <summary>
        /// Dynamixel response Id
        /// </summary>
        public byte ID
        {
            get { return statusPacket[2]; }
        }

        /// <summary>
        /// Status packet length byte.
        /// </summary>
        public byte Length
        {
            get { return statusPacket[3]; }
        }

        /// <summary>
        /// Status packet error byte.
        /// </summary>
        public byte Error
        {
            get { return statusPacket[4]; }
        }

        /// <summary>
        /// Status packet parameters
        /// </summary>
        public byte[] Parameters
        {
            get { return getParameters(); }
        }

        /// <summary>
        /// Status packet checksum
        /// </summary>
        public byte Checksum
        {
            get { return statusPacket[statusPacket.Length - 1]; }
        }
        #endregion

    }
    #endregion
    
    
}//AgaveRobotics.Robotis



