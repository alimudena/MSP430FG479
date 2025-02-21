import time
from pywinusb.hid import HidDevice

def find_mcp2210():
    VID = 0x04D8  # Microchip Vendor ID
    PID = 0x00DE  # MCP2210 Product ID
    
    try:
        # Find the MCP2210 HID device using pywinusb
        devices = HidDevice.find_all(vid=VID, pid=PID)
        if devices:
            device = devices[0]
            device.open()
            print("MCP2210 connected successfully.")
            return device
        else:
            print("MCP2210 not found.")
            return None
    except Exception as e:
        print(f"Failed to connect to MCP2210: {e}")
        return None

def configure_spi(device):
    # Configure SPI settings using pywinusb
    spi_config_command = [0x20, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x0A, 0x00] + [0x00] * 52
    device.write(spi_config_command)
    # Read the response
    response = device.read(64)
    print("SPI configuration response:", response)

def spi_transfer(device, data_out):
    """Send and receive SPI data using pywinusb."""
    transfer_command = [0x42, len(data_out)] + data_out + [0x00] * (60 - len(data_out))
    device.write(transfer_command)
    time.sleep(0.1)  # Allow time for response
    response = device.read(64)
    return response[2:2+len(data_out)]  # Extract useful data

def main():
    device = find_mcp2210()
    if not device:
        return
    
    configure_spi(device)
    
    print("Reading SPI data from MSP430...")
    try:
        while True:
            received_data = spi_transfer(device, [0x00])  # Sending dummy byte to read from MISO
            print("Received data:", received_data)
            time.sleep(0.5)
    except KeyboardInterrupt:
        print("Stopping SPI communication.")
    finally:
        device.close()

if __name__ == "__main__":
    main()
