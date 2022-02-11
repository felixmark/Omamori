"""
Omamori Serial Communicator

This script makes the configuration of the Omamori charm easier.
You need to install pyserial to run this script (execute pip install pyserial).

Written by Felix Mark.
(It is not very pretty but functional)
"""

from time import sleep
import serial
import serial.tools.list_ports


def execute_command(param_dict:dict) -> str:
    param_dict["serial_connection"].flushInput()
    print("Please reset your Omamori, command will be executed in 3 seconds...")
    sleep(3)
    print("Execution!")
    print("-"*80)
    if "parameters" in param_dict and param_dict["parameters"]:
        param_dict["command"] += ":" + ",".join(param_dict["parameters"])
    param_dict["command"] += ";"
    param_dict["serial_connection"].write(param_dict["command"].encode())
    print("TX:\n" + param_dict["command"])
    response = param_dict["serial_connection"].read(100).decode()
    if response is None or response == "":
        response = "NO RESPONSE FROM SERIAL DEVICE!"
    print("\nRX:\n" + response, end="")
    print("-"*80)


def send_own_command(param_dict:dict) -> None:
    own_command = input("Command: ")
    own_parameters = input("Parameters, devided by colons: ")
    execute_command({"serial_connection": param_dict["serial_connection"], "command": own_command, "parameters": ([own_parameters] if own_parameters else None)})


def set_color(param_dict:dict) -> None:
    red = input("Red   value (0-255): ")
    grn = input("Green value (0-255): ")
    blu = input("Blue  value (0-255): ")
    wht = input("White value (0-255): ")
    execute_command({"serial_connection": param_dict["serial_connection"], "command": "COL", "parameters": [red, grn, blu, wht]})


def set_sleep_time(param_dict:dict) -> None:
    sleep_time = input("Sleep time (value between 1 and 100, which gets multiplied by 8s): ")
    print("Sleep time will be set to " + str(int(sleep_time) * 8) + " seconds!")
    execute_command({"serial_connection": param_dict["serial_connection"], "command": "SLP", "parameters": [sleep_time]})


def main() -> None:

    # Display list of available serial devices and let user select one for communication 
    serial_ports = list(serial.tools.list_ports.comports())
    print("Omamori Serial Communicator")
    print("="*80)
    print("List of available serial connections:")
    for idx, serial_port in enumerate(serial_ports):
        print("[" + str(idx) + "] " + str(serial_port))
    print()
    selected_device = input("Select the device your Omamori is connected to: ")
    try:
        selected_device_int = int(selected_device)
        selected_serial_port = serial_ports[selected_device_int] 
    except (ValueError, IndexError):
        print("Selection invalid.")
        exit(1)
    
    # Try to establish serial communication on selected device
    try:
        serial_connection = serial.Serial(str(selected_serial_port.name), 19200, timeout=1)
        print()
        print("Serial port opened: " + str(serial_connection.name))
        
        input_text = None
        while input_text != "q":
            # Display "main menu" of the program
            print("="*80)
            print("What do you want to do?")
            menu_points = [
                {
                    "title": "Set Color", 
                    "function": set_color, 
                    "parameters": {
                        "serial_connection": serial_connection
                    }
                },
                {
                    "title": "Set default (color) mode", 
                    "function": execute_command, 
                    "parameters": {
                        "serial_connection": serial_connection, 
                        "command": "DEF"
                    }
                },
                {
                    "title": "Set battery display mode", 
                    "function": execute_command, 
                    "parameters": {
                        "serial_connection": serial_connection, 
                        "command": "BAT"
                    }
                },
                {
                    "title": "Get status", 
                    "function": execute_command, 
                    "parameters": {
                        "serial_connection": serial_connection, 
                        "command": "STA"
                    }
                },
                {
                    "title": "Set sleep time between blinks", 
                    "function": set_sleep_time, 
                    "parameters": {
                        "serial_connection": serial_connection
                    }
                },
                {
                    "title": "Send own command", 
                    "function": send_own_command, 
                    "parameters": {
                        "serial_connection": serial_connection
                    }
                }
            ]
            for idx, menu_point in enumerate(menu_points):
                print("[" + str(idx) + "] " + menu_point["title"])
            print("[q] Quit")
            print()

            # Get user selection and execute associated function
            input_text = input("Selection: ")
            try:
                input_int = int(input_text)
                selected_function = menu_points[input_int]["function"]
                if selected_function is not None:
                    selected_function(menu_points[input_int]["parameters"])
                else:
                    print("Function not implemented yet.")
            except (ValueError, IndexError):
                if input_text != "q":
                    print("Selection invalid.")

        serial_connection.close()
    except serial.SerialException:
        print("Serial connection already opened.")
        exit(1)

if __name__ == '__main__':
    main()
