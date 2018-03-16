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

#ifndef _RDKAFKA_AUX_H_
#define _RDKAFKA_AUX_H_

/**
 * @name Auxiliary types
 */

#include "rdkafka_conf.h"



/**
 * @brief Topic [ + Error code + Error string ]
 *
 * @remark Public type.
 * @remark Single allocation.
 */
struct rd_kafka_topic_result_s {
        char *topic;             /**< Points to data */
        rd_kafka_resp_err_t err; /**< Error code */
        char *errstr;            /**< Points to data after topic, unless NULL */
        char  data[1];           /**< topic followed by errstr */
};

#define rd_kafka_topic_result_destroy(terr) rd_free(terr)
rd_kafka_topic_result_t *
rd_kafka_topic_result_new (const char *topic, rd_kafka_resp_err_t err,
                           const char *errstr);


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



static rd_kafka_resp_err_t
rd_kafka_AdminOption_set_type (rd_kafka_AdminOption_t *options,
                               const char *name,
                               rd_kafka_confval_type_t valuetype,
                               const void *valuep,
                               char *errstr, size_t errstr_size) {
        rd_kafka_confval_t *confval;

        if (!strcmp(name, "timeout"))
                confval = &options->timeout;
        else if (!strcmp(name, "broker.timeout"))
                confval = &options->broker_timeout;
        else {
                rd_snprintf(errstr, errstr_size,
                            "Unknown admin option \"%s\"", name);
                return RD_KAFKA_RESP_ERR__INVALID_ARG;
        }

        return rd_kafka_confval_set_type(confval, name, valuetype, value,
                                         errstr, errstr_size);
}

RD_EXPORT rd_kafka_resp_err_t
rd_kafka_AdminOption_set_int32 (rd_kafka_AdminOption_t *options,
                                const char *name, int32_t value,
                              char *errstr, size_t errstr_size) {
        int intvalue = (int)value;
        return rd_kafka_AdminOption_set_any(options, name,
                                            RD_KAFKA_CONFVAL_INT, &intvalue,
                                            errstr, errstr_size);
}

RD_EXPORT rd_kafka_resp_err_t
rd_kafka_AdminOption_set_str (rd_kafka_AdminOption_t *options,
                              const char *name, const char *value,
                              char *errstr, size_t errstr_size) {
        int intvalue = (int)value;
        return rd_kafka_AdminOption_set_any(options, name,
                                            RD_KAFKA_CONFVAL_STR, value,
                                            errstr, errstr_size);
}


/**
 * @name CreateTopics
 * @{
 */

struct rd_kafka_CreateTopics_topic_result_s {
        rd_kafka_aux_topic_err terr;
};

/**
 * @brief CreateTopics result
 */
struct rd_kafka_CreateTopics_result_s {
        rd_list_t topics;   /**< Type (struct rd_kafka_aux_topic_err *) */
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
        rd_list_t *replicas;    /**< Type (rd_list_t (int32_t)):
                                 *   Array of replica lists, capped
                                  *   by num_partitions */
        rd_list_t config;       /**< Type (rd_strtup_t *):
                                 *   List of configuration key-value strings */
};

/**@}*/


/**
 * @name DeleteTopics
 * @{
 */

/**
 * @brief DeleteTopics result
 */
struct rd_kafka_DeleteTopics_result_s {
        rd_list_t topics;   /**< Type (rd_kafka_aux_topic_err *) */
};

struct rd_kafka_DeleteTopic_s {
        char *topic;   /**< Points to data */
        char data[1];  /**< The topic name is allocated along with the
                        *   the struct here. */
};

RD_EXPORT
void rd_kafka_admin_DeleteTopics (rd_kafka_t *rk,
                                  const rd_kafka_DeleteTopic_t *topics,
                                  size_t topic_cnt,
                                  const rd_kafka_admin_option_t *options,
                                  rd_kafka_queue_t *rkqu);


/**@}*/


/**
 * @name AlterConfigs
 * @{
 */


struct rd_kafka_ConfigEntry_s {
        rd_strtup_t kv;           /**< Name/Value pair */

        /* Response */
        bool readonly;            /**< Value is read-only (on broker) */
        bool is_default;          /**< Value is at its default */
        bool is_sensitive;        /**< Value is sensitive */
};

/**
 * @brief A cluster ConfigResource constisting of:
 *         - resource type (BROKER, TOPIC)
 *         - configuration property name
 *         - configuration property value
 *
 * https://cwiki.apache.org/confluence/display/KAFKA/KIP-133%3A+Describe+and+Alter+Configs+Admin+APIs
 */
struct rd_kafka_ConfigResource {
        rd_kafka_ConfigResourceType_t type;/**< Resource type */
        const char *name;                  /**< Resource name */
        rd_list_t config;                  /**< Type (rd_kafka_ConfigEntry_t *):
                                            *   List of config props */

        /* Response */
        rd_kafka_resp_err_t err;           /**< Response error code */
        char *errstr;                      /**< Response error string */
};

struct rd_kafka_ConfigList_s {
        rd_list_t resources;  /**< Type (struct rd_kafka_ConfigResource *):
                               *   List of config resources */
};

struct rd_kafka_AlterConfigs_result_s {
        rd_list_t configs;    /**< Type (rd_kafka_ConfigList_t) */
};

/**@}*/



/**
 * @name DescribeConfigs
 * @{
 */

struct rd_kafka_DescribeConfigs_result_s {
        rd_list_t configs;    /**< Type (rd_kafka_ConfigList_t) */
};

/**@}*/


/**
 * @name CreatePartitions
 * @{
 */

struct rd_kafka_NewPartitions_s {
        char     *topic;         /**< Topic for which to add partition(s) */
        int       total_count;   /**< New total partition count */
        rd_list_t assignments;   /**< Type (rd_list_t (int32_t)):
                                  *   New assignment (broker-id) list per
                                  *   new partition */

        /* Response */
        rd_kafka_resp_err_t err; /**< Response error code */
        char *errstr;            /**< Response error string */
};

struct rd_kafka_CreatePartitions_result_s {
        rd_list_t partitions;   /**< Type (rd_kafka_NewPartitions_t) */

};


/**@}*/

#endif /* _RDKAFKA_AUX_H_ */
