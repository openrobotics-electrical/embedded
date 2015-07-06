#region Copyright and Maintenance Log
/*************************************************************************
 * Copyright 2009 Mike Gebhard (Agave Robotics)
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 * Author..... Mike Gebhard
 * Company.... Agave Robotics
 * Email...... mailto:mike.gebhard@agaverobotics.com?subject=DynamixelSoftware
 * Help....... forum.agaverobotics.com
 * Started.... 09/27/2009
 * Updated.... 
 * 
 * The current version can be found at http://www.agaverobotics.com.
 *************************************************************************/
#endregion

using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Reflection;


namespace AgaveRobotics.Servos
{

    #region Dynamixel Instruction Enum
    /// <summary>
    /// Dynamixel instructions used in the dynamixel instruction packet
    /// </summary>
    /// <seealso cref="DynamixelInstruction"/>
    public enum DynamixelInstruction : byte
    {
        /// <summary>
        /// Ping a Dynamixel ID on the network.  The Dynamixel will respond a status packet.
        /// </summary>
        Ping = 1,
        /// <summary>
        /// Read register values.
        /// </summary>
        ReadData = 2,
        /// <summary>
        /// Write register values.
        /// </summary>
        WriteData = 3,
        /// <summary>
        /// Write register values and stay in standby mode
        /// until a global Action instruction is received.
        /// </summary>
        RegWrite = 4,
        /// <summary>
        /// Ecexute RegWrite instructions.
        /// </summary>
        Action = 5,
        /// <summary>
        /// Reset all registers (including ID) to default values.
        /// </summary>
        Reset = 6,
        /// <summary>
        /// Write register data to multiple Dynamixels at once.
        /// </summary>
        SyncWrite = 0x83,
    }
    #endregion


    #region ControlTable Enum
    /// <summary>
    /// Control table present on each Dynamixel.
    /// </summary>
    public enum ControlTable : byte
    {
        /// <summary>
        /// Model Number Address
        /// </summary>
        [Description("Model Number")]
        ModelNumber = 0,
        /// <summary>
        /// Firmware Version Address
        /// </summary>
        [Description("Firmware Version")]
        FirmwareVersion = 2,
        /// <summary>
        /// Id Address
        /// </summary>
        [Description("Id")]
        Id = 3,
        /// <summary>
        /// Baud Rate Address
        /// </summary>
        [Description("Baud Rate")]
        BaudRate = 4,
        /// <summary>
        /// Register number for Return Delay.
        /// </summary>
        [Description("Return Delay")]
        ReturnDelay = 5,
        /// <summary>
        /// Clockwise Angle Limit Address
        /// </summary>
        [Description("CW Angle Limit")]
        CWAngleLimit = 6,
        /// <summary>
        /// Counter Clockwise Angle Limit Address
        /// </summary>
        [Description("CCW Angle Limit")]
        CCWAngleLimit = 8,
        /// <summary>
        /// Temperature Limit Address
        /// </summary>
        [Description("Temperature Limit")]
        TemperatureLimit = 11,
        /// <summary>
        /// Low Voltage Limit Address
        /// </summary>
        [Description("Low Voltage Limit")]
        LowVoltageLimit = 12,
        /// <summary>
        /// High Voltage Limit Address
        /// </summary>
        [Description("High Voltage Limit")]
        HighVoltageLimit = 13,
        /// <summary>
        /// Max Torque Address
        /// </summary>
        [Description("Max Torque")]
        MaxTorque = 14,
        /// <summary>
        /// Status Return Level Address
        /// </summary>
        [Description("Status Return Level")]
        StatusReturnLevel = 16,
        /// <summary>
        /// Alarm Led Address
        /// </summary>
        [Description("Alarm Led")]
        AlarmLed = 17,
        /// <summary>
        /// Alarm Shutdown Address
        /// </summary>
        [Description("Alarm Shutdown")]
        AlarmShutdown = 18,
        /// <summary>
        /// Down Calibration Address
        /// </summary>
        [Description("Down Calibration")]
        DownCalibration = 20,
        /// <summary>
        /// Up Calibration Address
        /// </summary>
        [Description("Up Calibration")]
        UpCalibration = 22,
        /// <summary>
        /// Torque Enable Address
        /// </summary>
        [Description("Torque Enable")]
        TorqueEnable = 24,
        /// <summary>
        /// LED Address
        /// </summary>
        [Description("LED")]
        LED = 25,
        /// <summary>
        /// Clockwise Compliance Margin Address
        /// </summary>
        [Description("CW Compliance Margin")]
        CWComplianceMargin = 26,
        /// <summary>
        /// Counter Clockwise Compliance Margin Address
        /// </summary>
        [Description("CCW Compliance Margin")]
        CCWComplianceMargin = 27,
        /// <summary>
        /// Clockwise Compliance Slope Address
        /// </summary>
        [Description("CW Compliance Slope")]
        CWComplianceSlope = 28,
        /// <summary>
        /// Counter Clockwise Compliance Slope Address
        /// </summary>
        [Description("CCW Compliance Slope")]
        CCWComplianceSlope = 29,
        /// <summary>
        /// Goal Position Address
        /// </summary>
        [Description("Goal Position")]
        GoalPosition = 30,
        /// <summary>
        /// Moving Speed Address
        /// </summary>
        [Description("Moving Speed")]
        MovingSpeed = 32,
        /// <summary>
        /// Torque Limit Address
        /// </summary>
        [Description("Torque Limit")]
        TorqueLimit = 34,
        /// <summary>
        /// Register number for Current Position.
        /// </summary>
        [Description("Current Position")]
        CurrentPosition = 36,
        /// <summary>
        /// Current Speed Address
        /// </summary>
        [Description("Current Speed")]
        CurrentSpeed = 38,
        /// <summary>
        /// Current Load Address
        /// </summary>
        [Description("Current Load")]
        CurrentLoad = 40,
        /// <summary>
        /// Current Voltage Address
        /// </summary>
        [Description("Current Voltage")]
        CurrentVoltage = 42,
        /// <summary>
        /// Current Temperature Address
        /// </summary>
        [Description("Current Temperature")]
        CurrentTemperature = 43,
        /// <summary>
        /// Registered Instruction Address 
        /// </summary>
        [Description("Registered Instruction")]
        RegisteredInstruction = 44,
        /// <summary>
        /// Moving Address
        /// </summary>
        [Description("Moving")]
        Moving = 46,
        /// <summary>
        /// Lock Address
        /// </summary>
        [Description("Lock")]
        Lock = 47,
        /// <summary>
        /// Punch Address
        /// </summary>
        [Description("Punch")]
        Punch = 48,
    }
    #endregion
    

    #region Class InstructionPacket
    /// <summary>
    /// Build Dyanmixl instruction commands.
    /// </summary>
    public class InstructionPacket : DynamixelInstructionBuilderBase
    {

        // Members
        private byte[] startBytes = { 0xFF, 0xFF };
        private DynamixelInstruction instruction;
        private byte[] parameters;


        #region Constructor
        /// <summary>
        /// InstructionPacket default constructor
        /// </summary>
        public InstructionPacket()
        { 
            // Default constructor 
        }

        /// <summary>
        /// InstructionPacket
        /// </summary>
        /// <param name="Id">ID of the dynamixel to affect</param>
        /// <param name="Instruction">Instruction to preform.</param>
        public InstructionPacket(byte Id, DynamixelInstruction Instruction)
        {
            id = Id;
            instruction = Instruction;
        }
        #endregion


        #region ToByteArray
        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public override byte[] ToByteArray()
        {
            List<byte> i = new List<byte>();
            i.Add(startBytes[0]);
            i.Add(startBytes[1]);
            i.Add(id);
            i.Add(len());
            i.Add((byte)instruction);

            if ((parameters != null) && (parameters.Length > 0))
            {
                foreach (byte b in parameters)
                {
                    i.Add(b);
                }
            }

            i.Add(csum());
            return i.ToArray();
        }
        #endregion
        
        
        #region Calculate Length
        // Calculate the length byte
        private byte len()
        {
            int l = 2;
            

            // Count the parameter values
            if ((parameters != null) && (parameters.Length > 0))
            {
                return (byte)(l + parameters.Length);
            }

            return (byte)l;
        }
        #endregion
        

        #region Calculate the checksum
        /// <summary>
        /// Calculate the check sum = ~(ID + Length + instruction + Parameter 1 + paramerter N)
        /// </summary>
        /// <returns>~(ID + Length + instruction + Parameter 1 + paramerter N)</returns>
        private byte csum()
        {
            byte checksum = 0;

            checksum = (byte)((byte)id + (byte)len() + (byte)instruction);

            // Add the parameter values
            if ((parameters != null) && (parameters.Length > 0))
            {
                foreach (byte b in parameters)
                {
                    checksum += (byte)b;
                }
            }

            // bitwise not
            checksum = (byte)~(checksum & 0xFF);
            return checksum;
        }
        #endregion

        
        #region Properties

        /// <summary>
        ///  Dynamixel instruction start bytes
        /// </summary>

        public byte[] StartBytes
        {
            get { return startBytes; }
        }

        /// <summary>
        /// Instruction length byte
        /// </summary>
        public byte Length
        {
            get { return len(); }
        }

        /// <summary>
        /// Dynamixel instruction
        /// </summary>
        public DynamixelInstruction Instruction
        {
            get { return instruction; }
            set { instruction = value; }
        }

        /// <summary>
        /// Byte array of dynamixel parameters
        /// </summary>
        public byte[] Parameters
        {
            get { return parameters; }
            set { parameters = value; }
        }

        /// <summary>
        /// Checksum = ~(ID + Length + instruction + Parameter 1 + paramerter N)
        /// </summary>
        public byte Checksum
        {
            get { return csum(); }
        }

        #endregion


    }//InstructionPacket
    #endregion
    

    #region Class WriteDataBuilder
    /// <summary>
    /// Provides custom constructors and methods to build a dynamixel WriteData instruction.  
    /// </summary>
    public class WriteDataBuilder : DynamixelInstructionBuilderBase 
    {
        private byte startAddress;


        #region Constructors
        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// Dynmixel ID and Dynamixel control table address (register).
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the WriteData instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        public WriteDataBuilder(byte id, ControlTable startAddress)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
        }

        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// Dynmixel ID and parameter string.  
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the WriteData instruction.</param>
        /// <param name="parameters">The paramter argument is a space delimited string 
        /// starting with the start address followed by control table values.
        /// </param>
        public WriteDataBuilder(byte id, String parameters)
        {
            this.id = id;
            List<int> parms = ParseParameterString(parameters);

            // Address should be the first parameter
            // Reomove [0] so we can enumerate the 
            // remaining values
            this.startAddress = (byte)parms[0];

            for(int i = 1; i < parms.Count; i++)
            {
                AddParameter(parms[i]);
            } 
        }

        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and space delimited string 
        /// of values to write.
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the WriteData instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="parameters">The paramter argument is a space delimited string of control table values.</param>
        public WriteDataBuilder(byte id, ControlTable startAddress, String parameters)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
            List<int> parms = ParseParameterString(parameters);

            foreach (int value in parms)
            {
                AddParameter(value);
            }
        }

        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and initial value
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the WriteData instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="value">First value to write.</param>
        public WriteDataBuilder(byte id, ControlTable startAddress, int value)
        { 
            this.id = id;
            this.startAddress = (byte)startAddress;
            this.AddParameter(value);
        }

        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and array of values.
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the WriteData instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="args">Array of values to write starting at the start address.</param>
        public WriteDataBuilder(byte id, ControlTable startAddress, params int[] args)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
            foreach (int value in args)
            {
                AddParameter(value);  
            }
        }
        #endregion


        #region ParseParameterString

        /// <summary>
        /// Takes a space delimited string and converts the string to a generic list of integers.
        /// </summary>
        /// <param name="parmString">Space delimited string</param>
        /// <returns>Generic list of integers</returns>
        private List<int> ParseParameterString(String parmString)
        {
            List<int> parms = new List<int>();

            // Clean things up a bit.
            parmString = parmString.Trim();

            String[] sTemp = parmString.Split(' ');

            // holds the int.parse result
            int result;
            foreach (String s in sTemp)
            {
                // Skip the empties
                if (!String.IsNullOrEmpty(s.Trim()))
                {
                    if (int.TryParse(s, out result))
                    {
                        parms.Add(result);   
                    }
                }
            }
            return parms;
        }
        #endregion
          

        #region AddParameter
        /// <summary>
        /// Add a parameter value to the WriteData instruction.
        /// </summary>
        /// <param name="value">If the Dynamixel control table address requires a high and low byte the 
        /// value is split into two bytes [low byte] [high byte]; otherwise only the low byte is added.</param>
        public void AddParameter(int value)
        {
            int paramCount = paramters.Count;
            int currentAddress = (startAddress + paramCount);

            // Write low byte
            paramters.Add((byte)(value & 0xFF));

            // If the address is a word we'll need to write the high
            // byte.
            if (InstructionPacket.IsWordRegister((byte)currentAddress))
            { 
                paramters.Add((byte)((value & 0xFF00) / 0x100));
            }
        } //AddParameter
        #endregion
        

        #region ToByteArray
        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public override byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();
            int checksum = 0;

            // Add preample
            buff.Add(0xFF);
            buff.Add(0xFF);

            //Add Id
            buff.Add(id);
            checksum += id;

            // Calculate Length
            buff.Add((byte)(paramters.Count + 3));
            checksum += (paramters.Count + 3);

            // Add instruction
            buff.Add((byte)DynamixelInstruction.WriteData);
            checksum += (byte)DynamixelInstruction.WriteData;

            // Add start address
            buff.Add(startAddress);
            checksum += startAddress;

            // Add parameters
            foreach (byte b in paramters)
            {
                buff.Add(b);
                checksum += b;
            }

            // Calculate checksum
            // Get low byte and or bitwise or the result
            buff.Add((byte)~(checksum & 0xFF));

            return buff.ToArray();
        }
        #endregion


        #region Properties
        /// <summary>
        /// Get Dynamixel control table address.
        /// </summary>
        public byte StartAddress
        {
            get { return startAddress; }
        }
        #endregion

    } //WriteDataBuilder
    #endregion


    #region Class RegWriteBuilder

    /// <summary>
    /// Provides custom constructors and methods to build a dynamixel RegWrite instruction.
    /// </summary>
    public class RegWriteBuilder : DynamixelInstructionBuilderBase
    {
        //private byte id;
        private byte startAddress;

        #region Constructors

        /// <summary>
        /// Initializes a new instance of the RegWriteBuilder class with the specified 
        /// Dynmixel ID and Dynamixel control table address (register).
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the RegWrite instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        public RegWriteBuilder(byte id, ControlTable startAddress)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
        }

        /// <summary>
        /// Initializes a new instance of the RegWriteaBuilder class with the specified 
        /// Dynmixel ID and parameter string.  
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the RegWrite instruction.</param>
        /// <param name="parameters">A space delimited string 
        /// starting with the start address followed by control table values.
        /// </param>
        public RegWriteBuilder(byte id, String parameters)
        {
            this.id = id;
            List<int> parms = ParseParameterString(parameters);

            // Address should be the first parameter
            // Reomove [0] so we can enumerate the 
            // remaining values
            this.startAddress = (byte)parms[0];

            for(int i = 1; i < parms.Count; i++)
            {
                AddParameter(parms[i]);
            } 
        }

        /// <summary>
        /// Initializes a new instance of the RegWriteBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and space delimited string 
        /// of values to write.
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the RegWrite instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="parameters">A space delimited string of control table values.</param>
        public RegWriteBuilder(byte id, ControlTable startAddress, String parameters)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
            List<int> parms = ParseParameterString(parameters);

            foreach (int value in parms)
            {
                AddParameter(value);
            }
        }

        /// <summary>
        /// Initializes a new instance of the RegWriteBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and initial value
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the RegWrite instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="value">First value to write.</param>
        public RegWriteBuilder(byte id, ControlTable startAddress, int value)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
            this.AddParameter(value);
        }

        /// <summary>
        /// Initializes a new instance of the RegWriteBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and array of values.
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the RegWrite instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="args">Array of values to write starting at the start address.</param>
        public RegWriteBuilder(byte id, ControlTable startAddress, params int[] args)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
            foreach (int value in args)
            {
                AddParameter(value);  
            }
        }
        #endregion


        #region ParseParameterString
        /// <summary>
        /// Takes a space delimited string and converts the string to a generic list of integers.
        /// </summary>
        /// <param name="parmString">Space delimited string</param>
        /// <returns>Generic list of integers</returns>
        private List<int> ParseParameterString(String parmString)
        {
            List<int> parms = new List<int>();

            // Clean things up a bit.
            parmString = parmString.Trim();

            String[] sTemp = parmString.Split(' ');

            // holds the int.parse result
            int result;
            foreach (String s in sTemp)
            {
                // Skip the empties
                if (!String.IsNullOrEmpty(s.Trim()))
                {
                    if (int.TryParse(s, out result))
                    {
                        parms.Add(result);
                    }
                }
            }
            return parms;
        }
        #endregion


        #region AddParameter
        /// <summary>
        /// Add a parameter value to the RegWrite instruction.
        /// </summary>
        /// <param name="value">If the Dynamixel control table address requires a high and low byte the 
        /// value is split into two bytes [low byte] [high byte]; otherwise only the low byte is added.</param>
        public void AddParameter(int value)
        {
            int paramCount = paramters.Count;
            int currentAddress = (startAddress + paramCount);

            // Write low byte
            paramters.Add((byte)(value & 0xFF));

            // If the address is a word we'll need to write the high
            // byte.
            if (InstructionPacket.IsWordRegister((byte)currentAddress))
            {
                paramters.Add((byte)((value & 0xFF00) / 0x100));
            }
        } //AddParameter
        #endregion


        #region ToByteArray
        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public override byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();
            int checksum = 0;

            // Add preample
            buff.Add(0xFF);
            buff.Add(0xFF);

            //Add Id
            buff.Add(id);
            checksum += id;

            // Calculate Length
            buff.Add((byte)(paramters.Count + 3));
            checksum += (paramters.Count + 3);

            // Add instruction
            buff.Add((byte)DynamixelInstruction.RegWrite);
            checksum += (byte)DynamixelInstruction.RegWrite;

            // Add start address
            buff.Add(startAddress);
            checksum += startAddress;

            // Add parameters
            foreach (byte b in paramters)
            {
                buff.Add(b);
                checksum += b;
            }

            // Calculate checksum
            // Get low byte and or bitwise or the result
            buff.Add((byte)~(checksum & 0xFF));

            return buff.ToArray();
        }
        #endregion


        #region Properties
        /// <summary>
        /// Get Set the control table starting address of the write.
        /// </summary>
        public byte StartAddress
        {
            get { return startAddress; }
            set { startAddress = value; }
        }

        #endregion

    } //RegWriteBuilder
    #endregion


    #region Class ReadDataBuilder
    /// <summary>
    /// Provides custom constructors and methods to build a dynamixel ReadData instruction.
    /// </summary>
    public class ReadDataBuilder : DynamixelInstructionBuilderBase
    {
        // Members
        private byte startAddress;
        private byte bytesToRead;

        #region Constructors

        /// <summary>
        /// Initializes a new instance of the RegWriteBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and number
        /// of bytes to read from the control table.
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the ReadData instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="bytesToRead">The number of byte to read.</param>
        public ReadDataBuilder(byte id, ControlTable startAddress, byte bytesToRead)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
            this.bytesToRead = bytesToRead;
        }

        /// <summary>
        /// Initializes a new instance of the RegWriteBuilder class with the specified 
        /// Dynmixel ID and space delimited string of parameters
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the ReadData instruction.</param>
        /// <param name="paramters">A space delimited string of control table values starting with the 
        /// a start address followed by the number of bytes to read.</param>
        public ReadDataBuilder(byte id, String paramters)
        {
            this.id = id;
            List<int> temp = ParseParameterString(paramters);

            this.startAddress = (byte)temp[0];
            this.bytesToRead = (byte)temp[1];
        }

        #endregion


        #region ParseParameterString
        // Get the start address and number of byte to read from the parmater string.
        private List<int> ParseParameterString(String parmString)
        {
            List<int> parms = new List<int>();

            // Clean things up a bit.
            parmString = parmString.Trim();

            String[] sTemp = parmString.Split(' ');

            // holds the int.TryParse result
            int result;

            // We only need 2 parameter
            for (int i = 0; i < 2; i++)
            {
                if (int.TryParse(sTemp[i], out result))
                {
                    parms.Add(result);
                }   
            }

            return parms;
        }
        #endregion


        #region ToByteArray

        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public override byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();
            int checksum = 0;

            // Add preample
            buff.Add(0xFF);
            buff.Add(0xFF);

            //Add Id
            buff.Add(id);
            checksum += id;

            // Length for the read data instruction
            // shold alwyas be 4 (address and number of bytes)
            buff.Add((byte)(4));
            checksum += (4);

            // Add instruction
            buff.Add((byte)DynamixelInstruction.ReadData);
            checksum += (byte)DynamixelInstruction.ReadData;

            // Add start address
            buff.Add(startAddress);
            checksum += startAddress;

            buff.Add(bytesToRead);
            checksum += bytesToRead;

            // Calculate checksum
            // Get low byte and or bitwise or the result
            buff.Add((byte)~(checksum & 0xFF));

            return buff.ToArray();
        }
        #endregion


        #region Properties

        /// <summary>
        /// Get Set the control table starting address of the read.
        /// </summary>
        public byte StartAddress
        {
            get { return startAddress; }
            set { startAddress = value; }
        }

        /// <summary>
        /// Get Set the number of bytes to read
        /// </summary>
        public byte BytesToRead
        {
            get { return bytesToRead; }
            set { bytesToRead = value; }
        }

        #endregion

    } //ReadDataBuilder
    #endregion


    #region Class SyncWriteBuilder
    /// <summary>
    /// Provides custom constructors and methods to build a dynamixel SyncWrite instruction.
    /// </summary>
    public class SyncWriteBuilder : DynamixelInstructionBuilderBase
    {
        // Members
        private byte startAddress;
        private List<SyncWriteParameter> syncWriteParameters = new List<SyncWriteParameter>();

        #region Constructors

        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// control table start address.
        /// </summary>
        /// <param name="startAddress">Dynamixel control table address.</param>
        public SyncWriteBuilder(ControlTable startAddress)
        {
            this.id = 0xFE;
            this.startAddress = (byte)startAddress;
        }

        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// space delimited string of parameters.
        /// </summary>
        /// <param name="parameters">Space delimited string of SyncWrite parameters.
        /// [start address] [dyanmixel data length] [ID 1] [1st value] [value...N] [IDx] [1st x value] [x value...N]</param>
        public SyncWriteBuilder(String parameters)
        {
            this.id = 0xFE;

            // Split the input string into an int generic
            List<int> parms = ParseParameterString(parameters);

            // Turn the generic int list into a generic collection of SyncWriteParameters
            BuildSyncWriteParameters(parms);
        }


        #endregion


        #region ParseParameterString
        /// <summary>
        /// Takes a space delimited string and converts the string to a generic list of integers.
        /// </summary>
        /// <param name="parmString">Space delimited string</param>
        /// <returns>Generic list of integers</returns>
        private List<int> ParseParameterString(String parmString)
        {
            List<int> parms = new List<int>();

            // Clean things up a bit.
            parmString = parmString.Trim();

            String[] sTemp = parmString.Split(' ');

            // holds the int.parse result
            int result;
            foreach (String s in sTemp)
            {
                // Skip the empties
                if (!String.IsNullOrEmpty(s.Trim()))
                {
                    if (int.TryParse(s, out result))
                    {
                        parms.Add(result);
                    }
                }
            }
            return parms;
        }
        #endregion


        #region BuildSyncWriteParameters
        /// <summary>
        /// Convert a generic list of integers to a generic collection of SyncWriteParameters.
        /// </summary>
        /// <param name="parms">Generic list of integers.</param>
        private void BuildSyncWriteParameters(List<int> parms)
        {
            byte tempId;
            SyncWriteParameter swp;

            // Get the start address
            this.startAddress = (byte)parms[0];

            // Number of bytes to write to each dynamxiel
            int length = parms[1];

            for (int i = 2; i < parms.Count; i++)
            {
                // Extract the id
                tempId = (byte)parms[i++];

                // Construct a SyncWriteParameter
                swp = new SyncWriteParameter(tempId, (ControlTable)this.startAddress);

                // loop through the parm int list until 
                // we get the specified number of bytes 
                // defined in length
                while (swp.Paramters.Count < length)
                {
                    swp.AddParameter(parms[i++]);
                }

                i--;

                // Add to the collections
                syncWriteParameters.Add(swp);

            }
        }
        #endregion
        

        #region AddParameter Overload

        /// <summary>
        /// Add a SyncWriteParameter to the SyncWriteParameters generic collection
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the instruction.</param>
        /// <param name="value">Initial control table address for the write.</param>
        public void AddParameter(byte id, int value)
        {
            SyncWriteParameter swp = new SyncWriteParameter(id, (ControlTable)startAddress, value);

            if (IsSyncWriteParameterValid(swp))
            { 
                syncWriteParameters.Add(swp);   
            }            
        } //AddParameter

        /// <summary>
        /// Add a SyncWriteParameter to the SyncWriteParameters generic collection
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the instruction.</param>
        /// <param name="args">Generic list of values to write.</param>
        public void AddParameter(byte id, params int[] args)
        {
            SyncWriteParameter swp = new SyncWriteParameter(id, (ControlTable)startAddress, args);

            // Compare the current swp to the others in the collection
            if (IsSyncWriteParameterValid(swp))
            {
                syncWriteParameters.Add(swp);
            }
        } //AddParameter

        /// <summary>
        /// Add a SyncWriteParameter to the SyncWriteParameters generic collections
        /// </summary>
        /// <param name="syncWriteParameter">SyncWriteParameter to add to the SyncWriteParameters generic collection</param>
        public void AddParameter(SyncWriteParameter syncWriteParameter)
        {
            if (IsSyncWriteParameterValid(syncWriteParameter))
            { 
                syncWriteParameters.Add(syncWriteParameter);    
            }
        } //AddParameter

        #endregion


        #region ToByteArray
        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public override byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();
            int checksum = 0;

            int len;

            // Add preample
            buff.Add(0xFF);
            buff.Add(0xFF);

            //Add Id
            buff.Add(id);
            checksum += id;

            // Calculate Length
            // TODO: validate all instructions are the same length
            len = ((syncWriteParameters.Count) * (syncWriteParameters[0].Paramters.Count + 1) + 4) & 0xFF;
            buff.Add((byte)len);
            checksum += (byte)len;

            // Add instruction
            buff.Add((byte)DynamixelInstruction.SyncWrite);
            checksum += (byte)DynamixelInstruction.SyncWrite;

            // Add start address
            buff.Add(startAddress);
            checksum += startAddress;

            // Individual id lengths
            buff.Add((byte)syncWriteParameters[0].Paramters.Count);
            checksum += (byte)syncWriteParameters[0].Paramters.Count;

            // Add parameters
            foreach (SyncWriteParameter swp in syncWriteParameters)
            {
                // Add the Id 
                buff.Add(swp.Id);
                checksum += (byte)swp.Id;

                foreach (byte b in swp.Paramters)
                { 
                    // Add the values
                    buff.Add(b);
                    checksum += b;
                } 
            }

            // Calculate checksum
            // Get low byte and or bitwise or the result
            buff.Add((byte)~(checksum & 0xFF));

            return buff.ToArray();
        }
        #endregion


        #region ClearAllParameters
        /// <summary>
        /// Clears all SyncWrite Parameters
        /// </summary>
        public void ClearAllParameters()
        {
            syncWriteParameters.Clear();    
        }
        #endregion
        

        #region RemoveParameter
        /// <summary>
        /// Removes the first occurrence of the SyncWriteParameter object from syncWriteParameters list
        /// </summary>
        /// <param name="paramater">SyncWrite parameter</param>
        public void RemoveParameter(SyncWriteParameter paramater)
        {
            syncWriteParameters.Remove(paramater);
        }
        #endregion
        

        #region RemoveParameterAt
        /// <summary>
        /// Removes the element at the specified index of the syncWriteParameters list
        /// </summary>
        /// <param name="index"></param>
        public void RemoveParameterAt(int index)
        {
            syncWriteParameters.RemoveAt(index);   
        }
        #endregion
        

        #region ParameterIdExists
        /// <summary>
        /// Determines whether the SyncWriteParameters list already contains a 
        /// command for the SyncWriteParameter.Id argument  passed to the method.
        /// </summary>
        /// <param name="id">SyncWriteParameter to find</param>
        /// <returns>True if the parameter is found; otherwisw returns false</returns>
        public Boolean ParameterIdExists(byte id)
        {
            foreach (SyncWriteParameter p in syncWriteParameters)
            {
                if (p.Id == id) { return true; }
            }
            return false;
        }
        #endregion


        #region ParameterIndexOfId
        /// <summary>
        /// Returns the index of a SyncWriteParameter if a parameter with the same ID is 
        /// found in the SyncWriteParameters list.
        /// </summary>
        /// <param name="id">SyncWriteParameter to find</param>
        /// <returns>The first index of the SyncWriteParameter found in the SyncWriteParameters list</returns>
        public int ParameterIndexOfId(byte id)
        { 
            int index = -1;
            foreach (SyncWriteParameter p in syncWriteParameters)
            {
                index++;
                if (p.Id == Id) { return index; }
            }

            return index;
        }
        #endregion
        

        #region IsSyncWriteParameterValid
        /// <summary>
        /// The SyncWrite business rules say that all IDs being affected must write to the same
        /// control table registers.  The IsSyncWriteParameterValid compares each parameter byte length
        /// to the SyncWriteParameter index 0.
        /// </summary>
        /// <param name="swp">SyncWriteParameter to compare</param>
        /// <returns>True if the lengths are equal; otherwise false.</returns>
        private Boolean IsSyncWriteParameterValid(SyncWriteParameter swp)
        {
            // If there are not parameters then simply return
            if (syncWriteParameters.Count == 0) { return true; }

            // All syncWriteParameters must be the same lenght as the first syncWriteParameters parameter.
            if (syncWriteParameters[0].Paramters.Count == swp.Paramters.Count) { return true; }

            throw new ApplicationException("SyncWrite command must write equal data lengths to the control table");
        }
        #endregion
        

        #region Properties

        /// <summary>
        /// Get Set the control table starting address of the write.
        /// </summary>
        public byte StartAddress
        {
            get { return startAddress; }
            set { startAddress = value; }
        }

        #endregion

    } //SyncWriteBuilder
    #endregion


    #region Class SyncWriteParameter
    /// <summary>
    /// Provides custom constructors and methods to help build a dynamixel SyncWrite instruction.
    /// </summary>
    public class SyncWriteParameter : IDynamixelInstruction
    {
        // Members
        private byte id;
        private byte startAddress;
        private List<byte> paramters = new List<byte>();


        #region Constructors

        /// <summary>
        /// Initializes a new instance of the SyncWriteParameter class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register).
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        public SyncWriteParameter(byte id, ControlTable startAddress)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
        }
        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and initial value
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="value">First value to write.</param>
        public SyncWriteParameter(byte id, ControlTable startAddress, int value)
        { 
            this.id = id;
            this.startAddress = (byte)startAddress;
            this.AddParameter(value);
        }
        /// <summary>
        /// Initializes a new instance of the WriteDataBuilder class with the specified 
        /// Dynmixel ID, Dynamixel control table address (register), and array of values.
        /// </summary>
        /// <param name="id">Dynamixel ID to execute the instruction.</param>
        /// <param name="startAddress">Dynamixel control table address.</param>
        /// <param name="args">Array of values to write starting at the start address.</param>
        public SyncWriteParameter(byte id, ControlTable startAddress, params int[] args)
        {
            this.id = id;
            this.startAddress = (byte)startAddress;
            foreach (int value in args)
            {
                AddParameter(value);  
            }
        }

        #endregion


        #region AddParameter
        /// <summary>
        /// Add a parameter value to the WriteData instruction.
        /// </summary>
        /// <param name="value">Integer value.  If the address requires a high and low byte the 
        /// value is split into two bytes; otherwise only the low byte is used.</param>
        public void AddParameter(int value)
        {
            int paramCount = paramters.Count;
            int currentAddress = (startAddress + paramCount);

            // Write low byte
            paramters.Add((byte)(value & 0xFF));

            // If the address is a word we'll need to write the high
            // byte.
            if (InstructionPacket.IsWordRegister((byte)currentAddress))
            { 
                paramters.Add((byte)((value & 0xFF00) / 0x100));
            }
        } //AddParameter
        #endregion
        

        #region ToByteArray
        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();

            //Add Id
            //buff.Add(id);

            // Add parameters
            foreach (byte b in paramters)
            {
                buff.Add(b);
            }

            return buff.ToArray();
        }
        #endregion


        #region Properties

        /// <summary>
        /// Get Set the Dynamixel ID
        /// </summary>
        public byte Id
        {
            get { return id; }
            set { id = value; }
        }

        /// <summary>
        /// Get Set the geberic list of byte parameters
        /// </summary>
        public List<byte> Paramters
        {
            get { return paramters; }
            set { paramters = value; }
        }
        #endregion
        
    } //SyncWriteParameter
    #endregion
    

    #region Class PingBuilder
    /// <summary>
    /// Provides a custom constructor and method to build a dynamixel Ping instruction.
    /// </summary>
    public class PingBuilder : DynamixelInstructionBuilderBase
    {
        #region Constructor
        /// <summary>
        /// Initializes a new instance of the PingBuilder class with the specified 
        /// Dynmixel ID
        /// </summary>
        /// <param name="id">Dynamixel Id to ping</param>
        public PingBuilder(byte id)
        {
            this.id = id;
        }
        #endregion
        

        #region ToByteArray
        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public override byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();
            int checksum = 0;

            // Add preample
            buff.Add(0xFF);
            buff.Add(0xFF);

            //Add Id
            buff.Add(id);
            checksum += id;

            // Calculate Length
            buff.Add(2);
            checksum += (2);

            // Add instruction
            buff.Add((byte)DynamixelInstruction.Ping);
            checksum += (byte)DynamixelInstruction.Ping;

            // Calculate checksum
            // Get low byte and or bitwise or the result
            buff.Add((byte)~(checksum & 0xFF));

            return buff.ToArray();
        }
        #endregion

    } //PingBuilder
    #endregion


    #region Class ActionBuilder
    /// <summary>
    /// Provides a custom constructor and method to build a dynamixel Action instruction.
    /// The Action instruction executes cached RegWrite instruictions.
    /// </summary>
    public class ActionBuilder : DynamixelInstructionBuilderBase
    {

        #region Constructor
        /// <summary>
        /// Initializes a new instance of the ActionBuilder class useing the broadcast ID 0xFE.
        /// </summary>
        public ActionBuilder()
        {
           this.id = 0xFE;
        }
        #endregion
        

        #region ToByteArray
        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public override byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();
            int checksum = 0;

            // Add preample
            buff.Add(0xFF);
            buff.Add(0xFF);

            //Add Id
            buff.Add(id);
            checksum += id;

            // Calculate Length
            buff.Add(2);
            checksum += (2);

            // Add instruction
            buff.Add((byte)DynamixelInstruction.Action);
            checksum += (byte)DynamixelInstruction.Action;

            // Calculate checksum
            // Get low byte and or bitwise or the result
            buff.Add((byte)~(checksum & 0xFF));

            return buff.ToArray();
        }
        #endregion

    } //ActionBuilder
    #endregion


    #region Class ResetBuilder
    /// <summary>
    /// Provides a custom constructor and method to build a dynamixel ResetBuilder instruction.
    /// </summary>
    public class ResetBuilder : DynamixelInstructionBuilderBase
    {
        #region Constructor
        /// <summary>
        /// Initializes a new instance of the ResetBuilder class useing the broadcast ID 0xFE.
        /// </summary>
        /// <param name="id"></param>
        public ResetBuilder(byte id)
        {
            this.id = id;
        }
        #endregion
        

        #region ToByteArray
        /// <summary>
        /// The ToByteArray method overrides the base ToByteArray method.  
        /// ToByteArray creates an array of bytes that represents
        /// a serialized dynamixel command.  
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public override byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();
            int checksum = 0;

            // Add preample
            buff.Add(0xFF);
            buff.Add(0xFF);

            //Add Id
            buff.Add(id);
            checksum += id;

            // Calculate Length
            buff.Add(2);
            checksum += (2);

            // Add instruction
            buff.Add((byte)DynamixelInstruction.Reset);
            checksum += (byte)DynamixelInstruction.Reset;

            // Calculate checksum
            // Get low byte and or bitwise or the result
            buff.Add((byte)~(checksum & 0xFF));

            return buff.ToArray();
        }
        #endregion

    } //ResetBuilder
    #endregion


    #region IDynamixelInstruction
    /// <summary>
    /// Dynamixel instruction interface.  All dynamixel instructions implement the ToByteArray() method.
    /// </summary>
    public interface IDynamixelInstruction
    {
        /// <summary>
        /// The virtual ToByteArray method creates an array of bytes that represents a serialized dynamixel command that all
        /// inheriter must implement.
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        byte[] ToByteArray();

    } //IDynamixelInstruction
    #endregion


    #region DynamixelInstructionBuilderBase
    /// <summary>
    /// The Dynamixel instruction builder is the base class for dynamixel instruction classes.
    /// DynamixelInstructionBuilderBase implements the IDynamixelInstruction interface.
    /// </summary>
    public class DynamixelInstructionBuilderBase : IDynamixelInstruction
    {
        #region Members
        /// <summary>
        /// Dynamixel ID (byte)
        /// </summary>
        protected byte id;
        /// <summary>
        /// Genaric byte list of Dynamixel parameters
        /// </summary>
        protected List<byte> paramters = new List<byte>();
        #endregion
        

        #region ToByteArray
        /// <summary>
        /// The virtual ToByteArray method creates an array of bytes that represents a serialized dynamixel command.  The
        /// inheriters can override the basic implementation.
        /// </summary>
        /// <returns>Serialized dynamixel command (byte[])</returns>
        public virtual byte[] ToByteArray()
        {
            List<byte> buff = new List<byte>();
            int checksum = 0;

            // Add preample
            buff.Add(0xFF);
            buff.Add(0xFF);

            //Add Id
            buff.Add(id);
            checksum += id;

            // Calculate Length
            buff.Add(2);
            checksum += (2);

            // Add instruction
            buff.Add((byte)DynamixelInstruction.Ping);
            checksum += (byte)DynamixelInstruction.Ping;

            // Calculate checksum
            // Get low byte and or bitwise or the result
            buff.Add((byte)~(checksum & 0xFF));

            return buff.ToArray();
        }
        #endregion


        #region IsWordRegister
        /// <summary>
        /// Check the register (control table address) we're writing to.  If the register is a word size and 
        /// we're writing a byte then we need to also send 0x00 as the high byte.
        /// </summary>
        /// <param name="startAddress">Start address to check</param>
        /// <returns>True if the register is a word size.  Returns false if the register is a byte size.</returns>
        public static Boolean IsWordRegister(byte startAddress)
        {
            switch (startAddress)
            {
                case 0x00:
                case 0x06:
                case 0x08:
                case 0x0E:
                case 0x14:
                case 0x16:
                case 0x1E:
                case 0x20:
                case 0x22:
                case 0x24:
                case 0x26:
                case 0x28:
                case 0x30:
                    return true;
                default:
                    return false;
            }
        }
        #endregion


        #region ToString
        /// <summary>
        /// Convert a Dynamixel instruction byte array to a space delimited string.
        /// </summary>
        /// <returns>The string representation of a dynamixel instruction</returns>
        public override string ToString()
        {
            // Get the instruction
            byte[] buff = ToByteArray();

            // Convert to a string
            String temp = BitConverter.ToString(buff);

            //Replace the default the - with a space
            return temp.Replace('-', ' ');
        }
        #endregion
        

        #region Properties
        /// <summary>
        /// Get Set the dynamixel ID (byte)
        /// </summary>
        public byte Id
        {
            get { return id; }
            set { id = value; }
        }

        /// <summary>
        /// Get Set the genaric list of byte parameters
        /// </summary>
        public List<byte> Paramters
        {
            get { return paramters; }
            set { paramters = value; }
        }
        #endregion
        
    }
    #endregion
    
    


}//AgaveRobotics.Robotis
