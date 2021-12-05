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

#ifndef _OS_MUTEX_H_
#define _OS_MUTEX_H_

#include "os/os.h"
//#include "os/queue.h"
#include "rtos.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
struct os_mutex
{
    SLIST_HEAD(, os_task) mu_head;  /* chain of waiting tasks */
    uint8_t     _pad;
    uint8_t     mu_prio;            /* owner's default priority*/
    uint16_t    mu_level;           /* call nesting level */
    struct os_task *mu_owner;       /* owners task */
};
#endif

/* 
  XXX: NOTES
    -> Should we add a magic number or flag to the mutex structure so
    that we know that this is a mutex? We can use it for double checking
    that a proper mutex was passed in to the API.
    -> What debug information should we add to this structure? Who last
    acquired the mutex? File/line where it was released?
    -> Should we add a name to the mutex?
    -> Should we add a "os_mutex_inspect() api?
*/

/* XXX: api to create
os_mutex_inspect(); 
*/ 

struct os_mutex
{
  SemaphoreHandle_t sem;
};

/* Initialize a mutex */
static inline os_error_t os_mutex_init(struct os_mutex *mu)
{
  mu->sem = xSemaphoreCreateMutex();

  return (mu->sem != NULL) ? 0 : 1;
}


/* Release a mutex */
static inline os_error_t os_mutex_release(struct os_mutex *mu)
{
  return xSemaphoreGive(mu->sem) ? 0 : 1;
}

/* Pend (wait) for a mutex */
static inline os_error_t os_mutex_pend(struct os_mutex *mu, uint32_t timeout)
{
  return xSemaphoreTake(mu->sem, timeout) ? 0 : 1;
}

#ifdef __cplusplus
}
#endif

#endif  /* _OS_MUTEX_H_ */
