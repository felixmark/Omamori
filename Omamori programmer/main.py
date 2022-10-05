#!/usr/bin/env python3

"""
Omamori Serial Communicator

This script makes the configuration of the Omamori charm easier.
You need to install pyserial to run this script (execute pip install pyserial).
"""

from time import sleep
import os
import serial
import serial.tools.list_ports


def clear_screen():
    if os.name == 'nt':
        os.system('cls')
    else:
        os.system('clear')


def execute_command(serial_connection:object, command:str, parameters:list=None) -> str:
    serial_connection.flush()
    print("Please reset your Omamori, command will be executed in 3 seconds...")
    sleep(3)
    print("-"*80)
    if parameters and isinstance(parameters, list) and len(parameters) > 0 and parameters[0] and parameters[0] != "":
        command += ":" + ",".join(parameters)
    command += ";"
    serial_connection.write(command.encode())
    print("TX:\n" + command)
    response = serial_connection.read(100).decode()
    if response is None or response == "":
        response = "NO RESPONSE FROM SERIAL DEVICE!"
    print("\nRX:\n" + response)
    print("-"*80)


def send_own_command(serial_connection:object) -> None:
    own_command = input("Command: ")
    own_parameters = input("Parameters, devided by colons: ").split(",")
    print()
    execute_command(serial_connection, own_command, own_parameters)


def set_color(serial_connection:object) -> None:
    red = input("Red   value (0-255): ")
    grn = input("Green value (0-255): ")
    blu = input("Blue  value (0-255): ")
    wht = input("White value (0-255): ")
    print()
    execute_command(serial_connection, "COL", [red, grn, blu, wht])


def set_sleep_time(serial_connection:object) -> None:
    sleep_time = input("Sleep time (value between 1 and 100, which gets multiplied by 8s): ")
    print("Sleep time will be set to " + str(int(sleep_time) * 8) + " seconds!")
    print()
    execute_command(serial_connection, "SLP", [sleep_time])


def user_select_serial_port() -> object:
    serial_ports = list(serial.tools.list_ports.comports())
    if serial_ports is not None and len(serial_ports) > 0:
        for idx, serial_port in enumerate(serial_ports):
            print("[" + str(idx) + "] " + str(serial_port))
        print()
        selected_device = input("Select the device your Omamori is connected to: ")
        try:
            selected_device_int = int(selected_device)
            return serial_ports[selected_device_int] 
        except (ValueError, IndexError):
            print("Selection invalid.")
            exit(1)
    else:
            print("No serial devices found.")
            exit(1)


def print_main_menu(menu_points:list) -> None:
    for idx, menu_point in enumerate(menu_points):
        print("[" + str(idx) + "] " + menu_point["title"])
    print("[q] Quit")
    print()


def print_new_page() -> None:
    clear_screen()
    print(
        """     
   ____                                              
  / __ \                          _      
 / /  \ \_ __  __ _ _ __  ___ _ _(_)  _ __ _ _ ___  __ _ _ _ __ _ _ __  _ __  ___ _ _ 
 \ \__/ / '  \/ _` | '  \/ _ \ '_| | | '_ \ '_/ _ \/ _` | '_/ _` | '  \| '  \/ -_) '_|
  \____/|_|_|_\__,_|_|_|_\___/_| |_| | .__/_| \___/\__, |_| \__,_|_|_|_|_|_|_\___|_|  
                                     |_|           |___/
        """
    )


def main() -> None:
    """Main function for serial communication with Omamori."""

    main_functions = [
        { "title": "Get Info", "function": lambda serial_connection : execute_command(serial_connection, "INF") },
        { "title": "Set Color", "function": lambda serial_connection : set_color(serial_connection) },
        { "title": "Set default (color) mode", "function": lambda serial_connection : execute_command(serial_connection, "DEF") },
        { "title": "Set battery display mode", "function": lambda serial_connection : execute_command(serial_connection, "BAT") },
        { "title": "Set sleep time between blinks", "function": lambda serial_connection : set_sleep_time(serial_connection) },
        { "title": "Send own command", "function": lambda serial_connection : send_own_command(serial_connection) }
    ]

    # Display list of available serial devices and let user select one for communication
    print_new_page()
    selected_serial_port = user_select_serial_port()
    
    # Try to establish serial communication on selected device
    try:
        serial_connection = serial.Serial(str(selected_serial_port.device), 9600, timeout=1)
        input_text = None
        while input_text != "q":
            
            # Display main menu
            print_new_page()
            print_main_menu(main_functions)

            # Get user selection and execute associated function
            input_text = input("Selection: ")
            print()
            try:
                input_int = int(input_text)
                selected_function = main_functions[input_int]["function"]
                if selected_function is not None:
                    selected_function(serial_connection)
                else:
                    print("Function not implemented yet.")
            except (ValueError, IndexError) as e:
                if input_text != "q":
                    print("Selection invalid.")
                    print(e)
                else:
                    continue

            _ = input("Press Enter to continue.\n")

        # Close connection when execution finished
        serial_connection.close()
    except serial.SerialException as e:
        # Could not open serial connection
        print("Serial connection already opened.")
        exit(1)

if __name__ == '__main__':
    main()
