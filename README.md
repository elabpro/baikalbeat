# baikalbeat
Kafkabeat on C++

## Options

```text
  -h [ --help ]                         produce this help message
  -b [ --brokers ] arg                  the kafka broker list
  -t [ --topic ] arg                    the topic from which to fetch records
  -g [ --group-id ] arg                 the consumer group id
  -c [ --client-id ] arg                the kafka client id (default, 
                                        baikalbeat)
  -e [ --elasticsearch-url ] arg        elasticsearch URL (default, 
                                        http://localhost:9200/)
  -i [ --elasticsearch-index ] arg      elasticsearch index name (default, 
                                        xx_index_alias field)
  -p [ --elasticsearch-index-prefix ] arg
                                        prefix for elasticsearch index name 
                                        (ie, test-)
  -m [ --bulk-size ] arg                bulkSize for Elasticsearch batches 
                                        (default, 10000)
  -s [ --bulk-delay ] arg               delay (in microseconds) after bulk for 
                                        Elasticsearch (default, 500)
  -n [ --threads ] arg                  number of threads for parsing (default,
                                        5)
  -d [ --debug ] arg                    debug (default, 0 - no debug)
  --dry-run arg                         dry run for Kafka without ES (default, 
                                        0 - no dry)
```