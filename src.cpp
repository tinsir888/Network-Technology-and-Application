#define HAVE_REMOTE
#define WINDOWS_IGNORE_PACKING_MISMATCH
#pragma pack(1)
#include <Winsock2.h>
#include "pcap.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <iomanip>
#pragma comment(lib,"ws2_32.lib")//�ر�
#pragma warning(disable:4996)
#pragma warning(disable:6011)
using namespace std;
#pragma pack(1)//�ο��̿��飬����֡ͷ����ARP֡�ṹ
typedef struct FrameHeader_t//֡�ײ�
{
	BYTE DesMAC[6];  //Ŀ�ĵ�ַ
	BYTE SrcMAC[6];  //Դ��ַ
	WORD FrameType;  //֡����
}FrameHeader_t;
typedef struct ARPFrame_t//�ײ�
{
	FrameHeader_t FrameHeader;
	WORD HardwareType;//Ӳ������
	WORD ProtocolType;//Э������
	BYTE HLen;//Ӳ����ַ����
	BYTE PLen;//Э���ַ����
	WORD Operation;//��������
	BYTE SendHa[6];//���ͷ�MAC��ַ
	DWORD SendIP;//���ͷ�IP��ַ
	BYTE RecvHa[6];//���շ�MAC��ַ
	DWORD RecvIP;//���շ�IP��ַ
}ARPFrame_t;
#pragma pack()
int pMACaddr(BYTE MACaddr[6])//���MAC��ַ
{
	int i = 0;
	while (i <= 5)
	{
		cout << setw(2) << setfill('0') << hex << (int)MACaddr[i];
		if (i != 5)
			cout << "-";
		else
			cout << endl;
		i++;
	}
	return i;
}
int pIPaddr(DWORD IPaddr)//���IP��ַ
{
	BYTE* p = (BYTE*)&IPaddr;
	int i = 0;
	while (i <= 3)
	{
		cout << dec << (int)*p;

		if (i != 3)
			cout << ".";
		else
			cout << endl;
		p++;
		i++;
	}
	return i;
}
int pARPframe(ARPFrame_t* IPPacket)//���ARP֡
{
	cout << "���β���õ���ARP֡�������£�" << endl;
	cout << "Ŀ��MAC��ַ��"<< endl;
	pMACaddr(IPPacket->FrameHeader.DesMAC);
	cout << "ԴMAC��ַ��" << endl;
	pMACaddr(IPPacket->FrameHeader.SrcMAC);
	//ntoh():��һ��16λ���������ֽ�˳��ת��Ϊ�����ֽ�˳��
	cout << "֡����: " << hex << ntohs(IPPacket->FrameHeader.FrameType) << endl;
	cout << "Ӳ������: " << hex << ntohs(IPPacket->HardwareType) << endl;
	cout << "Э������: " << hex << ntohs(IPPacket->ProtocolType) << endl;
	cout << "Ӳ����ַ����: " << hex << (int)IPPacket->HLen << endl;
	cout << "Э���ַ����: " << hex << (int)IPPacket->PLen << endl;
	cout << "��������: " << hex << ntohs(IPPacket->Operation) << endl;
	//Operation ��ʾ������ĵ����ͣ�ARP ����Ϊ 1��ARP ��ӦΪ 2��RARP ����Ϊ 3��RARP ��ӦΪ 4��
	cout << "���Ͷ� MAC ��ַ: ";
	pMACaddr(IPPacket->SendHa);
	cout << "���Ͷ� IP ��ַ: ";
	pIPaddr(IPPacket->SendIP);
	cout << "Ŀ�Ķ� MAC ��ַ: ";
	pMACaddr(IPPacket->RecvHa);
	cout << "Ŀ�Ķ� IP ��ַ: ";
	pIPaddr(IPPacket->RecvIP);
	return 0;
}
int main()
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	pcap_addr_t* a;
	pcap_t* adhandle;
	BYTE* IP;
	char errbuf[PCAP_ERRBUF_SIZE];
	ARPFrame_t ARPFrame;
	ARPFrame_t* IPPacket;
	DWORD SIP, ReIP, MIP;
	u_int netmask;
	//��ȡ�豸�б�������������д�����
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING,
		NULL,
		&alldevs,
		errbuf)
		== -1)
	{
		cout << "��ȡ�豸�б�ʱ��������" <<errbuf<< endl;
		throw -1;
	}
	int i = 0,inum;
	//�ο��ϴ�ʵ����д�Ĵ��豸�˿ڴ��룬�Դ�����д�����
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
		a = d->addresses;
		while (a != NULL) //��Ե�һ�����飬�������IP��ַ�����룬�㲥��ַ�Ĵ���
		{
			if (a->addr->sa_family == AF_INET)
			{
				cout << "  IP��ַ: " << inet_ntoa(((struct sockaddr_in*)(a->addr))->sin_addr) << endl;
				//cout << "  ��������: " << inet_ntoa(((struct sockaddr_in*)(a->netmask))->sin_addr) << endl;
				//cout << "  �㲥��ַ: " << inet_ntoa(((struct sockaddr_in*)(a->broadaddr))->sin_addr) << endl;
			}
			a = a->next;
		}
	}
	if (i == 0)
	{
		cout << "Check NPcap" << endl << "û���ҵ��˿ڣ�����NPcap��" << endl;
		throw -2;
	}
	cout << "Enter the interface number:(range:1-" << i << ")" << endl << "���������Ķ˿ں�:����Χ��1-" << i << "��" << endl;
	cin >> inum;
	if (inum<1 || inum>i)
	{
		cout << "Interface number out of range!" << endl << "�˿ںŲ�����ȷ��Χ�ڣ�" << endl;
		pcap_freealldevs(alldevs);
		throw -3;
	}
	/* ������һ��ʵ����룬��ת��ѡ�е������� */

	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	if ((adhandle = pcap_open(d->name,
		65535,
		PCAP_OPENFLAG_PROMISCUOUS,//����Ϊ����ģʽ
		1000,
		NULL,
		errbuf
	)) == NULL)
	{
		cout << stderr << endl << "Unable to open the adapter, maybe it is not supported by WinPcap" << endl << "�޷��򿪣������Ƿ��ܵ�NPcap֧�֣�" << d->name;
		pcap_freealldevs(alldevs);
		throw -4;
	}
	cout <<"����˿ڣ�" <<inum<<" "<< d->description << endl;

	//��д����������ʹ��ֻ����ARP���ݰ����Դ�����д�����
	netmask = ((sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	struct bpf_program fcode;
	char packet_filter[] = "ether proto \\arp"; 
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		cout << "�������ݰ�������ʱ������������������﷨" << endl;
		pcap_freealldevs(alldevs);
		throw -5;
	}
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		cout << "���ù�����ʱ��������"<<endl;
		pcap_freealldevs(alldevs);
		throw -6;
	}
	//Ԥ����Ҫ���͵� ARP ���ݱ�
	for (i = 0; i < 6; i++)
	{
		ARPFrame.FrameHeader.DesMAC[i] = 0xFF;//��ʾ�㲥
		ARPFrame.FrameHeader.SrcMAC[i] = 0x00;//����Ϊ���� MAC ��ַ
		ARPFrame.RecvHa[i] = 0;//��0
		ARPFrame.SendHa[i] = 0x11;//����Ϊ���� MAC ��ַ
	}
	ARPFrame.FrameHeader.FrameType = htons(0x0806);//֡����ΪARP
	ARPFrame.HardwareType = htons(0x0001);//Ӳ������Ϊ��̫��
	ARPFrame.ProtocolType = htons(0x0800);//Э������ΪIP
	ARPFrame.HLen = 6;//Ӳ����ַ����Ϊ6
	ARPFrame.PLen = 4;//Э���ַ��Ϊ4
	ARPFrame.Operation = htons(0x0001);//����ΪARP����
	SIP = ARPFrame.SendIP = htonl(0x00000000);//����Ϊ���� IP ��ַ
	//����ѡ��������� IP ����Ϊ����� IP ��ַ
	for (a = d->addresses; a != NULL; a = a->next)
	{
		if (a->addr->sa_family == AF_INET)
		{
			ReIP = ARPFrame.RecvIP = inet_addr(inet_ntoa(((struct sockaddr_in*)(a->addr))->sin_addr));
		}
	}
	struct pcap_pkthdr* adhandleheader;
	const u_char* adhandledata;
	int tjdg = 0;
	//����ARPFrame�е����ݣ����ĳ���Ϊsizeof(ARPFrame_t)��������ͳɹ�������0
	if (pcap_sendpacket(adhandle, (u_char*)&ARPFrame, sizeof(ARPFrame_t)) != 0)
	{
		cout << "ARP���ݰ�����ʧ�ܣ�" << endl;
		pcap_freealldevs(alldevs);
		throw -7;
	}
	else
	{
		cout << "ARP���ݰ����ͳɹ���" << endl;
get_local_mac:	
		int jdg_catch_re_arp_p = pcap_next_ex(adhandle, &adhandleheader, &adhandledata);
		if (jdg_catch_re_arp_p == -1)
		{
			cout << "����ARP�������ݰ�ʱ��������" << endl;
			pcap_freealldevs(alldevs);
			throw -8;
		}
		else if (jdg_catch_re_arp_p == 0)
			{
				cout << "��δ������ݱ������Ժ�" << endl;
				cout << "�ѳ��Դ���: " << ++tjdg << endl;
				if (tjdg > 20)
				{
					cout << "�Ѷ�γ��Խ��գ���ȷ�϶˿��Ƿ�����" << endl;
					pcap_freealldevs(alldevs);
					throw -9;
				}				
				goto get_local_mac;
			}
			else
			{
				IPPacket = (ARPFrame_t*)adhandledata;
				if(SIP==IPPacket->SendIP)
					if (ReIP == IPPacket->RecvIP)
					{
						cout << "ȷ��������" << endl;
						goto get_local_mac;
					}
				if(SIP == IPPacket->RecvIP && ReIP == IPPacket->SendIP)
					{
						cout << "�ɹ���ȡ�ظ������ݱ���" << endl; 
						pARPframe(IPPacket);
						cout << endl;
						cout << "��ȡ������IP��ַ��MAC��ַ�Ķ�Ӧ��ϵ���£�" << endl << "IP��";
						pIPaddr(IPPacket->SendIP);
						cout << "MAC: ";
						pMACaddr(IPPacket->SendHa);
						cout << endl;
					}
				else goto get_local_mac;
			}
	}
	cout << endl;
	cout << "�����緢�����ݰ�" << endl;
	char pip[16];
	cout << "������Ŀ��IP��ַ" << endl;
	cin >> pip;
	ReIP = ARPFrame.RecvIP = inet_addr(pip);
	SIP = ARPFrame.SendIP =	IPPacket->SendIP;
	for (i = 0; i < 6; i++)
	{
		//������IP���뱨��
		ARPFrame.SendHa[i] = ARPFrame.FrameHeader.SrcMAC[i] = IPPacket->SendHa[i];
	}
	if (pcap_sendpacket(adhandle, (u_char*)&ARPFrame, sizeof(ARPFrame_t)) != 0)
	{
		cout << "ARP���ݰ�����ʧ�ܣ�" << endl;
		pcap_freealldevs(alldevs);
		throw -10;
	}
	else
	{
		cout << "ARP���ݰ����ͳɹ���" << endl; 
		inum = 0;
get_distant_mac:
		int jdg_catch_re_arp_p = pcap_next_ex(adhandle, &adhandleheader, &adhandledata);
		if (jdg_catch_re_arp_p == -1)
		{
			cout << "����ARP�������ݰ�ʱ��������" << endl;
			pcap_freealldevs(alldevs);
			throw -11;
		}
		else
			if (jdg_catch_re_arp_p == 0)
			{
				cout << "��δ������ݱ������Ժ�" << endl;
				cout << "�ѳ��Դ�����" <<dec<< ++inum << endl;
				if (inum > 20)
				{
					cout << "�Ѷ�γ��Խ��գ���ȷ�϶˿��Ƿ�����" << endl;
					pcap_freealldevs(alldevs);
					throw -12;
				}
				goto get_distant_mac;
			}
			else
			{
				IPPacket = (ARPFrame_t*)adhandledata;
				if (SIP == IPPacket->SendIP)
					if (ReIP == IPPacket->RecvIP)
					{
						cout << "ȷ�Ϸ���������" << endl;
						goto get_distant_mac;
					}
				//�յ��˻�Ӧ
				if (SIP == IPPacket->RecvIP && ReIP == IPPacket->SendIP)
				{
					cout << "�ɹ���ȡ�ظ������ݱ���" << endl;
					pARPframe(IPPacket);
					cout << endl;
					cout << "��ȡ��IP��ַ��MAC��ַ�Ķ�Ӧ��ϵ���£�" << endl << "IP��";
					pIPaddr(IPPacket->SendIP);
					cout << "MAC: ";
					pMACaddr(IPPacket->SendHa);
					cout << endl;
				}
				else goto get_distant_mac;
			}
	}
	return 0;
}