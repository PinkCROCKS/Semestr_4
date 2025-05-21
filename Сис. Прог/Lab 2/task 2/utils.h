#ifndef TASK_2_UTILS_H
#define TASK_2_UTILS_H

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <arpa/inet.h>
#include <stdexcept>
#include <ctime>

inline std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = {};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&now_tm, &now_c);
#else
    localtime_r(&now_c, &now_tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d_%H:%M:%S");
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

inline std::string ipToString(in_addr_t ip_addr_nbo) {
    char buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &ip_addr_nbo, buffer, INET_ADDRSTRLEN) == nullptr) {
        return "invalid_ip";
    }
    return std::string(buffer);
}

inline bool stringToIp(const std::string& ip_str, in_addr_t& ip_addr_nbo) {
    int result = inet_pton(AF_INET, ip_str.c_str(), &ip_addr_nbo);
    return result == 1;
}

inline uint16_t portToUint16(in_port_t port_nbo) {
    return ntohs(port_nbo);
}

inline in_port_t uint16ToPort(uint16_t port_hbo) {
    return htons(port_hbo);
}

#endif //TASK_2_UTILS_H
