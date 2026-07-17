#pragma once
#include <iostream>
#include <vector>

class StatsManager {
private:
    int totalWaitTime = 0;
    int totalServedCount = 0;
    std::vector<int> waitTimes;

public:
    void recordService(int requestTime, int servedTime) {
        int waitTime = servedTime - requestTime;
        totalWaitTime += waitTime;
        totalServedCount++;
        waitTimes.push_back(waitTime);
    }

    void printReport(int remainingQueueSize) const {
        std::cout << "\n==============================================" << std::endl;
        std::cout << "          校园智能排队系统 - 仿真效益报告          " << std::endl;
        std::cout << "==============================================" << std::endl;
        std::cout << "[基础数据] 成功接受服务的学生总数: " << totalServedCount << " 人" << std::endl;
        std::cout << "[基础数据] 仿真结束时仍在排队人数: " << remainingQueueSize << " 人" << std::endl;
        std::cout << "[基础数据] 累计排队等待总时长: " << totalWaitTime << " 分钟" << std::endl;
        std::cout << "----------------------------------------------" << std::endl;
        if (totalServedCount > 0) {
            double avgWait = static_cast<double>(totalWaitTime) / totalServedCount;
            std::cout << " 核心评价指标 -> 平均等待时间: " << avgWait << " 分钟/人" << std::endl;
        }
        else {
            std::cout << " 核心评价指标 -> 平均等待时间: 0 分钟/人" << std::endl;
        }
        std::cout << "==============================================" << std::endl;
    }
};