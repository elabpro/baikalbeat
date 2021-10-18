/**
 * \file
 * -----------------------------------------------------------------------------
 * Baikalbeat
 * Beat for reading messages from Kafka and to ingest them in ElasticSearch
 * 
 * author: Eugene Arbatsky, DataOps
 * date  : 2020-04-22
 * -----------------------------------------------------------------------------
 */

#include <string>
#include <vector>
#include <iostream>
#include <map>

#include <cpr/response.h>
#include <cppkafka/cppkafka.h>
#include <cppkafka/consumer.h>
#include <cppkafka/configuration.h>
#include <sys/time.h>
#include <unistd.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <boost/program_options.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "HTTPRequest.hpp"

using std::cout;
using std::endl;
using std::exception;
using std::string;

using cppkafka::Configuration;
using cppkafka::Consumer;
using cppkafka::Message;
using cppkafka::TopicPartitionList;

using namespace rapidjson;

namespace po = boost::program_options;

bool running = true;

string brokers = "127.0.0.1:9092";
string topic_name = "";
string group_id = "test";
string client_id = "baikalbeat";
string es_url = "http://localhost:9200/";
string es_index = "";
string es_index_prefix = "";
string version = "1.9";
string config = "";

int bulkSize = 10000;
int debugLevel = 0;
int maxThreads = 5;
int bulkDelay = 500; // in nanosecs
int dryRun = 0;

/// Very simple log callback (only print message to stdout)

/**
 * The main thread
 */
void BaikalbeatThread(int threadNumber) {
    struct timeval time;
    std::string indexKey = "xx_index_alias";
    const char* delimiter = "\"";
    int indexKeySize = indexKey.length();
    int mode = 0; // 0 - index is defined as variable, 1 - is taken from field
    long lastCommit = ((unsigned long long) time.tv_sec * 1000000) + time.tv_usec;
    std::string bulkBody = "";
    bulkBody[0] = 0x00;
    if (es_index.length() == 0) {
        mode = 1;
    }

    // Construct the configuration
    Configuration config = {
        {"metadata.broker.list", brokers},
        {"group.id", group_id},
        {"client.id", client_id + "-" + version},
        {"enable.auto.commit", true},
        {"auto.commit.interval.ms", 1000}
    };
    Consumer consumer(config);
    // Print the assigned partitions on assignment
    consumer.set_assignment_callback([](const TopicPartitionList & partitions) {
        cout << "Got assigned: " << partitions << endl;
    });

    // Print the revoked partitions on revocation
    consumer.set_revocation_callback([](const TopicPartitionList & partitions) {
        cout << "Got revoked: " << partitions << endl;
    });

    // Subscribe to the topic
    consumer.subscribe({topic_name});
    std::cout << "Topic " << topic_name << " is subscribed" << std::endl;

    int bulkIndex = 0;

    while (running) {
        // Try to consume a message
        Message msg = consumer.poll(std::chrono::milliseconds(100));
        if (msg) {
            // If we managed to get a message
            if (msg.get_error()) {
                // Ignore EOF notifications from rdkafka
                if (!msg.is_eof()) {
                    cout << "[+] Received error notification: " << msg.get_error() << endl;
                }
            } else {
                bulkIndex++;
                Document d;
                std::string s = msg.get_payload();
                std::string indexName = "";
                gettimeofday(&time, NULL);
                long recMicrosec = ((unsigned long long) time.tv_sec * 1000000) + time.tv_usec;
                if (dryRun == 0) {
                    if (debugLevel) {
                        std::cout << s << std::endl;
                    }
                    if (mode == 1) {
                        const char* s_str = s.c_str();
                        const char* pos1 = strstr(s_str, indexKey.c_str());
                        if (pos1 != NULL) {
                            const char* pos2 = strstr((char*) (pos1 + indexKeySize + 3), delimiter);
                            if (pos2 != NULL) {
                                size_t len = (size_t) (pos2 - pos1 - indexKeySize - 3);
                                char *result = (char*) malloc(len + 1);
                                if (result) {
                                    memcpy(result, (char*) (pos1 + indexKeySize + 3), len);
                                    result[len] = '\0';
                                    indexName = result;
                                    free(result);
                                } else {
                                    if (debugLevel) {
                                        std::cout << "ERROR: Can't get memory for a message" << std::endl;
                                    }
                                }
                            }
                        }
                        if (indexName.length() == 0) {
                            // Skip this message
                            continue;
                        }
                    } else {
                        indexName = es_index;
                    }
                    indexName = es_index_prefix + indexName;
                    bulkBody += "{\"index\":{\"_index\":\"" + indexName + "\"}}\n" + s + "\n";
                    if (bulkIndex > bulkSize || recMicrosec - lastCommit > 50000000) {
                        try {
                            while (1) {
                                http::Request request(es_url);
                                const auto response = request.send("POST", bulkBody,{"Content-type: application/json"});
                                if (response.status != 200) {
                                    std::cout << "ERROR: bulk wasn't written\n" << response.body.data() << std::endl;
                                } else {
                                    break;
                                }
                                usleep(bulkDelay * 10);
                            }
                            //                            std::cout << response.body.data() << std::endl;
                        } catch (const std::exception& e) {
                            std::cout << "ERROR:" << e.what() << std::endl;
                        }
                        bulkBody = "";
                        usleep(bulkDelay);
                        lastCommit = recMicrosec;
                        bulkIndex = 0;
                    }
                }
            }
        } else {
            gettimeofday(&time, NULL);
            long recMicrosec = ((unsigned long long) time.tv_sec * 1000000) + time.tv_usec;
            if (dryRun == 0 && recMicrosec - lastCommit > 500000 && bulkBody.length() > 0) {
                try {
                    http::Request request(es_url);
                    const auto response = request.send("POST", bulkBody,{"Content-type: application/json"});
                    std::cout << ":";
                } catch (const std::exception& e) {
                    std::cout << "ERROR:" << e.what() << std::endl;
                }
                bulkBody = "";
                lastCommit = recMicrosec;
            }
        }
    }
}

/**
 * Command line processor, options, configuration. Run threads
 */
int main(int argc, char* argv[]) {
    cout << "Baikalbeat (Kafka -> Elasticsearch beat, developed on C++), version " << version << endl;
    cout << "LibertyGlobal, DataOps, Eugene Arbatsky (c) 2020-2021" << endl;
    cout << endl;

    po::options_description options("Options");
    options.add_options()
            ("help,h", "produce this help message")
            ("config", po::value<std::string>(), "Config file")
            ("brokers,b", po::value<string>(&brokers), "the kafka broker list")
            ("topic,t", po::value<string>(&topic_name), "the topic from which to fetch records")
            ("group-id,g", po::value<string>(&group_id), "the consumer group id")
            ("client-id,c", po::value<string>(&client_id), "the kafka client id (default, baikalbeat)")
            ("elasticsearch-url,e", po::value<string>(&es_url), "elasticsearch URL (default, http://localhost:9200/)")
            ("elasticsearch-index,i", po::value<string>(&es_index), "elasticsearch index name (default, xx_index_alias field)")
            ("elasticsearch-index-prefix,p", po::value<string>(&es_index_prefix), "prefix for elasticsearch index name (ie, test-)")
            ("bulk-size,m", po::value<int>(&bulkSize), "bulkSize for Elasticsearch batches (default, 10000)")
            ("bulk-delay,s", po::value<int>(&bulkDelay), "delay (in microseconds) after bulk for Elasticsearch (default, 500)")
            ("threads,n", po::value<int>(&maxThreads), "number of threads for parsing (default, 5)")
            ("debug,d", po::value<int>(&debugLevel), "debug (default, 0 - no debug)")
            ("dry-run", po::value<int>(&dryRun), "dry run for Kafka without ES (default, 0 - no dry)")
            ;
    po::options_description fileOptions{"File"};
    fileOptions.add_options()
            ("brokers", po::value<string>(&brokers), "the kafka broker list")
            ("topic", po::value<string>(&topic_name), "the topic from which to fetch records")
            ("group-id", po::value<string>(&group_id), "the consumer group id")
            ("client-id", po::value<string>(&client_id), "the kafka client id (default, baikalbeat)")
            ("elasticsearch-url", po::value<string>(&es_url), "elasticsearch URL (default, http://localhost:9200/)")
            ("elasticsearch-index", po::value<string>(&es_index), "elasticsearch index name (default, xx_index_alias field)")
            ("elasticsearch-index-prefix", po::value<string>(&es_index_prefix), "prefix for elasticsearch index name (ie, test-)")
            ("bulk-size", po::value<int>(&bulkSize), "bulkSize for Elasticsearch batches (default, 10000)")
            ("bulk-delay", po::value<int>(&bulkDelay), "delay (in microseconds) after bulk for Elasticsearch (default, 500)")
            ("threads", po::value<int>(&maxThreads), "number of threads for parsing (default, 5)")
            ("debug", po::value<int>(&debugLevel), "debug (default, 0 - no debug)")
            ;
    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).options(options).run(), vm);
        if (vm.count("config")) {
            std::ifstream ifs{vm["config"].as<std::string>().c_str()};
            if (ifs)
                store(po::parse_config_file(ifs, fileOptions), vm);
        }
        po::notify(vm);
    } catch (exception &ex) {
        cout << "Error parsing options: " << ex.what() << endl;
        cout << endl;
        cout << options << endl;
        return 1;
    }
    if (topic_name.length() == 0 || es_url.length() == 0) {
        std::cout << "ERROR: No enogh variables!" << std::endl;
        return 1;
    }

    es_url += "/_bulk";

    /*
     * Create an asio::io_service and a thread_group (through pool in essence)
     */
    boost::asio::io_service ioService;
    boost::thread_group threadpool;


    /*
     * This will start the ioService processing loop. All tasks 
     * assigned with ioService.post() will start executing. 
     */
    boost::asio::io_service::work work(ioService);

    for (int i = 0; i < maxThreads; i++) {
        threadpool.create_thread(
                boost::bind(BaikalbeatThread, i)
                );
    }


    /*
     * Will wait till all the threads in the thread pool are finished with 
     * their assigned tasks and 'join' them. Just assume the threads inside
     * the threadpool will be destroyed by this method.
     */
    threadpool.join_all();

    /*
     * This will stop the ioService processing loop. Any tasks
     * you add behind this point will not execute.
     */
    ioService.stop();

    return 0;
}

