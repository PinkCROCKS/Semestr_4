#include "analyzer.h"

#include <string>
#include <map>
#include <stdexcept>
#define NO_FINDES(a) (a == std::string::npos)


bool parseLogLine(const std::string& logger_message, std::map<std::string, std::string>& fields) {
    fields.clear();
    size_t level_end_bracket_pos = logger_message.find("] ");
    if (NO_FINDES(level_end_bracket_pos)) {
        return false;
    }
    size_t current_pos = level_end_bracket_pos + 2;
    if (current_pos >= logger_message.length() || NO_FINDES(current_pos)) {
        return false;
    }
    while (current_pos < logger_message.length()) {
        size_t colon_pos = logger_message.find(':', current_pos);
        if (NO_FINDES(colon_pos)) {
            break;
        }
        size_t key_start_pos = logger_message.rfind(' ', colon_pos);
        if (NO_FINDES(key_start_pos) || key_start_pos < current_pos) {
            key_start_pos = current_pos;
        } else {
            key_start_pos += 1;
        }
        std::string key = logger_message.substr(key_start_pos, colon_pos - key_start_pos);
        if (key.empty()) {
            current_pos = colon_pos + 1;
            continue;
        }
        size_t value_start_pos = colon_pos + 1;
        if (value_start_pos < logger_message.length() && logger_message[value_start_pos] == ' ') {
            value_start_pos++;
        }

        size_t next_key_indicator_pos = std::string::npos;
        size_t search_from = value_start_pos;
        while(search_from < logger_message.length()) {
            size_t next_colon = logger_message.find(':', search_from);
            if (NO_FINDES(next_colon))
                break;

            size_t potential_key_start = logger_message.rfind(' ', next_colon);
            if (!NO_FINDES(potential_key_start) && potential_key_start > colon_pos) {
                if (potential_key_start + 1 < next_colon && isupper(logger_message[potential_key_start + 1])) {
                    next_key_indicator_pos = potential_key_start;
                    break;
                }
            }
            search_from = next_colon + 1;
        }
        size_t value_end_pos = (NO_FINDES(next_key_indicator_pos)) ? logger_message.length() : next_key_indicator_pos;

        std::string value = logger_message.substr(value_start_pos, value_end_pos - value_start_pos);

        fields[key] = value;

        current_pos = value_end_pos;
        if (current_pos < logger_message.length() && logger_message[current_pos] == ' ') {
            current_pos++;
        }

    }

    if (fields.find("Event") == fields.end() || fields.find("SrcIP") == fields.end()) {
        return false;
    }

    return true;
}

bool updateStatsFromFields(const std::map<std::string, std::string>& fields, IpStats& stats_entry, const std::string& perspective_ip) {
    const std::string& event_type = fields.at("Event");
    const std::string& src_ip = fields.at("SrcIP");
    std::string dst_ip = fields.count("DstIP") ? fields.at("DstIP") : "";

    uint16_t dst_port = 0;
    if (fields.count("DstPort")) {
        dst_port = static_cast<uint16_t>(std::stoul(fields.at("DstPort")));
    }
    size_t size = 0;
    if (fields.count("Size")) {
        size = std::stoul(fields.at("Size"));
    }

    if (perspective_ip == src_ip) {
        if (event_type == "Connect") {
            stats_entry.connections_initiated++;
            if (!dst_ip.empty())
                stats_entry.peer_ips.insert(dst_ip);
        }
        else if (event_type == "Disconnect" || event_type == "Disconnect_Abrupt")
            stats_entry.disconnects++;
        else if (event_type == "SendData") {
            stats_entry.send_data_events++;
            stats_entry.total_sent_bytes += size;
            if (!dst_ip.empty())
                stats_entry.peer_ips.insert(dst_ip);
            if (dst_port > 0) {
                stats_entry.sent_to_ports[dst_port].sent_bytes += size;
                stats_entry.sent_to_ports[dst_port].send_packets++;
            }
        } else if (event_type == "ReceiveData") {
            stats_entry.receive_data_events++;
            stats_entry.total_received_bytes += size;
            if (!dst_ip.empty()) stats_entry.peer_ips.insert(dst_ip);
        } else
            return false;
    } else if (perspective_ip == dst_ip) {
        if (event_type == "Connect") {
            stats_entry.connections_accepted++;
            stats_entry.peer_ips.insert(src_ip);
        } else if (event_type == "SendData") {
            stats_entry.receive_data_events++;
            stats_entry.total_received_bytes += size;
            stats_entry.peer_ips.insert(src_ip);
        }
    } else
        return false;
    return true;
}

IpStats& findOrCreateLocalStats(std::map<std::string, IpStats>& local_stats_map, const std::string& ip_key) {
    if (ip_key.empty() || ip_key == "invalid_ip") {
        throw std::runtime_error("Attempting to find/create stats for an invalid IP key.");
    }
    auto it = local_stats_map.find(ip_key);
    if (it == local_stats_map.end()) {
        it = local_stats_map.emplace(ip_key, IpStats{}).first;
    }
    return it->second;
}

void* analyzerThread(void* data) {
    auto* threadData = static_cast<AnalyzerThreadData*>(data);
    if (!threadData || !threadData->queue_ptr || !threadData->shutdown_flag_ptr || !threadData->shutdown_mutex_ptr || !threadData->analyzer_stats_ptr) {
        return nullptr;
    }

    printf("Analyzer thread %d started.\n", threadData->thread_id);

    int processed_count = 0;
    std::map<std::string, IpStats>& local_stats_map = threadData->analyzer_stats_ptr->local_stats;

    try {
        std::string log_line;
        while (threadData->queue_ptr->Pop(log_line)) {
            processed_count++;

            std::map<std::string, std::string> fields;
            if (!parseLogLine(log_line, fields)) {
                continue;
            }

            const std::string& src_ip_key = fields.at("SrcIP");
            std::string dst_ip_key = fields.count("DstIP") ? fields.at("DstIP") : "";

            try {
                IpStats& src_stats_entry = findOrCreateLocalStats(local_stats_map, src_ip_key);
                updateStatsFromFields(fields, src_stats_entry, src_ip_key);
            } catch (const std::exception& e) {
                fprintf(stderr, "Analyzer %d Error (SrcIP: %s): %s in line: %s\n", threadData->thread_id, src_ip_key.c_str(), e.what(), log_line.c_str());
            }

            if (!dst_ip_key.empty() && dst_ip_key != src_ip_key) {
                try {
                    IpStats& dst_stats_entry = findOrCreateLocalStats(local_stats_map, dst_ip_key);
                    updateStatsFromFields(fields, dst_stats_entry, dst_ip_key);
                } catch (const std::exception& e) {
                    fprintf(stderr, "Analyzer %d Error (DstIP: %s): %s in line: %s\n", threadData->thread_id, dst_ip_key.c_str(), e.what(), log_line.c_str());
                }
            }
        }

    } catch (const std::exception& e) {
        fprintf(stderr, "Analyzer thread %d FATAL Error: Unhandled exception in main loop: %s\n", threadData->thread_id, e.what());
    }

    printf("Analyzer thread %d shutting down. Processed %d logs.\n", threadData->thread_id, processed_count);

    return nullptr;
}
