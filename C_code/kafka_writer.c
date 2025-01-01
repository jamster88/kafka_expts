#include <stdio.h>
#include <librdkafka/rdkafka.h>
#include <string.h>

int main(int argc, char **argv)
{
	// Producer instance
	rd_kafka_t *rk;

	// Producer config
	rd_kafka_conf_t *conf;

	// error buffer
	char errbuff[1024];

	// broker
	char *brokers = "localhost:9092";

	// topic
	char *topic = "ctest_topic";

	// new kafka conf obj
	conf = rd_kafka_conf_new();

	// set bootstrap srvs
	if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errbuff, sizeof(errbuff)) != RD_KAFKA_CONF_OK) 
	{
		fprintf(stderr, "%% Config error: %s\n", errbuff);
		return 1;
	}

	// create producer
	rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errbuff, sizeof(errbuff));
	if (!rk)
	{
		fprintf(stderr, "%% Failed to create Kafka Producer: %s\n", errbuff);
		return 1;
	}

	// make messages
	char *message = "this is a test in C";
	size_t len = strlen(message);
	if (rd_kafka_producev
		(
		rk,
		RD_KAFKA_V_TOPIC(topic),
                RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                RD_KAFKA_V_VALUE(message, len),
                RD_KAFKA_V_END
		) == -1) 
	{
		fprintf(stderr, "%% Failed to produce to topic %s: %s\n",
				rd_kafka_topic_name(rd_kafka_topic_new(rk, topic, NULL)),
		                rd_kafka_err2str(rd_kafka_last_error()));
		rd_kafka_destroy(rk);
		return 1;
	}

	fprintf(stderr, "%% Message queued (%zd bytes)\n", len);

	// check for delivery reports
	rd_kafka_poll(rk, 30);

	// destroy producer
	rd_kafka_destroy(rk);

	return 0;
}
