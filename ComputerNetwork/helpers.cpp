//
// Created by LuoRun on 22/06/1.
//

#include "helpers.h"
#include "utils.h"

#include <map>
#include <iostream>
#include <iomanip>
#include<stdio.h>
using namespace std;
#define _CRT_SECURE_NO_WARNINGS
map<string, int> ip_count;

void ip_counter(const string &ip_str) { ++ ip_count[ip_str];}

void ip_counter(string &&ip_str) { ip_counter(ip_str); }

void helpers::protocol_helper_callback(const defs::ipv4_header *header) {
    FILE* fp = NULL;

    fopen_s(&fp,"result.txt", "a+");
    fprintf(fp, converter::ipv4_to_string(header->src).data());
    fprintf(fp, ",");
    fprintf(fp, converter::ipv4_to_string(header->des).data());
    fprintf(fp, "\n");
    fclose(fp);
    ip_counter(converter::ipv4_to_string(header->src));
}

void helpers::protocol_helper_callback(const defs::ipv6_header *header) {
    ip_counter(converter::ipv6_to_string(header->src));
}

void helpers::protocol_helper_callback(const defs::arp_header *header) {
    ip_counter(converter::ipv4_to_string(header->src_ip));
}

void helpers::output_ip_counts(int width) {
    cout << "IP" << setfill(' ') << setw(width) << "Flow" << endl;
    for (const auto j : ip_count)
        cout << j.first << setw(int(width - j.first.length())) << j.second << endl;
}


