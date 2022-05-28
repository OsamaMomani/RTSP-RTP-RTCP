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
bool g_play=1, g_teardown =false;
short g_client_port=12345; // client rtp  listening port
short g_rtcp_port= 12347;  // client rtcp listening port

// All user  libraries here:
#include "lib/CImg.h"
#include "lib/packets.h"
#include "lib/rtcp.cpp"
#include "lib/rtp.cpp"
#include "lib/rtsp.cpp"

void client_player();
void controll( cimg_library::CImgDisplay* );

int RTSPPacket::cseq = 0;
pthread_t rtp_client_thread, rtcp_client_thread;
int main(int argc, char *argv[])
{
    struct RTCPPacket rtcp;

    pthread_create(&rtp_client_thread, NULL, rtp_client, NULL);

    //while(!g_play) {cout<<"cc"<<g_play<<flush; sleep(1);} 
    cout << "nn"<< g_play <<endl;// wait for rtp client to start
    // rtsp
    rtsp_client(SETUP);
    //pthread_create(&rtsp_client_thread, NULL, rtsp_client, NULL);
    client_player();
}
using namespace cimg_library;

void client_player(){
    filesystem::create_directory( "client_buffer" );
    CImgDisplay disp;
    disp.resize(768,576+400);
    while(g_current_img <=500){
        usleep(30000);
        if (g_teardown){
            // wait for all threads to finish (they need to send teardown packets)
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
            img.resize_doubleXY(); // new size(768x576)
            CImg<unsigned char> frame(disp.width(),disp.height(),1,3,0);
            frame.draw_image(0,0,0,0,img,1);

            string text = to_string(g_current_img) +"/500";
            frame.draw_text(768/2-sizeof(text),576, &text[0],"white",1,24 );

            ifstream rtcp_logger("logs/rtcp_logger.log", ios_base::app);
            char chs[800000]= {0};
            rtcp_logger.read(chs, 800000);
            text = string(chs);
            text ="RTCP information:\n" + text.substr(text.length()- 400, 400);
            frame.draw_text(30,576+30, &text[0],"white",1,24 );
            disp = frame;
            
            g_current_img++;
        }
        controll(&disp);
    }
    filesystem::remove_all( "client_buffer" );
    exit(0);

}

void controll( CImgDisplay *disp){
    if(disp->is_keySPACE()){
        g_play = !g_play;
        cout<<"pause"<<endl;
        sleep(1); // wait before next keystroke
    }
    else if(disp->is_keyQ() || disp->is_keyESC() || disp->is_closed()){
        g_teardown = true;
        cout<<"teardown"<<endl;
        sleep(1); // wait before next keystroke
    }
    else if(disp->is_keyR()){
        g_current_img = 1;
        cout<<"reset"<<endl;
        sleep(1); // wait before next keystroke
    }
    else if (disp->is_keyARROWLEFT()){
        g_current_img = g_current_img - 50 > 0 ? g_current_img - 50 : 1;
        cout<<"left"<<endl;
        usleep(500000); // wait before next keystroke
    }
    else if (disp->is_keyARROWRIGHT()){
        g_current_img = g_current_img + 50 < 500 ? g_current_img + 50 : 500;
        cout<<"right"<<endl;
        usleep(500000); // wait before next keystroke
    }
}

