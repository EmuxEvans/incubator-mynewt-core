/*
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
#include "test_cborattr.h"

/*
 * Simple decoding. Only have key for one of the key/value pairs.
 */
TEST_CASE(test_cborattr_decode_partial)
{
    const uint8_t *data;
    int len;
    int rc;
    char test_str_b[4] = { '\0' };
    struct cbor_attr_t test_attrs[] = {
        [0] = {
            .attribute = "b",
            .type = CborAttrTextStringType,
            .addr.string = test_str_b,
            .len = sizeof(test_str_b),
            .nodefault = true
        },
        [1] = {
            .attribute = NULL
        }
    };

    data = test_str1(&len);
    rc = cbor_read_flat_attrs(data, len, test_attrs);
    TEST_ASSERT(rc == 0);
    TEST_ASSERT(!strcmp(test_str_b, "B"));
}
