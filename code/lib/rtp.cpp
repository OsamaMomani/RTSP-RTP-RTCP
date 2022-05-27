void *rtp_server(void *i)
{
    cout << "rtp_server" << endl;
    int socketID = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketID < 0)
    {
        cout << "Error while creating rtp server socket\n";
        return 0;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(g_client_port); // rtp client port (initialized in rtsp_server)
    int addrlength = sizeof(address);

    while (true)
    { // g_play is initialized in rtsp_server
        if (g_current_img > 500)
        {
            g_play = false;
            g_current_img = 1;
        }
    
        static RTPPacket rtp_packet;
        char name[200];
        sprintf(name, "server_vid/image%03d.jpg", g_current_img);
        ifstream img(name, ios::binary);
        if (!img.is_open())
        {
            cout << "Error opening image file"<<g_current_img;
            return 0;
        }
        img.read(rtp_packet.payload, sizeof(rtp_packet.payload));
        img.close();
        rtp_packet.sequence++;
        rtp_packet.timestamp = time(0);
        rtp_packet.ssrc = 0x12345678;   // TODO
        if(g_teardown) return 0;        // if rtsp teardown, kill this thread
        while(!g_play) {cout<<"rtp waiting play";sleep(1);}     // if rtsp pause, wait for play
        sendto(socketID, &rtp_packet, sizeof(rtp_packet), 0, (struct sockaddr *)&address, addrlength) !=-1 || cout << "Error sending rtp packet\n";
        cout<<"rtp sent:"<<g_current_img<< endl;
        usleep(1000);
        g_current_img++;
    }
    return 0 ;
}

void *rtp_client(void *i)
{
    cout << "rtp_client" << endl;
    int socketID = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketID < 0)
    {
        cout << "Error while creating rtp client socket\n";
        return 0;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(g_client_port); // rtp listening port
    int addrlength = sizeof(address);
    int b = bind(socketID, (sockaddr *)&address, sizeof(address));
    if (b < 0)
    {
        cout << "Error while binding rtp server socket\n";
        return 0;
    }
    while (true)
    {
        RTPPacket rtp_packet;
        cout<<"www"<< g_play <<endl;
        int valread;
        g_play=1 ;
        valread =recvfrom(socketID, (char *)&rtp_packet, sizeof(rtp_packet), MSG_WAITALL, 0, 0);
        // set g_play and start recieving rtp packets
        cout << "\n------------\nvalread: " << valread << endl
            << rtp_packet.first_word << endl
            << rtp_packet.sequence << endl
            << rtp_packet.timestamp << endl;
        char name[200];
        sprintf(name, "client_buffer/image%03d.jpg", rtp_packet.sequence);
        ofstream img(name, ios::binary);
        img.write(rtp_packet.payload, sizeof(rtp_packet.payload));
        img.close();
        
    }

}