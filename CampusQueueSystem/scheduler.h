#pragma once
#include <queue>
#include <vector>
#include "models.h"

// 优先级核心比较机制 (多维权重计分公式)
struct UserCompare {
    bool operator()(const User* u1, const User* u2) const {
        int score1 = u1->getUrgency() * 50 + u1->getCredibility() - u1->getRequestTime() * 2;
        int score2 = u2->getUrgency() * 50 + u2->getCredibility() - u2->getRequestTime() * 2;
        return score1 < score2; // 得分高的处于堆顶
    }
};

class SystemScheduler {
private:
    std::priority_queue<User*, std::vector<User*>, UserCompare> waitQueue;

public:
    void enqueueUser(User* user) {
        waitQueue.push(user);
    }

    User* dequeueTopUser() {
        if (waitQueue.empty()) return nullptr;
        User* top = waitQueue.top();
        waitQueue.pop();
        return top;
    }

    bool isEmpty() const { return waitQueue.empty(); }
    int getQueueSize() const { return waitQueue.size(); }
};