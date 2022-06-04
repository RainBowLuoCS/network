//
// Created by LuoRun on 22/06/1.
//

#ifndef CN_EXP_HANDLERS_H
#define CN_EXP_HANDLERS_H
#include "pcap.h"
#include "definitions.h"
#include <errno.h>
#include <maxminddb.h>
#include <string.h>
using namespace std;

string get_address_by_ip(MMDB_s mmdb,const char* ip_address);

void packet_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data);
void ethernet_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data);
void ipv4_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data);
void ipv6_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data);
void arp_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data);
void icmp_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data);
void tcp_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data);
void udp_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data);
#endif //CN_EXP_HANDLERS_H
