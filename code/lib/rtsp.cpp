void *rtsp_server( void* ){
    cout<< "rtsp_server" << endl;
    int socketID = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketID < 0)
    {
        cout << "Error while creating rtsp server socket\n";
        return 0;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(5544); // rtsp port (554 need root access)
    int b = bind(socketID, (sockaddr *)&address, sizeof(address));
   
    if (b < 0)
    {
        cout << "Error while binding rtsp server socket\n";
        return 0;
    }

    while (true){
        RTSPPacket rtsp_packet;
        int valread = recvfrom(socketID, (char *)&rtsp_packet, sizeof(rtsp_packet),MSG_WAITALL,0,0);
        ofstream log ("logs/server.log", ios_base::app);
        log << "\n------------\nvalread: " << valread<< endl << 
        rtsp_packet.uri << endl << rtsp_packet.session << endl << rtsp_packet.cseq << endl << rtsp_packet.state << endl;
        switch (rtsp_packet.method){
            case 2:
                log << "SETUP" << endl;
                g_client_port=rtsp_packet.client_port; // set rtp client port in the server side
                g_play=true; // set play flag in the server side
                g_current_img=1; // reset image counter in the server side
                //TODO: complete setup
                break;
            case 3:
                log << "PLAY" << endl;
                g_play=true;
                break;
            case 4:
                log << "PAUSE" << endl;
                g_play=false;
                break;
            case 5:
                log << "TEARDOWN" << endl;
                g_teardown=1;
                return 0; // kill this thread
        }
    }   
}
void *rtsp_client( void* pointer){
    RTSPPacket rtsp_packet = *(RTSPPacket*)pointer;
    cout << "rtsp_client" << endl;
    int socketID = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketID < 0)
    {
        cout << "Error while creating rtsp client socket\n";
        return 0;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(5544);
    sleep(1);  // wait for server
    int valread = sendto(socketID, (char *)&rtsp_packet, sizeof(rtsp_packet), 0, (struct sockaddr *)&address, sizeof(address));
    
     if (valread < 0)
    {
        cout << "Error while sending rtsp packet\n";
        return 0;
    }
    return 0;
 
}

