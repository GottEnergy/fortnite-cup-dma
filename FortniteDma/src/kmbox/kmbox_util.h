#pragma once

inline bool connect_serial_kmbox() {
	string port = find_port("USB-SERIAL CH340");

	if (!port.empty()) {

		if (open_port(hSerial, port.c_str(), CBR_115200)) {
			std::cout << hue::green << "(+) " << hue::white << "Serial kmbox found" << std::endl;
			return true;
		}
		else
		{
			std::cout << hue::yellow << "(/) " << hue::white << "Couldnt connect to serial kmbox" << std::endl;
		}
	}
	else {
		std::cout << hue::yellow << "(/) " << hue::white << "No serial kmbox found" << std::endl;
	}
	return false;
}

inline bool connect_net_kmbox() {
	int result = kmNet_init((char*)settings::kmbox::net::ip.c_str(), (char*)settings::kmbox::net::port.c_str(), (char*)settings::kmbox::net::uuid.c_str());

	if (result == err_creat_socket ||result == err_net_rx_timeout || result == err_net_version) {
		std::cout << hue::yellow << "(/) " << hue::white << "Failed to connect to kmbox net" << std::endl;
		return false;
	}

	std::cout << hue::green << "(+) " << hue::white << "Kmbox net connected" << std::endl;
	return true;
}

inline void km_move(int X, int Y) {
	std::string command = "km.move(" + std::to_string(X) + "," + std::to_string(Y) + ")\r\n";
	send_command(hSerial, command.c_str());
}

inline void km_click() {
	std::string command = "km.click(0)\r\n";
	send_command(hSerial, command.c_str());
}
