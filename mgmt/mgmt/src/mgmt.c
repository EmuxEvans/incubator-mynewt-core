/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#include <os/os.h>

#include "mgmt/mgmt.h"

static struct os_mutex mgmt_group_lock;
static STAILQ_HEAD(, mgmt_group) mgmt_group_list =
    STAILQ_HEAD_INITIALIZER(mgmt_group_list);

static int
mgmt_group_list_lock(void)
{
    int rc;

    if (!os_started()) {
        return (0);
    }

    rc = os_mutex_pend(&mgmt_group_lock, OS_WAIT_FOREVER);
    if (rc != 0) {
        goto err;
    }

    return (0);
err:
    return (rc);
}

int
mgmt_group_list_unlock(void)
{
    int rc;

    if (!os_started()) {
        return (0);
    }

    rc = os_mutex_release(&mgmt_group_lock);
    if (rc != 0) {
        goto err;
    }

    return (0);
err:
    return (rc);
}

int
mgmt_group_register(struct mgmt_group *group)
{
    int rc;

    rc = mgmt_group_list_lock();
    if (rc != 0) {
        goto err;
    }

    STAILQ_INSERT_TAIL(&mgmt_group_list, group, mg_next);

    rc = mgmt_group_list_unlock();
    if (rc != 0) {
        goto err;
    }

    return (0);
err:
    return (rc);
}

static struct mgmt_group *
mgmt_find_group(uint16_t group_id)
{
    struct mgmt_group *group;
    int rc;

    group = NULL;

    rc = mgmt_group_list_lock();
    if (rc != 0) {
        goto err;
    }

    STAILQ_FOREACH(group, &mgmt_group_list, mg_next) {
        if (group->mg_group_id == group_id) {
            break;
        }
    }

    rc = mgmt_group_list_unlock();
    if (rc != 0) {
        goto err;
    }

    return (group);
err:
    return (NULL);
}

struct mgmt_handler *
mgmt_find_handler(uint16_t group_id, uint16_t handler_id)
{
    struct mgmt_group *group;
    struct mgmt_handler *handler;

    group = mgmt_find_group(group_id);
    if (!group) {
        goto err;
    }

    if (handler_id >= group->mg_handlers_count) {
        goto err;
    }

    handler = &group->mg_handlers[handler_id];

    return (handler);
err:
    return (NULL);
}

void
mgmt_jbuf_setoerr(struct mgmt_jbuf *njb, int errcode)
{
    struct json_value jv;

    json_encode_object_start(&njb->mjb_enc);
    JSON_VALUE_INT(&jv, errcode);
    json_encode_object_entry(&njb->mjb_enc, "rc", &jv);
    json_encode_object_finish(&njb->mjb_enc);
}
