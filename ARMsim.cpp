/* On my honor, I have neither given nor received unauthorized aid on this assignment */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

class ARMsim {
private:

public:
    vector<int> registers;
    vector<int> memory;
    vector<string> commandList;
    int pc;
    int falsePC;
    int dataLoc;
    bool branched;
    bool disassembly;
    fstream fs1;
    fstream fs2;
    ARMsim();
    vector<string> commandManipulation(vector<string> commands, int index);
    void commandRunner(vector<string> commands);
    int btd(string s);
    int twoC(string s);
    void categoryOne(string command);
    void CBZ(int src, int offset);
    void CBNZ(int src, int offset);
    void categoryTwo(string command);
    void ORRI(int dest, int src, int immediate);
    void EORI(int dest, int src, int immediate);
    void ADDI(int dest, int src, int immediate);
    void SUBI(int dest, int src, int immediate);
    void ANDI(int dest, int src, int immediate);
    void categoryThree(string command);
    void EOR(int dest, int src1, int src2);
    void ADD(int dest, int src1, int src2);
    void SUB(int dest, int src1, int src2);
    void AND(int dest, int src1, int src2);
    void ORR(int dest, int src1, int src2);
    void LSR(int dest, int src1, int src2);
    void LSL(int dest, int src1, int src2);
    void categoryFour(string command);
    void LDUR(int srcD, int src, int immediate);
    void STUR(int srcD, int src, int immediate);
};

ARMsim::ARMsim() {
    this->registers = vector<int>(32, 0);
    this->memory = vector<int>();
    this->commandList = vector<string>();
    this->pc = 64;
    this->falsePC = 64;
    this->dataLoc = 0;
    this->branched = false;
    this->fs1.open("disassembly.txt", std::ios_base::app | std::ios_base::in | std::ios::trunc);
    this->fs2.open("simulation.txt", std::ios_base::app | std::ios_base::in | std::ios::trunc);
}

vector<string> ARMsim::commandManipulation(vector<string> commands, int index) {
    int loc = (pc - 64) / 4;
    commands.resize(index);
    for (int i = loc - 1; i < commandList.size(); i++)
        commands.push_back(commandList.at(i));
    return commands;
}

void ARMsim::commandRunner(vector<string> commands) {
    for (int i = 0; i < commands.size(); i++) {
        disassembly ? fs1 << commands.at(i) << "\t" << falsePC << "\t" : fs2 << "--------------------" << "\n" << "Cycle " << i + 1 << ":" << "\t" << pc << "\t";
        string category = commands.at(i).substr(0, 3);
        if (category == "001")
            categoryOne(commands.at(i));
        else if (category == "010")
            categoryTwo(commands.at(i));
        else if (category == "011")
            categoryThree(commands.at(i));
        else if (category == "100")
            categoryFour(commands.at(i));
        else {
            disassembly ? fs1 << "DUMMY" << "\n" : fs2 << "DUMMY" << "\n";
        }
        if (category != "001")
            pc += 4;
        falsePC += 4;
        if (branched && !disassembly) {
            branched = false;
            commands = commandManipulation(commands, i);
        }
        if (!disassembly) {
            fs2 << "\n" << "Registers";
            for (int i = 0; i < 32; i++) {
                if (i % 8 == 0)
                    fs2 << "\n" << "X" << (i % 10 == i ? "0" : "") << i << ":" << "\t";
                fs2 << registers.at(i) << "\t";
            }
            fs2 << "\n" << "\n" << "Data";
            for (int i = 0; i < memory.size(); i++) {
                if (i % 8 == 0)
                    fs2 << "\n" << i * 4 + dataLoc << ":" << "\t";
                fs2 << memory.at(i) << "\t";
            }
            fs2 << "\n" << "\n";
        }
    }
}

int ARMsim::btd(string s) {
    string val = s;
    int decimal = 0;
    int base = 1;
    string temp = val;
    while (temp.length() != 0) {
        int digit = stoi(temp.substr(temp.length() - 1, 1));
        temp = temp.substr(0, temp.length() - 1);
        decimal += digit * base;
        base *= 2;
    }
    return decimal;
}

int ARMsim::twoC(string s) {
    int val = 0;
    string binary = s;
    int sizeVal = binary.length();
    string binaryFlip = "";
    string sign = binary.substr(0, 1);
    if (sign == "1") {
        for (int i = 0; i < sizeVal; i++) {
            if (binary.at(i) == '0')
                binaryFlip += "1";
            else
                binaryFlip += "0";
        }
        binary = binaryFlip;
        val = btd(binary) + 1;
    }
    return (sign == "1" ? -1 * val : btd(binary));
}

void ARMsim::categoryOne(string command) {
    int src = btd(command.substr(8, 5));
    int offset = twoC(command.substr(13));
    string opcode = command.substr(3, 5);
    if (opcode == "10000") {
        CBZ(src, offset);
    }
    else {
        CBNZ(src, offset);
    }
}

void ARMsim::CBZ(int src, int offset) {
    if (registers.at(src) == 0) {
        pc += offset * 4;
        branched = true;
    }
    else
        pc += 4;
    string s1 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "CBZ X" << s1 << ", #" << offset << "\n" : fs2 << "CBZ X" << s1 << ", #" << offset << "\n";
}

void ARMsim::CBNZ(int src, int offset) {
    if (registers.at(src) != 0) {
        pc += offset * 4;
        branched = true;
    }
    else
        pc += 4;
    string s1 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "CBNZ X" << s1 << ", #" << offset << "\n" : fs2 << "CBNZ X" << s1 << ", #" << offset << "\n";
}

void ARMsim::categoryTwo(string command) {
    int dest = btd(command.substr(10, 5));
    int src = btd(command.substr(15, 5));
    int immediate = twoC(command.substr(20, 12));
    string opcode = command.substr(3, 7);
    if (opcode == "1000000")
        ORRI(dest, src, immediate);
    else if (opcode == "1000001")
        EORI(dest, src, immediate);
    else if (opcode == "1000010")
        ADDI(dest, src, immediate);
    else if (opcode == "1000011")
        SUBI(dest, src, immediate);
    else
        ANDI(dest, src, immediate);
}

void ARMsim::ORRI(int dest, int src, int immediate) {
    int a = registers.at(src);
    registers.at(dest) = a | immediate;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "ORRI X" << s1 << ", X" << s2 << ", #" << immediate << "\n" : fs2 << "ORRI X" << s1 << ", X" << s2 << ", #" << immediate << "\n";
}

void ARMsim::EORI(int dest, int src, int immediate) {
    int a = registers.at(src);
    registers.at(dest) = a ^ immediate;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "EORI X" << s1 << ", X" << s2 << ", #" << immediate << "\n" : fs2 << "EORI X" << s1 << ", X" << s2 << ", #" << immediate << "\n";
}

void ARMsim::ADDI(int dest, int src, int immediate) {
    int a = registers.at(src);
    registers.at(dest) = a + immediate;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "ADDI X" << s1 << ", X" << s2 << ", #" << immediate << "\n" : fs2 << "ADDI X" << s1 << ", X" << s2 << ", #" << immediate << "\n";
}

void ARMsim::SUBI(int dest, int src, int immediate) {
    int a = registers.at(src);
    registers.at(dest) = a - immediate;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "SUBI X" << s1 << ", X" << s2 << ", #" << immediate << "\n" : fs2 << "SUBI X" << s1 << ", X" << s2 << ", #" << immediate << "\n";
}

void ARMsim::ANDI(int dest, int src, int immediate) {
    int a = registers.at(src);
    registers.at(dest) = a & immediate;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "ANDI X" << s1 << ", X" << s2 << ", #" << immediate << "\n" : fs2 << "ANDI X" << s1 << ", X" << s2 << ", #" << immediate << "\n";
}

void ARMsim::categoryThree(string command) {
    int dest = btd(command.substr(11, 5));
    int src1 = btd(command.substr(16, 5));
    int src2 = btd(command.substr(21, 5));
    string opcode = command.substr(3, 8);
    if (opcode == "10100000")
        EOR(dest, src1, src2);
    else if (opcode == "10100010")
        ADD(dest, src1, src2);
    else if (opcode == "10100011")
        SUB(dest, src1, src2);
    else if (opcode == "10100100")
        AND(dest, src1, src2);
    else if (opcode == "10100101")
        ORR(dest, src1, src2);
    else if (opcode == "10100110")
        LSR(dest, src1, src2);
    else
        LSL(dest, src1, src2);
}

void ARMsim::EOR(int dest, int src1, int src2) {
    int a = registers.at(src1);
    int b = registers.at(src2);
    registers.at(dest) = a ^ b;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src1 == 31 ? "ZR" : to_string(src1);
    string s3 = src2 == 31 ? "ZR" : to_string(src2);
    disassembly ? fs1 << "EOR X" << s1 << ", X" << s2 << ", X" << s3 << "\n" : fs2 << "EOR X" << s1 << ", X" << s2 << ", X" << s3 << "\n";
}

void ARMsim::ADD(int dest, int src1, int src2) {
    int a = registers.at(src1);
    int b = registers.at(src2);
    registers.at(dest) = a + b;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src1 == 31 ? "ZR" : to_string(src1);
    string s3 = src2 == 31 ? "ZR" : to_string(src2);
    disassembly ? fs1 << "ADD X" << s1 << ", X" << s2 << ", X" << s3 << "\n" : fs2 << "ADD X" << s1 << ", X" << s2 << ", X" << s3 << "\n";
}

void ARMsim::SUB(int dest, int src1, int src2) {
    int a = registers.at(src1);
    int b = registers.at(src2);
    registers.at(dest) = a - b;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src1 == 31 ? "ZR" : to_string(src1);
    string s3 = src2 == 31 ? "ZR" : to_string(src2);
    disassembly ? fs1 << "SUB X" << s1 << ", X" << s2 << ", X" << s3 << "\n" : fs2 << "SUB X" << s1 << ", X" << s2 << ", X" << s3 << "\n";
}

void ARMsim::AND(int dest, int src1, int src2) {
    int a = registers.at(src1);
    int b = registers.at(src2);
    registers.at(dest) = a & b;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src1 == 31 ? "ZR" : to_string(src1);
    string s3 = src2 == 31 ? "ZR" : to_string(src2);
    disassembly ? fs1 << "AND X" << s1 << ", X" << s2 << ", X" << s3 << "\n" : fs2 << "AND X" << s1 << ", X" << s2 << ", X" << s3 << "\n";
}

void ARMsim::ORR(int dest, int src1, int src2) {
    int a = registers.at(src1);
    int b = registers.at(src2);
    registers.at(dest) = a | b;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src1 == 31 ? "ZR" : to_string(src1);
    string s3 = src2 == 31 ? "ZR" : to_string(src2);
    disassembly ? fs1 << "ORR X" << s1 << ", X" << s2 << ", X" << s3 << "\n" : fs2 << "ORR X" << s1 << ", X" << s2 << ", X" << s3 << "\n";
}

void ARMsim::LSR(int dest, int src1, int src2) {
    int a = registers.at(src1);
    int b = registers.at(src2) % 32;
    registers.at(dest) = a >> b;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src1 == 31 ? "ZR" : to_string(src1);
    string s3 = src2 == 31 ? "ZR" : to_string(src2);
    disassembly ? fs1 << "LSR X" << s1 << ", X" << s2 << ", X" << s3 << "\n" : fs2 << "LSR X" << s1 << ", X" << s2 << ", X" << s3 << "\n";
}

void ARMsim::LSL(int dest, int src1, int src2) {
    int a = registers.at(src1);
    int b = registers.at(src2) % 32;
    registers.at(dest) = a << b;
    string s1 = dest == 31 ? "ZR" : to_string(dest);
    string s2 = src1 == 31 ? "ZR" : to_string(src1);
    string s3 = src2 == 31 ? "ZR" : to_string(src2);
    disassembly ? fs1 << "LSL X" << s1 << ", X" << s2 << ", X" << s3 << "\n" : fs2 << "LSL X" << s1 << ", X" << s2 << ", X" << s3 << "\n";
}

void ARMsim::categoryFour(string command) {
    int srcD = btd(command.substr(11, 5));
    int src = btd(command.substr(16, 5));
    int immediate = twoC(command.substr(21, 11));
    string opcode = command.substr(3, 8);
    if (opcode == "10101010")
        LDUR(srcD, src, immediate);
    else
        STUR(srcD, src, immediate);
}

void ARMsim::LDUR(int srcD, int src, int immediate) {
    int loc = (registers.at(src) + immediate - dataLoc) / 4;
    registers.at(srcD) = memory.at(loc);
    string s1 = srcD == 31 ? "ZR" : to_string(srcD);
    string s2 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "LDUR X" << s1 << ", [X" << s2 << ", #" << immediate << "]" << "\n" : fs2 << "LDUR X" << s1 << ", [X" << s2 << ", #" << immediate << "]" << "\n";
}

void ARMsim::STUR(int srcD, int src, int immediate) {
    int loc = (registers.at(src) + immediate - dataLoc) / 4;
    memory.at(loc) = registers.at(srcD);
    string s1 = srcD == 31 ? "ZR" : to_string(srcD);
    string s2 = src == 31 ? "ZR" : to_string(src);
    disassembly ? fs1 << "STUR X" << s1 << ", [X" << s2 << ", #" << immediate << "]" << "\n" : fs2 << "STUR X" << s1 << ", [X" << s2 << ", #" << immediate << "]" << "\n";
}

int main(int argc, char* argv[])
{
    vector<string> commands;
    vector<string> data;
    string fileName = argv[1];
    fstream file;
    file.open(fileName);
    string line;
    ARMsim das;
    ARMsim as;
    if (file.is_open()){
        while (getline(file, line))
            commands.push_back(line);
        file.close();
    }
    int dummyIndex = -1;
    for (int i = 0; i < commands.size(); i++) {
        if (commands.at(i) == "10100000000000000000000000000000")
            dummyIndex = i;
        if (dummyIndex != -1 && i > dummyIndex)
            data.push_back(commands.at(i));
    }
    for (auto i : data)
        commands.erase(find(commands.begin(), commands.end(), i));
    for (auto i : commands) {
        as.commandList.push_back(i);
        das.commandList.push_back(i);
    }
    as.dataLoc = 64 + (commands.size() * 4);
    das.dataLoc = 64 + (commands.size() * 4);
    for (auto i : data) {
        int x = as.twoC(i);
        as.memory.push_back(x);
        das.memory.push_back(x);
    }
    das.disassembly = true;
    das.commandRunner(commands);
    as.disassembly = false;
    as.commandRunner(commands);
    for (auto i : data) {
        int x = das.twoC(i);
        das.fs1 << i << "\t" << das.falsePC << "\t" << x << "\n";
        das.falsePC += 4;
    }
    das.fs1.close();
}