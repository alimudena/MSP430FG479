import serial as serial_lib
import sys

def read_from_com8(output_file="output.txt", baudrate=57600, timeout=1):
    try:
        # Debugging: Check module path
        print("Using serial module from:", serial_lib.__file__)

        # Open the serial port
        with serial_lib.Serial(port="COM8", baudrate=baudrate, timeout=timeout) as ser, open(output_file, "wb") as file:
            print(f"Connected to {ser.port} at {ser.baudrate} baudrate.")
            
            while True:
                data = ser.read(1024)  # Read up to 1024 bytes at a time
                print(data)
                if data:
                    file.write(data)  # Write binary data to file
                    file.flush()
                    print(f"Received {len(data)} bytes: {data}")
                    print("Decoded Data:", data.decode(errors='ignore'))  # Print decoded data
                
    except AttributeError as e:
        print("AttributeError detected! Possible wrong serial module.")
        print(f"Error details: {e}")
        print("Try uninstalling serial and installing pyserial:")
        print("pip uninstall serial")
        print("pip install pyserial")
    except Exception as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("Program terminated by user.")

if __name__ == "__main__":
    read_from_com8()
