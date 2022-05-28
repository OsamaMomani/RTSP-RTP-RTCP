void rtcp_logger(ofstream& out, RTCPPacket rtcp_packet)
{
    if (rtcp_packet.common.pt == 200)
    {
        out << "Sender Report (SR)" << endl;
        out << "SSRC: " << rtcp_packet.r.sr.ssrc << endl;
        out << "NTP timestamp: " << rtcp_packet.r.sr.ntp_sec << "." << rtcp_packet.r.sr.ntp_frac << endl;
        out << "RTP timestamp: " << rtcp_packet.r.sr.rtp_ts << endl;
        out << "Packets sent: " << rtcp_packet.r.sr.psent << endl;
        out << "Octets sent: " << rtcp_packet.r.sr.osent << endl;
        out << "Lost packets: " << rtcp_packet.r.sr.rb.lost << endl;
        out << "Last sequence number: " << rtcp_packet.r.sr.rb.last_seq << endl;
        out << "Jitter: " << rtcp_packet.r.sr.rb.jitter << endl;
        out << "Last SR timestamp: " << rtcp_packet.r.sr.rb.lsr << endl;
        out << "Delay since last SR: " << rtcp_packet.r.sr.rb.dlsr << endl;
    }
    else if (rtcp_packet.common.pt == 201)
    {
        out << "Receiver Report (RR)" << endl;
        out << "SSRC: " << rtcp_packet.r.rr.ssrc << endl;
        out << "Fraction lost: " << rtcp_packet.r.rr.rb.fraction << endl;
        out << "Lost packets: " << rtcp_packet.r.rr.rb.lost << endl;
        out << "Last sequence number: " << rtcp_packet.r.rr.rb.last_seq << endl;
        out << "Jitter: " << rtcp_packet.r.rr.rb.jitter << endl;
        out << "Last SR timestamp: " << rtcp_packet.r.rr.rb.lsr << endl;
        out << "Delay since last SR: " << rtcp_packet.r.rr.rb.dlsr << endl;
    }
    else if (rtcp_packet.common.pt == 203)
    {
        out << "Goodbye (Bye)" << endl;
        out << "SSRC: " << rtcp_packet.r.bye.ssrc << endl;
        out << "Reason: len( " << rtcp_packet.r.bye.length<<" ) " <<rtcp_packet.r.bye.reason << endl;
    }
    else
    {
        out << "Unknown RTCP packet type" << endl;
    }
}





void* rtcp_listener(void *ofstream_ptr)
{
    cout << "rtcp_listener" << endl;
    int socketID = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketID < 0)
    {
        cout << "Error while creating rtcp client socket\n"<<flush;
        return 0;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons( g_rtcp_port ); // rtcp client port (initialized in rtsp_server)
    int bind_result = bind(socketID, (struct sockaddr *)&address, sizeof(address));
    if (bind_result < 0)
    {
        cout << "Error while binding rtcp client socket\n"<<flush;
        return 0;
    }
    RTCPPacket rtcp_packet;
    while (true)
    {
        int recv_result = recvfrom(socketID, &rtcp_packet, sizeof(rtcp_packet), MSG_WAITALL, 0, 0);
        if (recv_result < 0)
        {
            cout << "Error while receiving rtcp packet\n"<<flush;
            return 0;
        }
        //TODO calc(&rtcp_packet);
        rtcp_logger(*(ofstream*)ofstream_ptr, rtcp_packet);
    }
}

void rtcp_send(int socketID, struct sockaddr_in* address, RTCPPacket rtcp_packet)
{
    int send_result = sendto(socketID, &rtcp_packet, sizeof(rtcp_packet), 0, (struct sockaddr *)address, sizeof(*address));
    if (send_result < 0)
    {
        cout << "Error while sending rtcp packet\n";
    }
}