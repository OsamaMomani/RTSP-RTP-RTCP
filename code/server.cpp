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
short g_client_port=12345; // client rtp  listening port
short g_rtcp_port= 12346;  // server rtcp listening port

// All user libraries here:
#include "lib/packets.h"
#include "lib/rtcp.cpp"
#include "lib/rtp.cpp"
#include "lib/rtsp.cpp"

int RTSPPacket::cseq = 0;

int main(int argc, char *argv[])
{
    struct RTCPPacket rtcp;
    rtcp.common.pt = 201;

    ofstream *rtcp_logger= new ofstream("logs/rtcp_logger.log", ios_base::app);
    // rtsp
    pthread_t rtsp_server_thread, rtp_server_thread,rtcp_listener_thread;
    pthread_create(&rtsp_server_thread, NULL, rtsp_server, NULL);
    pthread_create(&rtp_server_thread, NULL, rtp_server, NULL);
    pthread_create(&rtcp_listener_thread, NULL, rtcp_listener, rtcp_logger);


    // join threads
    pthread_join(rtsp_server_thread, NULL);
    pthread_join(rtp_server_thread, NULL);
    pthread_join(rtcp_listener_thread, NULL);
}