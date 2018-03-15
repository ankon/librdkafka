/*
 * librdkafka - Apache Kafka C library
 *
 * Copyright (c) 2018 Magnus Edenhill
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RDKAFKA_ADMIN_H_
#define _RDKAFKA_ADMIN_H_


/**
 * @brief Configuration value type
 */
typedef enum rd_kafka_confval_type_t {
        RD_KAFKA_CONFVAL_I16,
        RD_KAFKA_CONFVAL_I32,
        RD_KAFKA_CONFVAL_STR,
        RD_KAFKA_CONFVAL_BOOL,
} rd_kafka_confval_type_t;

/**
 * @brief Configuration value (used by AdminOption).
 *        Comes with a type, backed by a union, and a flag to indicate
 *        if the value has been set or not.
 */
struct rd_kafka_confval {
        rd_kafka_confval_type_t valtype; /**< Value type, maps to union below.*/
        int is_set;                      /**< Value has been set. */
        union {
                int16_t I16;
                int32_t I32;
                char   *STR;
                int     BOOL;
        } u;
};



/**
 * @brief Topic [ + Error code + Error string ]
 *
 * Auxiliary type.
 */
struct rd_kafka_aux_topic_err {
        const char *topic;
        rd_kafka_resp_err_t err;
        const char *errstr;
};


/**
 * @brief Common AdminOption type used for all admin APIs.
 */
struct rd_kafka_AdminOption_s {
        /* Generic */
        rd_kafka_confval_t timeout;        /** I32: Full request timeout,
                                            *       includes looking up leader
                                            *       broker,
                                            *       waiting for req/response,
                                            *       etc. */

        /* Specific for one or more APIs */
        rd_kafka_confval_t broker_timeout; /**< I32: Timeout on broker.
                                            *   Valid for: CreateTopics */
        rd_kafka_confval_t validate_only;  /**< BOOL: Only validate (on broker),
                                            *   but don't perform action.
                                            *   Valid for: CreateTopics */
};


/**
 * @name CreateTopics
 * @{
 */

/**
 * @brief CreateTopics result
 */
struct rd_kafka_CreateTopics_result_s {
        struct rd_kafka_int_topic_err *topics;
        size_t topic_cnt;
};

/**
 * @brief NewTopic type, used with CreateTopics.
 */
struct rd_kafka_NewTopic_s {
        /* Required */
        const char *topic;      /**< Topic to be created */
        int num_partitions;     /**< Number of partitions to create */
        int replication_factor; /**< Replication factor */

        /* Optional */
        rd_list_t *replicas;    /**< Type (int):
                                 *   Array of replica lists, capped
                                 *   by num_partitions */
        rd_list_t config;       /**< Type (rd_strtup_t *):
                                 *   List of configuration key-value strings */
};

RD_EXPORT
void rd_kafka_admin_CreateTopics (rd_kafka_t *rk,
                                  const rd_kafka_NewTopic_t *new_topics,
                                  size_t new_topic_cnt,
                                  const rd_kafka_admin_option_t *options,
                                  rd_kafka_queue_t *rkqu);

/**@}*/


/**
 * @name DeleteTopics
 * @{
 */

/**
 * @brief DeleteTopics result
 */
struct rd_kafka_DeleteTopics_result_s {
        struct rd_kafka_int_topic_err *topics;  /* without error string */
        size_t topic_cnt;
};

RD_EXPORT
void rd_kafka_admin_DeleteTopics (rd_kafka_t *rk,
                                  const char *topics,
                                  size_t topic_cnt,
                                  const rd_kafka_admin_option_t *options,
                                  rd_kafka_queue_t *rkqu);


/**@}*/


/**
 * @name AlterConfigs
 * @{
 */

/**
 * @brief DescribeCluster result
 */
struct rd_kafka_DescribeCluster_result_s {
        struct rd_kafka_int_topic_err *topics;  /* without error string */
        size_t topic_cnt;
};



/**
 * @brief A cluster ConfigResource constisting of:
 *         - resource type (BROKER, TOPIC)
 *         - configuration property name
 *         - configuration property value
 *
 * https://cwiki.apache.org/confluence/display/KAFKA/KIP-133%3A+Describe+and+Alter+Configs+Admin+APIs
 */
struct rd_kafka_ConfigResource_s {
        rd_kafka_ConfigResourceType_t type;  /**< Resource type */
        const char *name;                    /**< Resource name */
}

RD_EXPORT
void rd_kafka_admin_AlterConfigs (rd_kafka_t *rk,
                                  const rd_kafka_ConfigResource_t *configs,
                                  size_t config_cnt,
                                  const rd_kafka_admin_option_t *options,
                                  rd_kafka_queue_t *rkqu);


/**@}*/

#endif /* _RDKAFKA_ADMIN_H_ */
