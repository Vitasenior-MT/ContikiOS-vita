CONTIKI_PROJECT = coap-example-server
all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky

# Include the CoAP implementation
MODULES += os/net/app-layer/coap

# Include CoAP resources
MODULES_REL += ./resources

CONTIKI_TARGET_SOURCEFILES += dht22.c adc-sensors.c

CONTIKI=../../..
include $(CONTIKI)/Makefile.include
