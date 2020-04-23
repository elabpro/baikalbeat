# baikalbeat
Kafkabeat on C++

## Options
-h [ --help ]                    produce this help message
-b [ --brokers ] arg             the kafka broker list
-t [ --topic ] arg               the topic in which to write to
-g [ --group-id ] arg            the consumer group id
-e [ --elasticsearch-url ] arg   elasticsearch URL (default, http://localhost:9200/)
-i [ --elasticsearch-index ] arg elasticsearch index name (default, test-kafkabeat-cpp)
-m [ --bulk-size ] arg           bulkSize for Elasticsearch batches (default, 10000)
-n [ --threads ] arg             number of threads for parsing (default, 5)
-d [ --debug ] arg               debug (default, 0 - no debu