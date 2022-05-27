// All standard libraries here:
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <time.h>

#include <string>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <fstream>
#include <filesystem>

#include <pthread.h>
using namespace std;

// Global shared varibles here:
int g_current_img = 1;
bool g_play, g_teardown =false;
short g_client_port=12345; // client rtp listening port

// All user libraries here:
#include "lib/packets.h"
#include "lib/rtp.cpp"
#include "lib/rtsp.cpp"
#include "lib/rtcp.cpp"

int RTSPPacket::cseq = 0;

int main()
{
    struct RTCPPacket rtcp;
    rtcp.common.pt = 201;

    // rtsp
    pthread_t rtsp_server_thread, rtp_server_thread;
    pthread_create(&rtsp_server_thread, NULL, rtsp_server, NULL);
    pthread_create(&rtp_server_thread, NULL, rtp_server, NULL);


    // join threads
    pthread_join(rtsp_server_thread, NULL);
    pthread_join(rtp_server_thread, NULL);
}