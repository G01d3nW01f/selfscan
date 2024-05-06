#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>
#include <unistd.h>



void check_user_id(){

	uid_t uid = getuid();

	if(uid != 0){
		std::cout << "[!]Not root user" << std::endl;
		std::cout << "[!]if you wanna get list of id for listen port" <<
			", you should execute this as root user" << std::endl;
		std::cout << std::endl;
	}
}



std::vector<int> extract_ports(const std::vector<std::string>& output_lines) {
    std::vector<int> ports;

    for (const auto& line : output_lines) {
        std::string port_str;
        std::istringstream iss(line);
        if (std::getline(iss, port_str, ':')) {
            if (std::getline(iss, port_str)) {
		    //forDebug
                //std::cout << "Port string: " << port_str << std::endl; 
                try {
                    int port = std::stoi(port_str);
                    ports.push_back(port);
                } catch (const std::invalid_argument& e) {
                    //std::cerr << "Invalid port string: " << port_str << std::endl;
                }
            }
        }
    }

    return ports;
}



void execute_lsof_i(const std::vector<int>& ports) {
    for (int port : ports) {
        std::string command = "lsof -i :" + std::to_string(port) + " | awk '{print $1,$9,$10}'";
        //std::cout << "Executing command: " << command << std::endl;
        system(command.c_str());
    }
}

void execute_lsof_i_for_listen(const std::vector<int>& ports) {
    for (int port : ports) {
        std::string command = "lsof -i :" + std::to_string(port);
        //std::cout << "Executing command: " << command << std::endl;
        system(command.c_str());
    }
}


std::vector<std::string> capture_netstat_established() {
    std::vector<std::string> output_lines;

    std::string command = "netstat -antuln | grep ESTABLISHED";
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to execute netstat command." << std::endl;
        return output_lines;
    }

    char buffer[128];
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            output_lines.push_back(buffer);
    }
    pclose(pipe);

    return output_lines;
}

std::vector<std::string> capture_netstat_listen() {
    std::vector<std::string> output_lines;

    std::string command = "netstat -antuln | grep LISTEN";
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to execute netstat command." << std::endl;
        return output_lines;
    }

    char buffer[128];
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            output_lines.push_back(buffer);
    }
    pclose(pipe);

    return output_lines;
}

int main() {

    check_user_id();

    std::vector<std::string> output_lines = capture_netstat_established();
    std::vector<std::string> output_lines2 = capture_netstat_listen();	

    std::cout << "[+]Established" << std::endl;
    for (const auto& line : output_lines) {
        std::cout << line;
    }

    std::cout << std::endl;


    std::cout << "--------------------------------" << std::endl;

    std::vector<int> ports = extract_ports(output_lines);
    execute_lsof_i(ports);

    std::cout << std::endl;

    std::cout << "[+]Listen" << std::endl;
    for (const auto& line : output_lines2) {
	std::cout << line;
    }

    std::cout << std::endl;

    std::vector<int> ports2 = extract_ports(output_lines2);
    execute_lsof_i_for_listen(ports2);
    std::cout << std::endl;	

    return 0;
}

