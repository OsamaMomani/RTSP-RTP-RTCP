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
bool g_play=0, g_teardown =false;
short g_client_port=12345; // client rtp listening port
//ofstream cout = ofstream("logs/err.log", ios_base::app); // bind all cout output to log file

// All user  libraries here:
#include "lib/CImg.h"
#include "lib/packets.h"
#include "lib/rtp.cpp"
#include "lib/rtsp.cpp"
#include "lib/rtcp.cpp"

void client_player();

int RTSPPacket::cseq = 0;
pthread_t rtsp_client_thread, rtp_client_thread, rtcp_client_thread;
int main()
{
    struct RTCPPacket rtcp;

    pthread_create(&rtp_client_thread, NULL, rtp_client, NULL);

    while(!g_play) {cout<<"cc"<<g_play; sleep(1);} 
    cout << "nn"<< g_play <<endl;// wait for rtp client to start
    // rtsp
    RTSPPacket rtsp_packet; // inital rtsp setup packet
    strncpy(rtsp_packet.uri, "rtsp://127.0.0.1:5544/test.mp4", sizeof(rtsp_packet.uri));
    strncpy(rtsp_packet.session, "12345", sizeof(rtsp_packet.session));
    //rtsp_packet.cseq = 1;
    rtsp_packet.state = 200; 
    rtsp_packet.method = SETUP;
    rtsp_packet.client_port = g_client_port; 

    pthread_create(&rtsp_client_thread, NULL, rtsp_client, (void*) &rtsp_packet );
    //pthread_create(&rtsp_client_thread, NULL, rtsp_client, NULL);
    client_player();
}
using namespace cimg_library;

void client_player(){
    filesystem::create_directory( "client_buffer" );
    CImgDisplay disp;
    while(g_current_img <=500){
        if (g_teardown){
            // wait for all threads to finish (they need to send teardown packets)
            pthread_join(rtsp_client_thread, NULL);
            pthread_join(rtp_client_thread, NULL);
            //pthread_join(rtsp_client_thread, NULL);
 
            filesystem::remove_all( "client_buffer" );
            exit(0);
        }
        if (g_play  ){
            char name[200];

            sprintf(name,"client_buffer/image%03d.jpg",g_current_img);
            // first, check if file exist and not empty
            if ( ! std::filesystem::exists(name) ||  
                    std::filesystem::exists(name) && ! std::filesystem::file_size(name)  ){ 
                sleep(0.1);
                cout<<"packet lost:"<<g_current_img<<endl;
                continue;
            }

            CImg<unsigned char>img(name);
            disp = img;
            g_current_img++;
        }
        usleep(30000);
    }
    filesystem::remove_all( "client_buffer" );
    exit(0);

}