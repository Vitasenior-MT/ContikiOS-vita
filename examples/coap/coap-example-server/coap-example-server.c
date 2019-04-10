/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Erbium (Er) CoAP Engine example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "dev/leds.h"
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "dev/i2c.h"
#include "dev/dht22.h"
/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "COAP"
#define LOG_LEVEL LOG_LEVEL_INFO
/*---------------------------------------------------------------------------*/
#define WITH_SERVER_REPLY  0
#define UDP_CLIENT_PORT	8002
#define UDP_SERVER_PORT 10000
#define temp_threshold_max temp_max
#define temp_threshold_min temp_min
#define humi_threshold_max humi_max
#define humi_threshold_min humi_min
#define mono_threshold_max mono_max
#define diox_threshold_max diox_max
int temp_max = 28;
int temp_min = 10;
int humi_max = 70;
int humi_min = 10;
int mono_max = 200;
int diox_max = 12500;
static struct simple_udp_connection udp_conn;
uip_ipaddr_t server_ipaddr;
//mensagem vinda do servidor NodeJs
static char *message;
/*---------------------------------------------------------------------------*/
#define ADC_SENSORS_CONF_ADC3_PIN     2             /**< Hard-coded to PA2    */
#define ADC_SENSORS_CONF_ADC2_PIN     4             /**< Hard-coded to PA4    */
/*---------------------------------------------------------------------------*/
static struct etimer et;
static struct etimer et2;
int temperature_total, humidity_total, mono_total, diox_total;
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  //estrair mensagem
  message = (char *)data;
  LOG_INFO("Received request %s from ", message);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  char temp_maxchar[10];
  sprintf(temp_maxchar,"temp_max");
  char temp_minchar[10];
  sprintf(temp_minchar,"temp_min");
  char humi_maxchar[10];
  sprintf(humi_maxchar,"humi_max");
  char humi_minchar[10];
  sprintf(humi_minchar,"humi_min");
  char mono_maxchar[10];
  sprintf(mono_maxchar,"mono_max");
  char diox_maxchar[10];
  sprintf(diox_maxchar,"diox_max");
  char returnmsg[50];
  if(strstr(message,temp_maxchar) != NULL){
    int i = 9;
    char aux[1];
    int f = 0;
    while(message[i] != '#'){
    	aux[f]=message[i];
    	LOG_INFO("aux - %s\n",aux);
    	i++;
    	f++;
    }
    int valuemsg = atoi(aux);
    temp_max = valuemsg;
    LOG_INFO("CHANGE MAX TEMP THRESHOLD TO %d\n",valuemsg);
    sprintf(returnmsg, "CHANGED MAX TEMP THRESHOLD TO %d  #",valuemsg);
    LOG_INFO("Sending --- %s     to ", returnmsg);
    LOG_INFO_6ADDR(&server_ipaddr);
    LOG_INFO_("\n");
    simple_udp_sendto(&udp_conn, &returnmsg, sizeof(returnmsg), &server_ipaddr);
   }else if(strstr(message,temp_minchar) != NULL){
    int i = 9;
    char aux[1];
    int f = 0;
    while(message[i] != '#'){
    	aux[f]=message[i];
    	LOG_INFO("aux - %s\n",aux);
    	i++;
    	f++;
    }
    int valuemsg = atoi(aux);
    temp_min = valuemsg;
    LOG_INFO("CHANGE MIN TEMP THRESHOLD TO %d\n",valuemsg);
    sprintf(returnmsg, "CHANGED MIN TEMP THRESHOLD TO %d  #",valuemsg);
    LOG_INFO("Sending --- %s     to ", returnmsg);
    LOG_INFO_6ADDR(&server_ipaddr);
    LOG_INFO_("\n");
    simple_udp_sendto(&udp_conn, &returnmsg, sizeof(returnmsg), &server_ipaddr);
   }else if(strstr(message,humi_maxchar) != NULL){
    int i = 9;
    char aux[1];
    int f = 0;
    while(message[i] != '#'){
    	aux[f]=message[i];
    	LOG_INFO("aux - %s\n",aux);
    	i++;
    	f++;
    }
    int valuemsg = atoi(aux);
    humi_max = valuemsg;
    LOG_INFO("CHANGE MAX HUMI THRESHOLD TO %d\n",valuemsg);
    sprintf(returnmsg, "CHANGED MAX HUMI THRESHOLD TO %d  #",valuemsg);
    LOG_INFO("Sending --- %s     to ", returnmsg);
    LOG_INFO_6ADDR(&server_ipaddr);
    LOG_INFO_("\n");
    simple_udp_sendto(&udp_conn, &returnmsg, sizeof(returnmsg), &server_ipaddr);
   }else if(strstr(message,humi_minchar) != NULL){
    int i = 9;
    char aux[1];
    int f = 0;
    while(message[i] != '#'){
    	aux[f]=message[i];
    	LOG_INFO("aux - %s\n",aux);
    	i++;
    	f++;
    }
    int valuemsg = atoi(aux);
    humi_min = valuemsg;
    LOG_INFO("CHANGE MIN HUMI THRESHOLD TO %d\n",valuemsg);
    sprintf(returnmsg, "CHANGED MIN HUMI THRESHOLD TO %d  #",valuemsg);
    LOG_INFO("Sending --- %s     to ", returnmsg);
    LOG_INFO_6ADDR(&server_ipaddr);
    LOG_INFO_("\n");
    simple_udp_sendto(&udp_conn, &returnmsg, sizeof(returnmsg), &server_ipaddr);
   }else if(strstr(message,mono_maxchar) != NULL){
    int i = 9;
    char aux[1];
    int f = 0;
    while(message[i] != '#'){
    	aux[f]=message[i];
    	LOG_INFO("aux - %s\n",aux);
    	i++;
    	f++;
    }
    int valuemsg = atoi(aux);
    mono_max = valuemsg;
    LOG_INFO("CHANGE MAX MONO THRESHOLD TO %d\n",valuemsg);
    sprintf(returnmsg, "CHANGED MAX MONO THRESHOLD TO %d  #",valuemsg);
    LOG_INFO("Sending --- %s     to ", returnmsg);
    LOG_INFO_6ADDR(&server_ipaddr);
    LOG_INFO_("\n");
    simple_udp_sendto(&udp_conn, &returnmsg, sizeof(returnmsg), &server_ipaddr);
  }else if(strstr(message,diox_maxchar) != NULL){
    int i = 9;
    char aux[1];
    int f = 0;
    while(message[i] != '#'){
    	aux[f]=message[i];
    	LOG_INFO("aux - %s\n",aux);
    	i++;
    	f++;
    }
    int valuemsg = atoi(aux);
    diox_max = valuemsg;
    LOG_INFO("CHANGE MAX DIOX THRESHOLD TO %d\n",valuemsg);
    sprintf(returnmsg, "CHANGED MAX DIOX THRESHOLD TO %d  #",valuemsg);
    LOG_INFO("Sending --- %s     to ", returnmsg);
    LOG_INFO_6ADDR(&server_ipaddr);
    LOG_INFO_("\n");
    simple_udp_sendto(&udp_conn, &returnmsg, sizeof(returnmsg), &server_ipaddr);
  }else{
    LOG_INFO("Invalid message\n");
  }
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;
  uip_ip6addr(&ipaddr, 0xfd00, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
  uip_ip6addr(&server_ipaddr, 0xfd00, 0, 0, 0, 0, 0, 0, 1);
}
/*---------------------------------------------------------------------------*/
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t
  res_chunks,
  res_separate,
  res_push,
  res_event,
  res_sub,
  res_b1_sep_b2;
extern coap_resource_t res_dht22;
extern coap_resource_t res_mono;
extern coap_resource_t res_allsensors;
PROCESS(er_example_server, "Erbium Example Server");
AUTOSTART_PROCESSES(&er_example_server);
PROCESS_THREAD(er_example_server, ev, data)
{
	static int seq_id = 0;
  static int counter = 0;
  float temperature_value, humidity_value, mono_value, diox_value;
  static int temp_average[] = {0,0,0,0,0};
  static int humi_average[] = {0,0,0,0,0};
  static int mono_average[] = {0,0,0,0,0};
  static int diox_average[] = {0,0,0,0,0};
  char warningbufftemp[60];
  char warningbuffhumi[60];
  char warningbuffmono[60];
  char warningbuffdiox[60];
  PROCESS_BEGIN();
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback);
  set_global_address();
  etimer_set(&et2, (CLOCK_SECOND * 5));
  /* Wait a bit */
  etimer_set(&et, (CLOCK_SECOND * 2));
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  PROCESS_PAUSE();
  LOG_INFO("Starting COAP Server\n");
  /* Configure the ADC ports */
  //CO Sensor
  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC2);
  //CO2 Sensor
  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC3);
  //Sensor temperatura e humidade
  SENSORS_ACTIVATE(dht22);
  coap_activate_resource(&res_allsensors, "sensors/allsensors");
	/* Wait a bit */
  etimer_set(&et, (CLOCK_SECOND*30));
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  /* Define application-specific events here. */
  int aux = 0;
  int aux2 = 0;
  float aux3 = 0;
  int value = 0;
  float xpto = 0;
  int voltage = 0;
  int16_t temperature = 0, humidity = 0, diox = 0, mono = 0;
  while(1) {
    LOG_INFO("Counter value --- %d   \n", counter);
    seq_id++;
    xpto = 0;
    value = 0;
    aux = 0;
    voltage = adc_zoul.value(ZOUL_SENSORS_ADC3);
    // 27980 (max registado)   -  5 v
    // voltage                 -  x v
    xpto = (float)voltage * 5;
    xpto = xpto / 27980;
    // 4v      - 2000 ppm
    // voltage - x ppm
    aux = ((2000 *  xpto) / 4);
    value = (int)aux;
    // AC voltage value, with applied corresponding sensor algorithm
    LOG_INFO("AC voltage = %d V // CO2 value = %d ppm\n", voltage, value);
    diox = value;
    voltage = 0;
    xpto = 0;
    value = 0;
    aux = 0;
    aux2 = 0;
    voltage = adc_zoul.value(ZOUL_SENSORS_ADC2);
    //somar erro
    voltage = voltage + 620;
    //se for abaixo de VREF, então co=0ppm
    if(voltage < 17000){
      aux = 0;
      aux2 = 0;
    }else{
      xpto = voltage / 10000;
      aux = (int)xpto;
      //LOG_INFO("aux: %d\n", aux);
      xpto = voltage % 10000;
      aux2 = (int)xpto;
      //LOG_INFO("aux2: %d\n", aux2);
      xpto = (float)aux + ((float)aux2*0.0001);
      LOG_INFO("Vgas: %d,%d\n", aux, aux2);
      //Cx = 1/M x (Vgas - Vgas0)
      //M = SensivityCode x TIAGain x 10^-9 x 10^3
      //M = 3.70 x 100 x 10^-9 x 10^3 = 0.000370 x 1000
      //Vgas0 = Vref + Voffset
      //xpto = ((float)voltage * 5) / 1024;
      value = (1 / 0.000370) * (xpto - 1.70);
      //offset do sensor (calibracao manual)
      value = value - 40.16;
      aux = (int)(value / 1);
    }
    //AC voltage value, with applied corresponding sensor algorithm
    LOG_INFO("AC voltage = %d mV // CO value = %d ppm\n", voltage, aux);
    mono = aux;	
    dht22_read_all(&temperature, &humidity);
    LOG_INFO("TEMP = %d mV // HUMI = %d ppm\n", temperature, humidity);
    temp_average[0] = temp_average[1];
    temp_average[1] = temp_average[2];
    temp_average[2] = temp_average[3];
    temp_average[3] = temp_average[4];
    temp_average[4] = temperature / 10;
    temperature_value = ((float)temp_average[0] + (float)temp_average[1] + (float)temp_average[2] + (float)temp_average[3] + (float)temp_average[4]) / 5;
    temperature_total = (int)temperature_value;
    LOG_INFO("Temp value --- %d     \n", temperature_total);
    humi_average[0] = humi_average[1];
    humi_average[1] = humi_average[2];
    humi_average[2] = humi_average[3];
    humi_average[3] = humi_average[4];
    humi_average[4] = humidity / 10;
    humidity_value = ((float)humi_average[0] + (float)humi_average[1] + (float)humi_average[2] + (float)humi_average[3] + (float)humi_average[4]) / 5;
    humidity_total = (int)humidity_value;
    LOG_INFO("Humi value --- %d     \n", humidity_total);
    mono_average[0] = mono_average[1];
    mono_average[1] = mono_average[2];
    mono_average[2] = mono_average[3];
    mono_average[3] = mono_average[4];
    mono_average[4] = mono;
    mono_value = ((float)mono_average[0] + (float)mono_average[1] + (float)mono_average[2] + (float)mono_average[3] + (float)mono_average[4]) / 5;
    if(mono_value < 0){
      mono_value = 0;    
    }
    mono_total = (int)mono_value;
    LOG_INFO("Mono value --- %d     \n", mono_total);
    diox_average[0] = diox_average[1];
    diox_average[1] = diox_average[2];
    diox_average[2] = diox_average[3];
    diox_average[3] = diox_average[4];
    diox_average[4] = diox;
    diox_value = ((float)diox_average[0] + (float)diox_average[1] + (float)diox_average[2] + (float)diox_average[3] + (float)diox_average[4]) / 5;
    if(diox_value < 0){
      diox_value = 0;    
    }
    diox_total = (int)diox_value;
    LOG_INFO("diox value --- %d     \n", diox_total);
    if(counter > 5){
      if (temperature_total > temp_threshold_max || temperature_total < temp_threshold_min){
        if(NETSTACK_ROUTING.node_is_reachable()) {
          sprintf(warningbufftemp, "{'WARNING':{'temp':'%d'}}#", temperature_total);
          LOG_INFO_("\n ");
          LOG_INFO("Sending WARNING TEMP --- %s     to ", warningbufftemp);
          LOG_INFO_6ADDR(&server_ipaddr);
          LOG_INFO_("\n");
            /* Wait a bit */
          etimer_set(&et, (CLOCK_SECOND * 2));
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            simple_udp_sendto(&udp_conn, &warningbufftemp, sizeof(warningbufftemp), &server_ipaddr);
            /* Wait a bit */
          etimer_set(&et, (CLOCK_SECOND * 2));
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
          } else {
            LOG_INFO("Not reachable yet\n");
          }
        LOG_INFO(" ------------------------------------------------------ \n");
      }
      if (humidity_total > humi_threshold_max || humidity_total < humi_threshold_min){
        if(NETSTACK_ROUTING.node_is_reachable()) {
          sprintf(warningbuffhumi, "{'WARNING':{'humi':'%d'}}#", humidity_total);
          LOG_INFO_("\n ");
          LOG_INFO("Sending WARNING HUMI --- %s     to ", warningbuffhumi);
          LOG_INFO_6ADDR(&server_ipaddr);
          LOG_INFO_("\n");
            /* Wait a bit */
          etimer_set(&et, (CLOCK_SECOND * 2));
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            simple_udp_sendto(&udp_conn, &warningbuffhumi, sizeof(warningbuffhumi), &server_ipaddr);
            /* Wait a bit */
          etimer_set(&et, (CLOCK_SECOND * 2));
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
          } else {
            LOG_INFO("Not reachable yet\n");
          }
        LOG_INFO(" ------------------------------------------------------ \n");
      }
      if (mono_total > mono_threshold_max){
        if(NETSTACK_ROUTING.node_is_reachable()) {
          sprintf(warningbuffmono, "{'WARNING':{'mono':'%d'}}#", mono_total);
          LOG_INFO_("\n ");
          LOG_INFO("Sending WARNING MONOXIDE --- %s     to ", warningbuffmono);
          LOG_INFO_6ADDR(&server_ipaddr);
          LOG_INFO_("\n");
            /* Wait a bit */
          etimer_set(&et, (CLOCK_SECOND * 2));
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            simple_udp_sendto(&udp_conn, &warningbuffmono, sizeof(warningbuffmono), &server_ipaddr);
            /* Wait a bit */
          etimer_set(&et, (CLOCK_SECOND * 2));
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
          } else {
            LOG_INFO("Not reachable yet\n");
          }
        LOG_INFO(" ------------------------------------------------------ \n");
      }
      if (diox_total > diox_threshold_max){
        if(NETSTACK_ROUTING.node_is_reachable()) {
          sprintf(warningbuffdiox, "{'WARNING':{'diox':'%d'}}#", diox_total);
          LOG_INFO_("\n ");
          LOG_INFO("Sending WARNING DIOXIDE --- %s     to ", warningbuffdiox);
          LOG_INFO_6ADDR(&server_ipaddr);
          LOG_INFO_("\n");
            /* Wait a bit */
          etimer_set(&et, (CLOCK_SECOND * 2));
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            simple_udp_sendto(&udp_conn, &warningbuffdiox, sizeof(warningbuffdiox), &server_ipaddr);
            /* Wait a bit */
          etimer_set(&et, (CLOCK_SECOND * 2));
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
          } else {
            LOG_INFO("Not reachable yet\n");
          }
        LOG_INFO(" ------------------------------------------------------ \n");
        }
      }
      counter++;
      etimer_set(&et2, (CLOCK_SECOND * 60));
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));
      /* Call the event_handler for this application-specific event. */
      res_event.trigger();
      /* Also call the separate response example handler. */
      res_separate.resume();
      if(counter > 1000){
        counter = 5;
      } 
  }   /* while (1) */
  PROCESS_END();
}