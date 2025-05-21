#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

enum class Level {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
private:

    Level level_;
    std::vector<std::ostream*> handlers_;
    std::vector<std::unique_ptr<std::ostream>> owned_streams_;

    Logger(Level level,
           std::vector<std::ostream*> handlers,
           std::vector<std::unique_ptr<std::ostream>> owned_streams)
            : level_(level),
              handlers_(std::move(handlers)),
              owned_streams_(std::move(owned_streams)) {}

    static std::string levelToString(Level level) {
        switch (level) {
            case Level::DEBUG:   return "DEBUG";
            case Level::INFO:    return "INFO";
            case Level::WARNING: return "WARNING";
            case Level::ERROR:   return "ERROR";
            case Level::CRITICAL:return "CRITICAL";
            default:             return "UNKNOWN";
        }
    }

public:

    ~Logger() {
        close();
    }

    void close() {
        handlers_.clear();
        owned_streams_.clear();
    }

    void log(Level level, const std::string& message) {
        if (level < level_) return;

        const std::string formatted = "[" + levelToString(level) + "] " + message + "\n";
        for (auto handler : handlers_) {
            if (handler) {
                *handler << formatted;
                handler->flush();
            }
        }
    }

    void debug(const std::string& msg) { log(Level::DEBUG, msg); }
    void info(const std::string& msg) { log(Level::INFO, msg); }
    void warning(const std::string& msg) { log(Level::WARNING, msg); }
    void error(const std::string& msg) { log(Level::ERROR, msg); }
    void critical(const std::string& msg) { log(Level::CRITICAL, msg); }

    class Builder {
    public:
        Builder() : level_(Level::INFO) {}

        Builder& addFileHandler(const std::string& filename) {
            auto stream = std::make_unique<std::ofstream>(filename, std::ios::app);
            if (stream->is_open()) {
                handlers_.push_back(stream.get());
                owned_streams_.push_back(std::move(stream));
            }
            return *this;
        }

        Builder& addStreamHandler(std::ostream& stream) {
            handlers_.push_back(&stream);
            return *this;
        }

        Builder& setLevel(Level level) {
            level_ = level;
            return *this;
        }

        Logger build() {
            return Logger(level_, std::move(handlers_), std::move(owned_streams_));
        }
    private:
        Level level_;
        std::vector<std::ostream*> handlers_;
        std::vector<std::unique_ptr<std::ostream>> owned_streams_;
    };
};