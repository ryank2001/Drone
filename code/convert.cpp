#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <windows.h>

std::unordered_map<std::string, std::vector<int>> commands = {
    {"CHAR", {0x01, 1}},
    {"INT", {0x02, 2}},
    {"FLOAT", {0x03, 4}},
    {"STRING", {0x04, -1}},
    {"SET", {0x05, -2}},
    {"GET", {0x06, -2}},
    {"INCREMENT", {0x07, 0}},
    {"DECREMENT", {0x08, 0}},
    {"PLUS", {0x09, 0}},
    {"MINUS", {0x0A, 0}},
    {"TIMES", {0x0B, 0}},
    {"DIVIDE", {0x0C, 0}},
    {"MODULO", {0x0D, 0}},
    {"UNARYMINUS", {0x0E, 0}},
    {"EQUALS", {0x0F, 0}},
    {"NOTEQUALS", {0x10, 0}},
    {"LESSTHAN", {0x11, 0}},
    {"LESSTHANOREQUALS", {0x12, 0}},
    {"GREATERTHAN", {0x13, 0}},
    {"GREATERTHANOREQUALS", {0x14, 0}},
    {"LOGICALAND", {0x15, 0}},
    {"LOGICALOR", {0x16, 0}},
    {"LOGICALXOR", {0x17, 0}},
    {"LOGICALNOT", {0x18, 0}},
    {"BITWISEAND", {0x19, 0}},
    {"BITWISEOR", {0x1A, 0}},
    {"BITWISEXOR", {0x1B, 0}},
    {"BITWISENOT", {0x1C, 0}},
    {"TOCHAR", {0x1D, 0}},
    {"TOINT", {0x1E, 0}},
    {"TOFLOAT", {0x1F, 0}},
    {"ROUND", {0x20, 0}},
    {"FLOOR", {0x21, 0}},
    {"CEILING", {0x22, 0}},
    {"MIN", {0x23, 0}},
    {"MAX", {0x24, 0}},
    {"ABS", {0x25, 0}},
    {"CONSTANT", {0x26, 0}},
    {"MAP", {0x27, 0}},
    {"POW", {0x28, 0}},
    {"SQRT", {0x29, 0}},
    {"DELAY", {0x2A, 0}},
    {"DELAYUNTIL", {0x2B, 0}},
    {"MILLIS", {0x2C, 0}},
    {"PINMODE", {0x2D, 0}},
    {"ANALOGREAD", {0x2E, 0}},
    {"ANALOGWRITE", {0x2F, 0}},
    {"DIGITALREAD", {0x30, 0}},
    {"DIGITALWRITE", {0x31, 0}},
    {"PRINT", {0x32, 0}},
    {"PRINTLN", {0x33, 0}},
    {"OPEN", {0x34, 0}},
    {"CLOSE", {0x35, 0}},
    {"WRITE", {0x36, 0}},
    {"READINT", {0x37, 0}},
    {"READFLOAT", {0x38, 0}},
    {"READSTRING", {0x39, 0}},
    {"READCHAR", {0x3A, 0}},
    {"IF", {0x3B, 1}},
    {"ELSE", {0x3C, 1}},
    {"ENDIF", {0x3D, 0}},
    {"WHILE", {0x3E, 2}},
    {"ENDWHILE", {0x3F, 0}},
    {"LOOP", {0x40, 0}},
    {"ENDLOOP", {0x41, 0}},
    {"STOP", {0x42, 0}},
    {"FORK", {0x43, 0}},
    {"WAITUNTILLDONE", {0x44, 0}}
};

int main() {
    std::string filename = "test.txt";
    std::string port = "\\\\.\\COM10";

    HANDLE serial = CreateFile("\\\\.\\COM11", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (serial == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to open serial port." << std::endl;
        return 1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(serial, &dcbSerialParams)) {
        std::cout << "Failed to get serial port parameters." << std::endl;
        CloseHandle(serial);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(serial, &dcbSerialParams)) {
        std::cout << "Failed to set serial port parameters." << std::endl;
        CloseHandle(serial);
        return 1;
    }
    
    std::string line;
    std::ifstream openFile("LEDS.txt");
    if (openFile.is_open()) {
        
        std::vector<char> byteList;
        int counter = 0;
        while (openFile.eof() == false){
            

            getline(openFile, line, ' ');
            if(counter > 0){
                byteList.push_back(atoi(line.c_str()));
                counter--;
            }
            else if(counter == -1){
                if( line == "0"){
                 
                    byteList.push_back(0);
                    counter = 0;
                }
                else{
                    byteList.push_back(line[1]);
                }
            }
            else if (counter == -2){
                byteList.push_back(line[1]);
                counter = 0;
            }
            else {
                if (commands.find(line) != commands.end()) {
                    byteList.push_back(commands[line][0]);
                    counter = commands[line][1];
                }
            }
            

        }
        
        
        for (int byte : byteList) {
            printf("%d ", byte);
        }
        
        Sleep(5000);
        byteList.push_back(0x66);
        byteList.push_back(' ');
        byteList.push_back('\n');
        std::string command = "store LEDS " + std::to_string(byteList.size() -2) + " ";
        printf("Size: %d\n", byteList.size());
        DWORD bytesWritten;
        WriteFile(serial, command.c_str(), command.size(), &bytesWritten, NULL);
        for (int byte : byteList) {
            WriteFile(serial, &byte, 1, &bytesWritten, NULL);
        }
        std::cout << "Data sent." << std::endl;
    } else {
        std::cout << "Failed to open file." << std::endl;
    }
    Sleep(1000);
    ReadFile(serial, &line, 1, NULL, NULL);
    printf("%s", line);

    CloseHandle(serial);
    return 0;
}