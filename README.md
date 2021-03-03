# baikalbeat
Kafkabeat on C++

## Options
--------
-h [ --help ]                    produce this help message<br>
-b [ --brokers ] arg             the kafka broker list<br>
-t [ --topic ] arg               the topic in which to write to<br>
-g [ --group-id ] arg            the consumer group id<br>
-e [ --elasticsearch-url ] arg   elasticsearch URL (default, http://localhost:9200/)<br>
-i [ --elasticsearch-index ] arg elasticsearch index name (default, test-kafkabeat-cpp)<br>
-m [ --bulk-size ] arg           bulkSize for Elasticsearch batches (default, 10000)<br>
-n [ --threads ] arg             number of threads for parsing (default, 5)<br>
-d [ --debug ] arg               debug (default, 0 - no debug)<br>
