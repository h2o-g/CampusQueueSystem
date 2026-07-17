#include <iostream>
#include <vector>
#include <string>
#include <thread>  
#include <chrono>  
#include <windows.h> 
#include "models.h"
#include "scheduler.h"
#include "stats.h"
#include "dataloader.h"

std::vector<User*> g_memoryGarbageCollector;

// 解决控制台闪烁的核心：复位光标，原地覆盖
void clearScreen() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hOut, pos);
}

// 提取单字母用于填入表格，如果空闲则返回短划线 "-"
std::string getShortName(User* user) {
    if (user == nullptr) return "-";
    std::string full = user->getStudentId();
    return std::string(1, full.back());
}

int main() {

    std::vector<CampusResource*> resources;
    resources.push_back(new LibrarySeat(101));
    resources.push_back(new LibrarySeat(102));
    resources.push_back(new WashingMachine(201));

    // 构建业务调度队列
    SystemScheduler seatScheduler;
    SystemScheduler washScheduler;
    StatsManager stats;

    //数据反序列化
    std::vector<User*> allPendingUsers = DataLoader::loadUsers("students.txt", g_memoryGarbageCollector);
    if (allPendingUsers.empty()) return 0;

    // 存储竖向表格历史记录的容器
    std::vector<std::string> verticalHistory;

    system("cls"); // 首帧清屏清除背景冗余

    for (int clock = 800; clock <= 830; clock++) {

        // 物理实体状态自刷新
        for (auto res : resources) {
            res->updateStatus(clock);
        }

        // 业务导流分线
        for (auto user : allPendingUsers) {
            if (user->getRequestTime() == clock) {
                if (user->getTargetResourceType() == 1) {
                    seatScheduler.enqueueUser(user);
                }
                else if (user->getTargetResourceType() == 2) {
                    washScheduler.enqueueUser(user);
                }
            }
        }

        // 专属排队池调度
        for (auto res : resources) {
            if (res->isFree()) {
                if (res->getResourceType() == 1 && !seatScheduler.isEmpty()) {
                    User* topUser = seatScheduler.dequeueTopUser();
                    stats.recordService(topUser->getRequestTime(), clock);
                    res->occupy(topUser, clock);
                }
                else if (res->getResourceType() == 2 && !washScheduler.isEmpty()) {
                    User* topUser = washScheduler.dequeueTopUser();
                    stats.recordService(topUser->getRequestTime(), clock);
                    res->occupy(topUser, clock);
                }
            }
        }

        int min = clock % 100;
        std::string minStr = (min < 10 ? "0" : "") + std::to_string(min);

        std::string r0 = getShortName(resources[0]->getUser());
        std::string r1 = getShortName(resources[1]->getUser());
        std::string r2 = getShortName(resources[2]->getUser());

        std::string row = "|     " + minStr + "     |       " + r0 + "        |       " + r1 + "        |       " + r2 + "        |";
        verticalHistory.push_back(row);


        clearScreen();

        std::cout << "===================== 校园资源多业务动态监控大屏 =====================\n\n";
        std::cout << " [当前系统时间] 0" << clock / 100 << ":" << minStr << "\n";
        std::cout << " [专属队列积压] 图书馆座位排队: " << seatScheduler.getQueueSize()
            << " 人   |   洗衣房排队: " << washScheduler.getQueueSize() << " 人\n\n";

        // 打印表头
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << "| 时间(8:xx) |   101 (座位)   |   102 (座位)   |  201 (洗衣机)  |\n";
        std::cout << "-----------------------------------------------------------------\n";

       // 逐行打印历史记录（限制最多只显示最近的 15 行）
        int maxRows = 15;
        int startIndex = verticalHistory.size() > maxRows ? verticalHistory.size() - maxRows : 0;

        for (size_t i = startIndex; i < verticalHistory.size(); ++i) {
            std::cout << verticalHistory[i] << "\n";
        }
        std::cout << "-----------------------------------------------------------------\n\n";

        // 智能动态提示信息区
        int totalWaiting = seatScheduler.getQueueSize() + washScheduler.getQueueSize();
        if (totalWaiting > 0) {
            std::cout << " [监控提示] 业务高峰拥堵，双业务队列正在智能分流叫号...           \n";
        }
        else {
            std::cout << " [监控提示] 业务高峰期已消化完毕，当前排队序列均已安全清零。      \n";
        }

        // 动画走字延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    int totalRemaining = seatScheduler.getQueueSize() + washScheduler.getQueueSize();
    stats.printReport(totalRemaining);

    for (auto res : resources) delete res;
    for (auto user : g_memoryGarbageCollector) delete user;

    std::cout << "\n========== 结束 ==========" << std::endl;
    return 0;
}