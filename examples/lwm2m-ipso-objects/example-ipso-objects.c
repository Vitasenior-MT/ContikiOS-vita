/*
 * Copyright (c) 2015, Yanzi Networks AB.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *      OMA LWM2M and IPSO Objects example.
 * \author
 *      Joakim Eriksson, joakime@sics.se
 *      Niclas Finne, nfi@sics.se
 */

#include "contiki.h"
#include "dev/leds.h"
#include "services/lwm2m/lwm2m-engine.h"
#include "services/lwm2m/lwm2m-rd-client.h"
#include "services/lwm2m/lwm2m-device.h"
#include "services/lwm2m/lwm2m-server.h"
#include "services/lwm2m/lwm2m-security.h"
#include "services/ipso-objects/ipso-objects.h"
#include "services/ipso-objects/ipso-sensor-template.h"
#include "services/ipso-objects/ipso-control-template.h"
#include "dev/leds.h"

#define DEBUG DEBUG_PRINT //DEBUG_NONE
#include "net/ipv6/uip-debug.h"

#ifndef REGISTER_WITH_LWM2M_BOOTSTRAP_SERVER
#define REGISTER_WITH_LWM2M_BOOTSTRAP_SERVER 1
#endif

#ifndef REGISTER_WITH_LWM2M_SERVER
#define REGISTER_WITH_LWM2M_SERVER 1
#endif

#ifndef LWM2M_SERVER_ADDRESS
#define LWM2M_SERVER_ADDRESS "coaps://[fd00::1]:5685"
#endif


PROCESS(example_ipso_objects, "IPSO object example");
AUTOSTART_PROCESSES(&example_ipso_objects);
/*---------------------------------------------------------------------------*/
static void
setup_lwm2m_servers(void)
{
#ifdef LWM2M_SERVER_ADDRESS
  coap_endpoint_t server_ep;
  if(coap_endpoint_parse(LWM2M_SERVER_ADDRESS, strlen(LWM2M_SERVER_ADDRESS),
                         &server_ep) != 0) {
    lwm2m_rd_client_register_with_bootstrap_server(&server_ep);
    lwm2m_rd_client_register_with_server(&server_ep);
  }
#endif /* LWM2M_SERVER_ADDRESS */

  lwm2m_rd_client_use_bootstrap_server(REGISTER_WITH_LWM2M_BOOTSTRAP_SERVER);
  lwm2m_rd_client_use_registration_server(REGISTER_WITH_LWM2M_SERVER);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_ipso_objects, ev, data)
{
  static struct etimer periodic;
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  PRINTF("Starting IPSO objects example%s\n",
         REGISTER_WITH_LWM2M_BOOTSTRAP_SERVER ? " (bootstrap)" : "");
  /* Initialize the OMA LWM2M engine */
  lwm2m_engine_init();

  /* Register default LWM2M objects */
  lwm2m_device_init();
  lwm2m_security_init();
  lwm2m_server_init();

  ipso_objects_init();

  setup_lwm2m_servers();
  /* Tick loop each 5 seconds */
  etimer_set(&periodic, CLOCK_SECOND * 5);

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER && etimer_expired(&periodic)) {

      etimer_reset(&periodic);
    }
  }
  PROCESS_END();
}
