/*------------------------------------------------------------------------------*\
 * This source file is subject to the GPLv3 license that is bundled with this   *
 * package in the file COPYING.                                                 *
 * It is also available through the world-wide-web at this URL:                 *
 * http://www.gnu.org/licenses/gpl-3.0.txt                                      *
 * If you did not receive a copy of the license and are unable to obtain it     *
 * through the world-wide-web, please send an email to                          *
 * siempre.aprendiendo@gmail.com so we can send you a copy immediately.         *
 *                                                                              *
 * @category  Robotics                                                          *
 * @copyright Copyright (c) 2011 Jose Cortes (http://www.softwaresouls.com) *
 * @license   http://www.gnu.org/licenses/gpl-3.0.txt GNU v3 Licence            *
 *                                                                              *
\*------------------------------------------------------------------------------*/


using System;
using System.IO.Ports;
using System.Threading;

using MyUtils;
using MyBasicSystem;

namespace MySerialPort
{
    class RCDataReader
    {
        public bool end = false;
        protected enum ZigSequence { Unknown, Header, Id, Data1, ComplementData1, Data2, ComplementData2 };
        private byte data1, data2;
        private int value = 0;

        ZigSequence nextRCZigSeq = ZigSequence.Unknown;

        public int getValue()
        {
            return value;
        }

        public void rawRemoteDataReceived(byte[] rcData)
        {
            byte data = (byte)rcData[0];            

            if (nextRCZigSeq == ZigSequence.Data1)
            {
                data1 = data;
                Debug.show("data1 {0}", data1);
                nextRCZigSeq = ZigSequence.ComplementData1;
            }
            else
            {
                if (nextRCZigSeq == ZigSequence.ComplementData1)
                {
                    nextRCZigSeq = ZigSequence.Data2;
                }
                else
                {
                    if (nextRCZigSeq == ZigSequence.Data2)
                    {
                        data2 = data;
                        Debug.show("data2 {0}", data2);
                        nextRCZigSeq = ZigSequence.ComplementData2;
                        value=Hex.fromHexHLConversionToShort(data1, data2);
                        Console.WriteLine("data1:{0}, data2:{1} => value:{2}", data1, data2, value);
                    }
                    else
                    {
                        if (nextRCZigSeq == ZigSequence.ComplementData2)
                        {
                            nextRCZigSeq = ZigSequence.Header;
                        }
                    }
                }
            }

            switch (data)
            {
                case 0xFF:
                    if (nextRCZigSeq == ZigSequence.Unknown || nextRCZigSeq == ZigSequence.Header)
                        nextRCZigSeq = ZigSequence.Id;
                    break;

                case 0x55:
                    if (nextRCZigSeq == ZigSequence.Id)
                        nextRCZigSeq = ZigSequence.Data1;
                    break;
            }

            if(value==512)
                end = true;
        }
    }

    public class SerialPort2Dynamixel
    {
        const int HeaderSize = 4;
        const int PacketLengthByteInx = 3;
        const int BufferSize = 1024;
        const int MaximuTimesTrying = 250;

        private static Mutex mutex = new Mutex();
        private byte[] buffer = new byte[BufferSize];

        /*
         * When data will be received 
         *      serialPort.DataReceived
         * then
         *      serialDataReceived will be called              
         * and then, it will call the method assigned in
         *      setReceiveDataMethod 
         * with the parameter method 
         *      remoteControlDataReceived 
         * 
        */
        public delegate void remoteControlDataReceived(byte[] data);
        private remoteControlDataReceived myRemoteControlDataReceived=null;
        private void serialDataReceived(Object obj, SerialDataReceivedEventArgs e)
        {
            if (myRemoteControlDataReceived != null)
            {
                byte[] data = new byte[1];
                
                int n = serialPort.BytesToRead;

                do
                {
                    if (n > 0)
                    {
                        serialPort.Read(data, 0, 1);
                        myRemoteControlDataReceived(data);
                        //Console.Out.WriteLine("Data received: {0}", buffer[2]);
                    }
                    n = serialPort.BytesToRead;
                } while (n > 0);
            }
        }

        protected void initBuffer()
        {
            for (int i = 0; i < BufferSize; i++)
            {
                buffer[i] = 0;
            }
        }
        
        private SerialPort serialPort = new SerialPort();
        RCDataReader rdDataReader = new RCDataReader();
        
        public void setRemoteControlMode(bool on)
        {
            if (on)
                setReceiveDataMethod(rdDataReader.rawRemoteDataReceived);
            else
                setReceiveDataMethod(null);
        }

        public bool isRemoteControlEnd()
        {
            return rdDataReader.end;
        }

        public int getRemoteControlValue()
        {
            return rdDataReader.getValue();
        }

        public void setReceiveDataMethod(remoteControlDataReceived rcDataReceived)
        {
            myRemoteControlDataReceived = rcDataReceived;
            serialPort.DataReceived += new SerialDataReceivedEventHandler(serialDataReceived);
        }

        public bool open(String com)
        {
            return open (com, 1000000);
        }

        public bool open(String com, int speed)
        {
            bool opened = false;            

            serialPort.PortName = com;
            serialPort.BaudRate = speed;
            serialPort.DataBits = 8;
            serialPort.Parity = Parity.None;
            serialPort.StopBits = StopBits.One;

            try
            {
                if (serialPort.IsOpen)                    
                    Debug.show("SerialPort2Dynamixel.open", "Serial port is already open");
                else
                {
                    serialPort.Open();
                    Debug.show("SerialPort2Dynamixel.open, opened", com);
                    opened = true;
                }
            }
            catch (Exception exc)
            {                
                Debug.show("open, exception", exc.Message);
            }

            return opened;
        }

        public void close()
        {
            if (serialPort.IsOpen)
                serialPort.Close();
            Debug.show("SerialPort2Dynamixel.close", "Conecction closed!");
        }

        private void cleanConnection()
        {
            serialPort.DiscardInBuffer();
        }

        public byte[] query(byte[] buffer, int pos, int wait)
        {
            mutex.WaitOne();

            byte[] outBuffer = null;
            try
            {
                serialPort.Write(buffer, 0, pos);
                System.Threading.Thread.Sleep(wait);
                outBuffer = rawRead();
                //outBuffer = readPacket();
            }
            catch (Exception exc)
            {
                Debug.show("SerialPort2Dynamixel.query", exc.Message);
            }

            mutex.ReleaseMutex();

            return outBuffer;
        }

        public void rawWrite(byte[] buffer, int pos)
        {
            mutex.WaitOne();

            try
            {
                serialPort.Write(buffer, 0, pos);
            }
            catch (Exception exc)
            {
                Debug.show("SerialPort2Dynamixel.rawWrite", exc.Message);
            }

            mutex.ReleaseMutex();
        }

        public byte[] rawRead()
        {
            mutex.WaitOne();

            byte[] localbuffer = null;
            int n = serialPort.BytesToRead;
            if (n != 0)
            {
                localbuffer = new byte[n];
                try
                {
                    serialPort.Read(localbuffer, 0, n);
                }
                catch (Exception exc)
                {
                    Debug.show("SerialPort2Dynamixel.rawRead", exc.Message);
                }
            }

            mutex.ReleaseMutex();

            return localbuffer;
        }
    }
}
