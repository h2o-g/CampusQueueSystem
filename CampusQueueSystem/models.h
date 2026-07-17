#pragma once
#include <iostream>
#include <string>

// 定义资源状态枚举
enum class ResourceStatus {
    FREE,           // 空闲
    OCCUPIED,       // 被占用
    MAINTENANCE     // 维护中
};

// 实体类：User (学生用户) 
class User {
private:
    std::string studentId;
    int credibility;       // 信誉分 (0-100)
    int requestTime;       // 发起请求的系统时间戳
    int urgency;           // 任务紧急度 (1:低, 2:中, 3:高)
    int targetResourceType;// 目标资源类型 (1: 座位, 2: 洗衣机)

public:
    User(std::string sid, int cred, int time, int urg, int target)
        : studentId(sid), credibility(cred), requestTime(time), urgency(urg), targetResourceType(target) {}

    // Getter 方法
    std::string getStudentId() const { return studentId; }
    int getCredibility() const { return credibility; }
    int getRequestTime() const { return requestTime; }
    int getUrgency() const { return urgency; }
    int getTargetResourceType() const { return targetResourceType; }
};

//抽象基类：CampusResource
class CampusResource {
protected:
    int id;
    int resourceType;      // 资源类型标识
    ResourceStatus status;
    User* currentUser;     // 指向当前占用该资源的用户指针

public:
    CampusResource(int resId, int resType)
        : id(resId), resourceType(resType), status(ResourceStatus::FREE), currentUser(nullptr) {}

    virtual ~CampusResource() = default;

    // 虚函数接口
    virtual void updateStatus(int currentTime) = 0;
    virtual void displayInfo() const = 0;

    // 通用有效方法
    bool isFree() const { return status == ResourceStatus::FREE; }
    int getId() const { return id; }
    int getResourceType() const { return resourceType; }
    User* getUser() const { return currentUser; }

    virtual void occupy(User* user, int currentTime) {
        status = ResourceStatus::OCCUPIED;
        currentUser = user;
    }
};

// 派生类：LibrarySeat
class LibrarySeat : public CampusResource {
private:
    int releaseTime;

public:
    LibrarySeat(int resId) : CampusResource(resId, 1), releaseTime(-1) {}

    void occupy(User* user, int currentTime) override {
        CampusResource::occupy(user, currentTime);
        releaseTime = currentTime + 3; // 固定占用3分钟
    }

    void updateStatus(int currentTime) override {
        if (status == ResourceStatus::OCCUPIED && currentTime >= releaseTime) {
            status = ResourceStatus::FREE;
            currentUser = nullptr;
            releaseTime = -1;
        }
    }

    void displayInfo() const override {
        std::cout << "[图书馆座位 " << id << "] 状态: "
            << (status == ResourceStatus::FREE ? "空闲" : "占用") << std::endl;
    }
};

//派生类：WashingMachine
class WashingMachine : public CampusResource {
private:
    int releaseTime;

public:
    WashingMachine(int resId) : CampusResource(resId, 2), releaseTime(-1) {}

    void occupy(User* user, int currentTime) override {
        CampusResource::occupy(user, currentTime);
        releaseTime = currentTime + 5; // 固定占用5分钟
    }

    void updateStatus(int currentTime) override {
        if (status == ResourceStatus::OCCUPIED && currentTime >= releaseTime) {
            status = ResourceStatus::FREE;
            currentUser = nullptr;
            releaseTime = -1;
        }
    }

    void displayInfo() const override {
        std::cout << "[宿舍洗衣机 " << id << "] 状态: "
            << (status == ResourceStatus::FREE ? "空闲" : "运行中") << std::endl;
    }
};