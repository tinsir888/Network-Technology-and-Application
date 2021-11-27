#define HAVE_REMOTE
#include <Winsock2.h>
#include "pcap.h"
#include <iostream>
#include <iomanip>
#include <cstdio>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)
#pragma warning(disable:6011)
using namespace std;
//�洢MAC��ַ�Ľṹ��
struct ethernet_header
{
	uint8_t ether_final[6];//Ŀ��MAC��ַ
	uint8_t ether_from[6];//ԴMAC��ַ
	uint16_t ether_type;
};
//�洢IP��ַ��У��͵Ľṹ��
struct ip_header
{
	uint8_t ip_header_length : 4,
		ip_version : 4;

	uint8_t ip_tos;
	uint16_t ip_length;
	uint16_t ip_checksum;//У����ֶ�
	struct in_addr  ip_source_address;//Դ��ַ
	struct in_addr  ip_destination_address;//Ŀ�ĵ�ַ
};
/* packet handler ���� */
/* ÿ�β������ݰ�ʱ��libpcap�����Զ���������ص����� */
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
	struct tm* ltime;
	struct ethernet_header* ethernet_protocol; /*��̫��Э�����*/
	struct ip_header* ip_protocol;/*ipЭ�����*/
	ip_protocol = (struct ip_header*)(pkt_data + 14); /*����ip���ݰ�������*/
	char timestr[16];
	time_t local_tv_sec;
	u_char* macsave;
	cout << "�������ݰ�!" << endl;
	/* ��ʱ���ת���ɿ�ʶ��ĸ�ʽ */
	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	cout << "����ʱ��:    ";
	strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);
	cout << timestr << endl;
	cout << "���ݰ�����:  " << header->len << "�ֽ�" << endl;
	ethernet_protocol = (struct ethernet_header*)pkt_data;
	/*ԴMAC��ַ*/
	macsave = ethernet_protocol->ether_from;
	cout << "ԴMAC��ַ:   ";
	printf("%02x:%02x:%02x:%02x:%02x:%02x", *macsave, *(macsave + 1), *(macsave + 2), *(macsave + 3), *(macsave + 4), *(macsave + 5));//�������ԣ�cout�������ֵ�bug������Ҳ��֧�ֵ�printf����ʾ
	/*Ŀ��MAC��ַ*/
	cout << endl;
	macsave = ethernet_protocol->ether_final;
	cout << "Ŀ��MAC��ַ: ";
	printf("%02x:%02x:%02x:%02x:%02x:%02x", *macsave, *(macsave + 1), *(macsave + 2), *(macsave + 3), *(macsave + 4), *(macsave + 5));
	cout << endl;
	/*Դip��ַ*/
	cout << "ԴIP��ַ:    " << inet_ntoa(ip_protocol->ip_source_address) << endl;
	/*Ŀ��ip��ַ*/
	cout << "Ŀ��IP��ַ:  " << inet_ntoa(ip_protocol->ip_destination_address) << endl;
	//cout << "֡���ͣ�" << "��̫��֡" << endl;
	/*У����ֶ�*/
	cout << "У����ֶ�:  " << ip_protocol -> ip_checksum << endl;
	cout << endl << endl;
}
int main()
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	pcap_addr_t* a;
	pcap_t* adhandle;
	int inum;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];
	cout << "Start!" << endl << "��ʼɨ��˿ڣ�" << endl;
	//ɨ�����ж˿ڲ�չʾ
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING,
		NULL,
		&alldevs,
		errbuf
	) == -1)
	{
		cout << "error!" << endl << "��ȡ�˿ڴ���";
		exit(1);
	}
	for (d = alldevs; d != NULL; d = d->next)
	{
		cout << ++i << " " << d->name << endl;
		if (d->description)
		{
			cout << d->description << endl;
		}
		else
		{
			cout << "No description available" << endl << "û�п��õ�������" << endl;
		}
	}
	if (i == 0)
	{
		cout << "Check Winpcap" << endl << "û���ҵ��˿ڣ����� NPcap��" << endl;
		return -1;
	}
	cout << "Enter the interface number:(range:1-" << i << ")" << endl << "���������Ķ˿ں�:����Χ��1-" << i << "��" << endl;
	cin >> inum;
	if (inum<1 || inum>i)
	{
		cout << "Interface number out of range!" << endl << "�˿ںŲ�����ȷ��Χ�ڣ�" << endl;
		pcap_freealldevs(alldevs);
		return -1;
	}
	/* ��ת��ѡ�е������� */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	if ((adhandle = pcap_open(d->name,
		65535,
		PCAP_OPENFLAG_PROMISCUOUS,
		1000,
		NULL,
		errbuf
	)) == NULL)
	{
		cout << stderr << endl << "Unable to open the adapter, it is not supported by NPcap" << endl << "�޷��򿪣������Ƿ��ܵ� NPcap ֧�֣�" << d->name;
		pcap_freealldevs(alldevs);
		return -1;
	}
	cout << "listening on : " << d->description << endl;
	pcap_freealldevs(alldevs);
	int pnum = 0;
	cout << "Enter the number of data package needed!" << endl << "���벶�����ݰ�����" << endl;
	cin >> pnum;
	//main��ͨ������pcap_loop������pnum�����ݰ�
	pcap_loop(adhandle, pnum, packet_handler, NULL);
	cout << "Packages caputuring finished!" << endl << "���ݱ����������" << endl;
	system("pause");
	return 0;
}