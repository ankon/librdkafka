#include "rdkafka_int.h"
#include "rdkafka_admin.h"




rd_kafka_resp_err_t
rd_kafka_AdminOptions_set_timeout (rd_kafka_AdminOptions_t *options,
                                  int timeout_ms,
                                  char *errstr, size_t errstr_size) {
        return rd_kafka_confval_set_type(&options->timeout,
                                         RD_KAFKA_CONFVAL_INT, &timeout_ms,
                                         errstr, errstr_size);
}


rd_kafka_resp_err_t
rd_kafka_AdminOptions_set_validate_only (rd_kafka_AdminOptions_t *options,
                                        int true_or_false,
                                        char *errstr, size_t errstr_size) {
        return rd_kafka_confval_set_type(&options->validate_only,
                                         RD_KAFKA_CONFVAL_INT, &true_or_false,
                                         errstr, errstr_size);
}



rd_kafka_AdminOptions_t *rd_kafka_AdminOptions_new (void) {
        rd_kafka_AdminOptions_t *options;

        options = rd_calloc(1, sizeof(*options));

        rd_kafka_confval_init_int(&options->timeout, "timeout",
                                  -1, 3600*1000, 0);
        rd_kafka_confval_init_int(&options->operation_timeout,
                                  "operation_timeout",
                                  -1, 3600*1000, 0);
        rd_kafka_confval_init_int(&options->validate_only, "validate_only",
                                  0, 1, 0);

        return options;
}

void rd_kafka_AdminOptions_destroy (rd_kafka_AdminOptions_t *options) {
        rd_free(options);
}


