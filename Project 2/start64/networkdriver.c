#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "networkdriver.h"
#include "BoundedBuffer.h"
#include "diagnostics.h"
#include "freepacketdescriptorstore__full.h"
#include "generic_queue.h"
#include "networkdevice__full.h"
#include "packetdescriptorcreator.h"


/*
Syd Lynch
slynch2
CIS 415 Project 2

This is my own work except for the included .h files which were provided
as additional resources for the project by Joe Sventek.

I discussed the project with fellow classmates Danny Lu, Jeremy Unck, and 
Robert Macy, but all code was written entirely by me.

*/


/* GLOBAL VARIABLES */

FreePacketDescriptorStore *store;
NetworkDevice *network_device;

BoundedBuffer *send_buffer;
BoundedBuffer *receive_buffer[MAX_PID+1];

pthread_t sending_thread;
pthread_t receiving_thread;

/* THREAD FUNCTIONS */

static void *send() {
	int i;
	PacketDescriptor *pd = NULL;
	for (;;) {
		/* Wait until there is a packet in the send buffer, then send to the network device */
		pd = blockingReadBB(send_buffer);
		for (i = 0; i < 2; i++) {
			if (send_packet(network_device, pd) == 1) {
				DIAGNOSTICS("[DRIVER> Info: Packet successfully sent after %d attempts.\n", i+1);
				break;
			}
			else {
				if (i == 1) {
					DIAGNOSTICS("[DRIVER> Info: Packet not sent after 2 attempts. Returning to the store.\n");
				}
			}
		}

		nonblocking_put_pd(store, pd);
	}

	return NULL;
}

static void *receive() {
	PacketDescriptor *pd = NULL;
	PID current_pid;
	for(;;) {
		/* Wait until there is a packet in the FPDS, then attempt to write it in the appropriate receive buffer */
		blocking_get_pd(store, &pd);
		current_pid = packet_descriptor_get_pid(pd);
		init_packet_descriptor(pd);
		register_receiving_packetdescriptor(network_device, pd);
		await_incoming_packet(network_device);					
		if (nonblockingWriteBB(receive_buffer[current_pid], pd) == 1) {
			DIAGNOSTICS("[DRIVER> Info: Packet written to receive buffer %u\n", current_pid);
		}
		else {
			DIAGNOSTICS("[DRIVER> Info: Receive buffer is full. Returning packet to store.\n");
			nonblocking_put_pd(store, pd);
		}		
	}
	
	return NULL;
}

void blocking_send_packet(PacketDescriptor *pd) {
	blockingWriteBB(send_buffer, pd);
	return;
}

int nonblocking_send_packet(PacketDescriptor *pd) {
	return nonblockingWriteBB(send_buffer, pd);
}

void blocking_get_packet(PacketDescriptor **pd, PID pid) {
	*pd = blockingReadBB(receive_buffer[pid]);
	return;
}

int nonblocking_get_packet(PacketDescriptor **pd, PID pid) {
	return nonblockingReadBB(receive_buffer[pid], (void**)pd);
}

void init_network_driver(NetworkDevice *nd, void *mem_start, unsigned long mem_length, FreePacketDescriptorStore **fpds_ptr) {
	int i;
	int num_packets;

	if (nd != NULL) {
		network_device = nd;
	}
	else {
		DIAGNOSTICS("[DRIVER> Warning: Null network device provided, init failure\n");
	return;
	}

	pthread_attr_t detach_attribute;
	*fpds_ptr = create_fpds();
	store = *fpds_ptr;
	num_packets = create_free_packet_descriptors(store, mem_start, mem_length);

	/* Allocate buffer sizes relative to total number of packet descriptors, splitting between the send buffer and receive buffers */
	send_buffer = createBB(num_packets / 2);
	for (i = 0; i < MAX_PID+1; i++) {
		receive_buffer[i] = createBB((num_packets / 2) / (MAX_PID+1));
	}
	
	pthread_attr_init(&detach_attribute);
	pthread_attr_setdetachstate(&detach_attribute, PTHREAD_CREATE_DETACHED);
	pthread_create(&sending_thread, &detach_attribute, &send, NULL);
	pthread_create(&receiving_thread, &detach_attribute, &receive, NULL);
	pthread_attr_destroy(&detach_attribute);
}
