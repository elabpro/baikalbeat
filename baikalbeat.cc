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

#include <cpr/response.h>
#include <elasticlient/client.h>
#include <cppkafka/cppkafka.h>
#include <cppkafka/consumer.h>
#include <cppkafka/configuration.h>
#include <elasticlient/client.h>
#include <elasticlient/bulk.h>
#include <elasticlient/logging.h>
#include <sys/time.h>
#include <unistd.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <boost/program_options.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

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

string brokers;
string topic_name;
string group_id;
string es_url = "http://localhost:9200/";
string es_index = "test-kafkabeat-cpp";
int bulkSize = 10000;
int debugLevel = 0;
int maxThreads = 5;

/// Very simple log callback (only print message to stdout)
void logCallback(elasticlient::LogLevel logLevel, const std::string &msg) {
    if (logLevel != elasticlient::LogLevel::DEBUG) {
        std::cout << "LOG " << (unsigned) logLevel << ": " << msg << std::endl;
    }
}

void BaikalbeatThread(int threadNumber){
    struct timeval time;
    // Construct the configuration
    Configuration config = {
        {"metadata.broker.list", brokers},
        {"group.id", group_id},
        // Disable auto commit
        {"enable.auto.commit", false}};
    // Create the consumer
    Consumer consumer(config);
    // Print the assigned partitions on assignment
    consumer.set_assignment_callback([](const TopicPartitionList &partitions) {
        cout << "Got assigned: " << partitions << endl;
    });

    // Print the revoked partitions on revocation
    consumer.set_revocation_callback([](const TopicPartitionList &partitions) {
        cout << "Got revoked: " << partitions << endl;
    });

    // Subscribe to the topic
    consumer.subscribe({topic_name});
    std::shared_ptr<elasticlient::Client> client = std::make_shared<elasticlient::Client>(
        std::vector<std::string>({es_url})); // last / is mandatory

    int bulkIndex = 0;
    if(debugLevel){
        elasticlient::setLogFunction(logCallback);
    }
    elasticlient::Bulk bulkIndexer(client);
    elasticlient::SameIndexBulkData bulk(es_index, 100);

    while (running)
    {
        // Try to consume a message
        Message msg = consumer.poll();
        if (msg)
        {
            // If we managed to get a message
            if (msg.get_error())
            {
                // Ignore EOF notifications from rdkafka
                if (!msg.is_eof())
                {
                    cout << "[+] Received error notification: " << msg.get_error() << endl;
                }
            }
            else
            {
                bulkIndex++;
                Document d;
                std::string s = msg.get_payload();
                d.Parse(s.c_str());
                // Print the key (if any)
                if (msg.get_key())
                {
                    //                    cout << msg.get_key() << " -> ";
                }
                // Print the payload
                //                cout << msg.get_payload() << endl;
                // cpr::Response indexResponse = client.index("kafkabeat",
                // "docType", msg.get_key(), msg.get_payload());
                gettimeofday(&time, NULL);
                long microsec = ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;
                bulk.indexDocument("docType",
                                   std::to_string(microsec) + std::to_string(bulkIndex), msg.get_payload());
                // Now commit the message
                //if(indexResponse.status_code >= 200 &&
                //indexResponse.status_code <= 300){
                //}
                if (bulkIndex >= bulkSize)
                {
                    size_t errors = bulkIndexer.perform(bulk);
                    if(debugLevel){
                    std::cout << "When indexing " << bulk.size() << " documents, "
                              << errors << " errors occured" << std::endl;
                    }
                    if (errors > 0){
                        // retry
                        usleep(100);
                        size_t errors = bulkIndexer.perform(bulk);
                        if(debugLevel){
                        std::cout << "When indexing " << bulk.size() << " documents, "
                              << errors << " errors occured" << std::endl;
                        }
                    }
                    consumer.commit(msg);
                    bulk.clear();
                    bulkIndex = 0;
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    cout << "Baikalbeat (Kafka -> Elasticsearch beat, developed on C++), version 0.9beta+threads" << endl;
    cout << "LibertyGlobal, DataOps, Eugene Arbatsky (c) 2020" << endl;
    cout << endl;

    po::options_description options("Options");
    options.add_options()
        ("help,h", "produce this help message")
        ("brokers,b", po::value<string>(&brokers)->required(),"the kafka broker list")
        ("topic,t", po::value<string>(&topic_name)->required(),"the topic in which to write to")
        ("group-id,g", po::value<string>(&group_id)->required(),"the consumer group id")
        ("elasticsearch-url,e", po::value<string>(&es_url),"elasticsearch URL (default, http://localhost:9200/)")
        ("elasticsearch-index,i", po::value<string>(&es_index),"elasticsearch index name (default, test-kafkabeat-cpp)")
        ("bulk-size,m", po::value<int>(&bulkSize),"bulkSize for Elasticsearch batches (default, 10000)")
        ("threads,n", po::value<int>(&maxThreads),"number of threads for parsing (default, 5)")
        ("debug,d", po::value<int>(&debugLevel),"debug (default, 0 - no debug)")
        ;

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).options(options).run(), vm);
        po::notify(vm);
    }
    catch (exception &ex)
    {
        cout << "Error parsing options: " << ex.what() << endl;
        cout << endl;
        cout << options << endl;
        return 1;
    }

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

    for(int i=0;i<maxThreads;i++){
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

