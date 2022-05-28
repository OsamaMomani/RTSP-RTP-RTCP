// --------------------------- RTSP Packet ---------------------------------
enum  method_t {
    OPTIONS=0,
    DESCRIBE=1,
    SETUP=2,
    PLAY=3,
    PAUSE=4,
    TEARDOWN=5
};


struct RTSPPacket
{
    method_t method;
    short client_port;
    char uri[256]="";
    char  session[256];
    static int cseq;
    int state; // 100-199 for request, 200-299 for success response, 300-399 for redirect response, 400-499 for client error, 500-599 for server error
    int npt[2]; // Normal Play time in seconds, used for PLAY to indicate the time to start playing
    // [start, end (not implemented in this code)]
};

// --------------------------- RTP Packet ---------------------------------
struct RTPPacket
{   // this struct follows the exact format of an RTP packet
    unsigned short first_word=0b1000000000011010;
    unsigned short sequence=0;
    unsigned long  timestamp=time(0);
    unsigned long  ssrc;
    char  payload[10000]={0};
};
/* 
struct rtp_hdr_t {
    unsigned int version:2;   // protocol version
    unsigned int p:1;         // padding flag 
    unsigned int x:1;         // header extension flag
    unsigned int cc:4;        // CSRC count 
    unsigned int m:1;         // marker bit 
    unsigned int pt:7;        // payload type 
    unsigned int seq:16;      // sequence number 
};

*/

// --------------------------- RTCP Packet ---------------------------------
/*
rtcp_common:
    define common fields for all RTCP packets types(SR,RR,SDES,BYE,APP)
*/

struct rtcp_common {
    unsigned int version:2;   /* protocol version */
    unsigned int p:1;         /* padding flag */
    unsigned int count:5;     /* varies by packet type */
    unsigned int pt:8;        /* RTCP packet type 
    abbrev.  name                 value
    SR       sender report          200
    RR       receiver report        201
    SDES     source description     202
    BYE      goodbye                203
    APP      application-defined    204*/
    unsigned short length;           /* pkt len in words, w/o this word */
} ;


/*
rtcp_rb:
    define reception report block
    SR, RR have one or more reception report blocks
*/
struct rtcp_rb {
    unsigned int ssrc;              /* data source being reported */
    unsigned int fraction:8;        /* fraction lost since last SR/RR */
    int lost:24;                    /* cumul. no. pkts lost (signed!) */
    unsigned int last_seq;          /* extended last seq. no. received */
    unsigned int jitter;            /* interarrival jitter */
    unsigned int lsr;               /* last SR packet from this source */
    unsigned int dlsr;              /* delay since last SR packet */
};


/*
* Generic RTCP packet
*/
struct RTCPPacket {
    rtcp_common common;     /* common header */
    union {
        /* sender report (SR) */
        struct {
            unsigned int ssrc;     /* sender generating this report */
            unsigned int ntp_sec;  // NTP timestamp   <--| These are the only
            unsigned int ntp_frac; //                 <--| additional 20-bytes
            unsigned int rtp_ts;   // RTP timestamp   <--| fields besides 
            unsigned int psent;    // packets sent    <--| the RR 
            unsigned int osent;    // octets sent     <--| header.
            struct rtcp_rb rb;  /* variable-length list */
        } sr;

        /* reception report (RR) */
        struct {
            unsigned int ssrc;     /* receiver generating this report */
            struct rtcp_rb rb;  /* variable-length list */
        } rr;
        /* BYE */
        struct {
            unsigned int ssrc;   /* list of sources */

            // OPTIONAL reason for leaving and length of reason in bytes 
            unsigned int length:8;
            unsigned char reason[256];
        } bye;
    } r;
} ; //


/*

------------------------ RTCP Packet formats based on RFC3550 ------------------------:

6.4.1 SR: Sender Report RTCP Packet

        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=SR=200   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         SSRC of sender                        |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
sender |              NTP timestamp, most significant word             |
info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |             NTP timestamp, least significant word             |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         RTP timestamp                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     sender's packet count                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      sender's octet count                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



6.4.2 RR: Receiver Report RTCP Packet

        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


6.6 BYE: Goodbye RTCP Packet

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |V=2|P|    SC   |   PT=BYE=203  |             length            |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                           SSRC/CSRC                           |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      :                              ...                              :
      +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
(opt) |     length    |               reason for leaving            ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



*/
