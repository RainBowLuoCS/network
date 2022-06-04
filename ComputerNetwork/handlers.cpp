//
// Created by LuoRun on 22/06/1.
//

#include "handlers.h"
#include "utils.h"
#include "helpers.h"
#pragma warning(disable:4996)
#include <ctime>
#include <iostream>
#include <winsock2.h>
#include <tuple>
#include <stdio.h>

using namespace std;
using namespace defs;

string get_address_by_ip(MMDB_s mmdb,const char* ip_address)
{
    MMDB_entry_data_s entry_data;
    MMDB_lookup_result_s result;
    int gai_error, mmdb_error, status;
    string b;
    result = MMDB_lookup_string(&mmdb, ip_address, &gai_error, &mmdb_error);
    if (0 != gai_error) {
        return b;
    }
    if (MMDB_SUCCESS != mmdb_error) {
        return b;
    }
    if (result.found_entry) {

        /* 获取城市名称(英文) */
        status = MMDB_get_value(&result.entry, &entry_data, "city", "names", "en", NULL);
        if (MMDB_SUCCESS == status) {/* MMDB_get_value 成功 */
            if (entry_data.has_data) {/* 找到了想要的数据 */
                if (entry_data.type == MMDB_DATA_TYPE_UTF8_STRING) {
                    /* entry_data.utf8_string 返回的不是null-terminated 字符串,需要根据长度自己截取数据 */
                    string a(entry_data.utf8_string);
                    return a.substr(0, entry_data.data_size);
                }
            }
            else {
                return b;
            }
        }
        else {
            return b;
        }
    }
    return b;
}

void packet_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data) {
    tm *lt;
    char time_str[16];
    time_t local_tv_sec = header->ts.tv_sec;
    lt = localtime(&local_tv_sec);
    strftime(time_str, sizeof time_str, "%H:%M:%S", lt);
    cout << endl << "Time: " << time_str << ", " << header->ts.tv_usec
         << "\t\tLength: " << header->len << endl;
    ethernet_pkg_handler(param, header, data);
}

void ethernet_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data) {
    auto eh = (eth_header *)data;
    auto type = ntohs(eh->type);
    cout << "Type: 0x" << to_hex_string(type) << " (" << (eth_type)type << ")\n";
    cout << "Destination address: " << eh->des << endl;
    cout << "Source address: " << eh->src << endl;
    switch ((eth_type)type) {
        case IPv4:
            ipv4_pkg_handler(param,header,data);
            break;
        case IPv6:
            ipv6_pkg_handler(param, header, data);
            break;
        case ARP:
            arp_pkg_handler(param, header, data);
            break;
        case RARP:
            cerr << "We don\'t handle RARP packets. Process exited." << endl;
            break;
        default: cerr << "Unknown eth_type type recognized." << endl;
    }
}

void ipv4_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data) {
    	MMDB_s mmdb;
	int status;
	const char* filename = "../../../GitRepository/Clone/gocapture/GeoLite2-City.mmdb";
	status = MMDB_open(filename, MMDB_MODE_MMAP, &mmdb);
    auto ih = (ipv4_header *)(data + eth_header_length);
    cout << "Version: " << (ih->v_ihl & 0xf0 >> 4) << endl;
    const auto ihl = ih->v_ihl & 0x0f;
    cout << "Header length: " << ihl << " (" << (ihl * 4) << "B)" << endl;
    cout << "Type of service: " << int(ih->tos) << endl;
    cout << "Total length: " << ntohs(ih->len) << endl;
    cout << "Identification: " << ntohs(ih->id) << endl;
    const auto flags = ih->fl_fof & 0xe000 >> 12;
    const auto offset = ih->fl_fof & 0x1fff;
    cout << "Flags: " << flags << endl;
    cout << "Fragment offset: " << offset << " (" << offset * 8 << " B)" << endl;
    cout << "Protocol: " << (ipv4_protocol)ih->proto << endl;
    cout << "Checksum: " << ntohs(ih->checksum) << endl;
    //string a = get_address_by_ip(mmdb, (const char*)temp_a);
    //if(a.size()!=0)
    //    cout << "Source IP address: " << ih->src << "  city is:" <<a<< endl;

    cout << "Source IP address:  " << ih->src << endl;
    //a = get_address_by_ip(mmdb, (const char*)temp_b);
    //if (a.size() != 0)
    //    cout << "Destination IP address: " << ih->des << "  city is:" << a << endl;
    cout << "Destination IP address:  " << ih->des << endl;
    //MMDB_close(&mmdb);
    switch ((ipv4_protocol)ih->proto) {
        case TCP:
            tcp_pkg_handler(param, header, data);
            break;
        case UDP:
            udp_pkg_handler(param, header, data);
            break;
        case ICMP:
            icmp_pkg_handler(param, header, data);
            break;
        default: cerr << "Unknown ipv4_protocol type recognized." << endl;
    }
    helpers::protocol_helper_callback(ih);
}

void ipv6_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data) {
    auto ih = (ipv6_header *)(data + eth_header_length);
    uint ver;
    uint tc;
    uint fl;
    std::tie(ver, tc, fl) = converter::ipv6_vtf_extract(ih->vtf);
    cout << "Version: " << ver << endl;
    cout << "Traffic class: " << tc << endl;
    cout << "Flow label: " << fl << endl;
    cout << "Payload length: " << ntohs(ih->len) << endl;
    cout << "Next head: " << ntohs(ih->next) << endl;
    cout << "Hop Limit: " << int(ih->ttl) << endl;
    cout << "Source IP address: " << ih->src << endl;
    cout << "Destination IP address: " << ih->des << endl;
    switch ((ipv6_protocol) ih->next) {
        case TCPv6:
            tcp_pkg_handler(param, header, data);
            break;
        case UDPv6:
            udp_pkg_handler(param, header, data);
            break;
        case ICMPv6:
            icmp_pkg_handler(param, header, data);
            break;
        default: cerr << "Unknown ipv6_protocol type recognized." << endl;
    }
    helpers::protocol_helper_callback(ih);
}

void arp_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data) {
    auto ah = (arp_header *)(data + eth_header_length);
    cout << "Hardware type: " << ntohs(ah->hwt) << endl;
    cout << "Protocol type: " << cover_to_hex(ntohs(ah->prt)) << endl;
    cout << "Hardware address length: " << int(ah->hwl) << endl;
    cout << "Protocol address length: " << int(ah->prl) << endl;
    cout << "Operation code: " << (arp_oc)ah->op << endl;
    cout << "Source IP address: " << ah->src_ip << endl;
    cout << "Destination IP address: " << ah->des_ip << endl;
    cout << "Source MAC address: " << ah->src_mac << endl;
    cout << "Destination MAC address: " << ah->des_mac << endl;
    helpers::protocol_helper_callback(ah);
}

void icmp_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data) {
    auto ih = (icmp_header *)(data + pro_header_length);
    cout << "ICMP type: " << (icmp_type)ih->type << endl;
    cout << "ICMP code: " << ih->code << endl;
    cout << "Identification: " << ih->id << endl;
    cout << "Sequence: " << ih->seq << endl;
    cout << "ICMP checksum: " << ih->checksum << endl;
}

void tcp_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data) {
    auto th = (tcp_header *)(data + pro_header_length);
    cout << "Source port: " << ntohs(th->sp) << endl;
    cout << "Destination port: " << ntohs(th->dp) << endl;
    cout << "Sequence: " << ntohl(th->sq) << endl;
    cout << "Acknowledgement: " << ntohl(th->ac) << endl;
    const auto offset = th->off & 0xf0 >> 4;
    cout << "Data offset: " << offset << " (" << (offset >> 2) << " B)" << endl;
    cout << "Flags: " << converter::tcp_flag_to_string(th->fl) << endl;
    cout << "Window: " << th->windows << endl;
    cout << "Checksum: " << th->checksum << endl;
    cout << "Urgent pointer: " << th->up << endl;
}

void udp_pkg_handler(uchar* param, const struct pcap_pkthdr* header, const uchar* data) {
    auto uh = (udp_header *)(data + pro_header_length);
    cout << "Source port: " << ntohs(uh->sp) << endl;
    cout << "Destination port: " << ntohs(uh->dp) << endl;
    cout << "Length: " << ntohs(uh->len) << endl;
    cout << "Checksum: " << ntohs(uh->checksum) << endl;
}



