#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "models.h"

class DataLoader {
public:
    static std::vector<User*> loadUsers(const std::string& filename, std::vector<User*>& garbageCollector) {
        std::vector<User*> loadedUsers;
        std::ifstream inFile(filename);

        if (!inFile.is_open()) {
            std::cerr << "[错误] 无法打开数据文件: " << filename
                << " (请确保 students.txt 与程序在同一目录下!)" << std::endl;
            return loadedUsers;
        }

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            std::string name;
            int cred, time, urg, target;

            if (ss >> name >> cred >> time >> urg >> target) {
                User* newUser = new User(name, cred, time, urg, target);
                loadedUsers.push_back(newUser);
                garbageCollector.push_back(newUser); // 注册至防泄露生命周期链
            }
        }
        inFile.close();
        return loadedUsers;
    }
};