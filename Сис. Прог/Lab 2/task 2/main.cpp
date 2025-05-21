#include "generator.hpp"
#include "analyzer.h"
#include "ts_queue.hpp"
#include "ip_st.h"

#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <thread>
#include <map>
#include <cstring>
#include <stdexcept>

std::map<std::string, IpStats> statisticIn(std::vector<AnalyzerData>& p_analyzer_data_store) {
    std::map<std::string, IpStats> global_stats;
    fprintf(stdout, "\nAggregating final statistics from all analyzers...\n"); fflush(stdout);
    for (AnalyzerData& analyzer_data : p_analyzer_data_store) {
        pthread_mutex_lock(&(analyzer_data.stats_mutex));
        try {
            for (const auto& pair : analyzer_data.local_stats) global_stats[pair.first] += pair.second;
        } catch (...) {
            pthread_mutex_unlock(&(analyzer_data.stats_mutex));
            fprintf(stderr, "Error processing aggregation for one analyzer.\n"); throw;
        }
        pthread_mutex_unlock(&(analyzer_data.stats_mutex));
    }
    return global_stats;
}


int main() {
    int generator_threads = 2;
    int analyzer_threads = 3;
    int working_time = 10;
    LogLevels generator_log_level = INFO;

    printf("--- Log Generator & Analyzer ---\n");
    printf("Generators: %d\nAnalyzers:  %d\nRun Time:   %d seconds\n",
           generator_threads, analyzer_threads, working_time);
    printf("---------------------------------\n");

    ThreadSafeQueue<std::string> log_queue;
    bool shutdown = false;
    pthread_mutex_t shutdown_mutex;
    std::vector<AnalyzerData> analyzer_data_store;

    int main_ret_code = 0;
    int mutex_ret = pthread_mutex_init(&shutdown_mutex, nullptr);
    if (mutex_ret != 0) {
        return 1;
    }
    try {
        analyzer_data_store.resize(analyzer_threads);
    } catch (const std::exception& e) {
        fprintf(stderr, "FATAL: Failed to initialize analyzer data store: %s\n", e.what());
        pthread_mutex_destroy(&shutdown_mutex);
        return 1;
    }

    std::vector<pthread_t> generator_pthreads(generator_threads, 0);
    std::vector<pthread_t> analyzer_pthreads(analyzer_threads, 0);

    std::vector<GeneratorThreadData*> generator_thread_args(generator_threads, nullptr);
    std::vector<AnalyzerThreadData*> analyzer_thread_args(analyzer_threads, nullptr);
    int pthread_ret;
    bool creation_failed = false;

    fprintf(stdout, "Starting analyzer threads...\n");
    for (int i = 0; i < analyzer_threads; ++i) {
        try {
            analyzer_thread_args[i] = new AnalyzerThreadData(&log_queue, &shutdown, &shutdown_mutex, &analyzer_data_store[i], i);
            pthread_ret = pthread_create(&analyzer_pthreads[i], nullptr, analyzerThread, analyzer_thread_args[i]);
            if (pthread_ret != 0) {
                delete analyzer_thread_args[i]; analyzer_thread_args[i] = nullptr;
                fprintf(stderr, "ERROR: Analyzer pthread_create failed for thread %d: %s\n", i, strerror(pthread_ret));
                creation_failed = true;
                break;
            }
        } catch (const std::bad_alloc& ba) {
            fprintf(stderr, "ERROR: Failed to allocate memory for analyzer %d args: %s\n", i, ba.what());
            creation_failed = true;
            break;
        }
    }

    if (!creation_failed) {
        fprintf(stdout, "Starting generator threads...\n"); fflush(stdout);
        for (int i = 0; i < generator_threads; ++i) {
            try {
                generator_thread_args[i] = new GeneratorThreadData(&log_queue, &shutdown, &shutdown_mutex, i, generator_log_level);
                pthread_ret = pthread_create(&generator_pthreads[i], nullptr, generatorThread, generator_thread_args[i]);
                if (pthread_ret != 0) {
                    delete generator_thread_args[i];
                    generator_thread_args[i] = nullptr;
                    fprintf(stderr, "ERROR: Generator pthread_create failed for thread %d: %s\n", i, strerror(pthread_ret));
                    creation_failed = true;
                    break;
                }
            } catch (const std::bad_alloc& ba) {
                fprintf(stderr, "ERROR: Failed to allocate memory for generator %d args: %s\n", i, ba.what());
                creation_failed = true;
                break;
            }
        }
    }

    if (creation_failed) {
        fprintf(stderr, "FATAL: Thread creation failed. Initiating immediate shutdown.\n");
        main_ret_code = 1;

        pthread_mutex_lock(&shutdown_mutex);
        shutdown = true;
        pthread_mutex_unlock(&shutdown_mutex);

        log_queue.Shutdown();
    } else {
        fprintf(stdout, "Application running for %d seconds...\n", working_time);

        auto start_time = std::chrono::steady_clock::now();
        while (true) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
            if (elapsed.count() >= working_time) {
                fprintf(stdout, "\nRun duration reached (%ld seconds). Initiating shutdown...\n", (long)working_time); fflush(stdout);
                pthread_mutex_lock(&shutdown_mutex);
                shutdown = true;
                pthread_mutex_unlock(&shutdown_mutex);
                log_queue.Shutdown();
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds((long) 200));
        }

    }

    for (int i = 0; i < generator_threads; ++i) {
        if (generator_pthreads[i] != 0) {
            pthread_join(generator_pthreads[i], nullptr);
        }

        delete generator_thread_args[i];
        generator_thread_args[i] = nullptr;
    }

    log_queue.Shutdown();
    for (int i = 0; i < analyzer_threads; ++i) {
        if (analyzer_pthreads[i] != 0) {
            pthread_join(analyzer_pthreads[i], nullptr);
        }
        delete analyzer_thread_args[i];
        analyzer_thread_args[i] = nullptr;
    }

    if (!creation_failed) {
        try {
            std::map<std::string, IpStats> final_stats = statisticIn(analyzer_data_store);
            fprintf(stdout, "\n--- Final Aggregated Statistics ---\n");
            if (final_stats.empty()) { fprintf(stdout, "(No statistics collected)\n"); }
            else {
                for (const auto& pair : final_stats) {
                    fprintf(stdout, "IP Address: %s\n", pair.first.c_str());
                    std::cout << pair.second.toString();
                    fprintf(stdout, "---------------------------------\n");
                }
            }
            fflush(stdout);
        } catch (const std::exception& e) {
            main_ret_code = 1;
        }
    } else {
        fprintf(stdout, "\n--- Final Aggregated Statistics Skipped (due to thread creation failure) ---\n");
    }


    pthread_mutex_destroy(&shutdown_mutex);

    return main_ret_code;
}