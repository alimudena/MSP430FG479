import serial
import serial.tools.list_ports

def list_ports():
    ports = serial.tools.list_ports.comports()
    print("Available COM ports:")
    for port in ports:
        print(f"{port.device} - {port.description}")

def select_port():
    list_ports()
    port_name = input("Enter the COM port (e.g., COM3 or /dev/ttyUSB0): ")
    return port_name

def main():
    port = select_port()
    baud_rate = 115200  # Adjust as needed
    
    try:
        ser = serial.Serial(port, baud_rate, timeout=1)
        print(f"Connected to {port}")
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        return
    
    with open("received_data.txt", "w") as file:
        while True:
            user_input = input("Enter data to send (or type 'exit' to quit): ")
            if user_input.lower() == 'exit':
                break
            
            ser.write(user_input.encode())  # Send data
            print(f"Sent: {user_input}")
            
            received_data = ser.readline().decode().strip()  # Read response
            if received_data:
                print(f"Received: {received_data}")
                file.write(received_data + '\n')
    
    ser.close()
    print("Serial connection closed.")

if __name__ == "__main__":
    main()
