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
* Reception report block
*/
struct rtcp_rr {
    unsigned int ssrc;             /* data source being reported */
    unsigned int fraction:8;  /* fraction lost since last SR/RR */

    int lost:24;              /* cumul. no. pkts lost (signed!) */
    unsigned int last_seq;         /* extended last seq. no. received */
    unsigned int jitter;           /* interarrival jitter */
    unsigned int lsr;              /* last SR packet from this source */
    unsigned int dlsr;             /* delay since last SR packet */
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
            unsigned int ntp_sec;  /* NTP timestamp */
            unsigned int ntp_frac;
            unsigned int rtp_ts;   /* RTP timestamp */
            unsigned int psent;    /* packets sent */
            unsigned int osent;    /* octets sent */
            struct rtcp_rr rr[1];  /* variable-length list */
        } sr;

        /* reception report (RR) */
        struct {
            unsigned int ssrc;     /* receiver generating this report */
            struct rtcp_rr rr[1];  /* variable-length list */
        } rr;
        /* BYE */
        struct {
            unsigned int src[1];   /* list of sources */

            /* can't express trailing text for reason */
        } bye;
    } r;
} ;



