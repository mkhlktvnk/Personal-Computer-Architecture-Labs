#include <iostream>
#include <windows.h>

HANDLE COM_PORT_1;
HANDLE COM_PORT_2;
LPCTSTR NAME_PORT_1 = L"COM1";
LPCTSTR NAME_PORT_2 = L"COM2";

char read_from_port()
{
	DWORD recieved_size;
	char recieved_char;
	BOOL status = ReadFile(COM_PORT_2, &recieved_char, 1, &recieved_size, 0);
	if (!status) {
		std::cout << "Error reading port!" << "\n";
		exit(-111);
	}
	return recieved_char;
}

void set_port_state(DCB &serial_params)
{
	serial_params.DCBlength = sizeof(serial_params);
	if (!GetCommState(COM_PORT_1, &serial_params)) {
	std::cout << "Error setting port state!" << "\n";
	exit(-111);
	}
	serial_params.BaudRate = CBR_9600;
	serial_params.ByteSize = 8;
	serial_params.StopBits = ONESTOPBIT;
	serial_params.Parity = NOPARITY;
	if (!SetCommState(COM_PORT_2, &serial_params)) {
		std::cout << "Error setting port state!" << "\n";
		exit(-111);
	}
}

void write_byte_to_port(char data_to_write)
{
	DWORD size_to_write = sizeof(data_to_write);
	DWORD written_bytes;
	BOOL status = WriteFile(COM_PORT_1, &data_to_write,
	size_to_write, &written_bytes, 0);
	if (!status) {
	std::cout << "Error writing byte!" << "\n";
	exit(-111);
	}
	std::cout << "Written byte in port(COM-1): " << data_to_write << "\n";
}

int main()
{
	COM_PORT_1 = ::CreateFile(NAME_PORT_1, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_AT-
	TRIBUTE_NORMAL, 0);

	COM_PORT_2 = ::CreateFile(NAME_PORT_2, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_AT-
	TRIBUTE_NORMAL, 0);

	DCB serial_params = { 0 };
	set_port_state(serial_params);
	write_byte_to_port('A');
	std::cout << "Recieved data(COM-2): " << read_from_port() << std::endl;
}
