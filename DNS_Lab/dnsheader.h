/**
 *  @brief     DNS����ͷ
 *  @details   ��
 *  @author    ���
 *  @version   0.1
 *  @date      2020.07.11-2020.07.12
 */

#ifndef _DNSHEADER_H 
#define _DNSHEADER_H

#ifdef __cplusplus
extern "C" {
#endif

	//ID
	//�ɿͻ��������ò��ɷ��������ؽ�����ͻ�����ͨ������ȷ����Ӧ���ѯ�Ƿ�ƥ��
	//QR��0��ʾ��ѯ����1��ʾ��Ӧ����
	//OPCODE
	//ͨ��ֵΪ0����׼��ѯ��������ֵΪ1�������ѯ����2��������״̬���󣩡�
	//AA : Ȩ����(Authoritative answer)
	//TC : �ضϵ�(Truncated)
	//Ӧ����ܳ��ȳ�512�ֽ�ʱ��ֻ����ǰ512���ֽ�
	//RD : �����ݹ�(Recursion desired)
	//��ѯ�������ã���Ӧ���з���
	//�������ַ���������ݹ��ѯ�������λΪ0���ұ���������ַ�����û��һ��Ȩ���ش𣬾ͷ���һ���ܽ��ò�ѯ���������ַ������б����Ϊ������ѯ
	//RA���ݹ����(Recursion Available)
	//������ַ�����֧�ֵݹ��ѯ��������Ӧ�иñ�����Ϊ1
	//Z������Ϊ0�������ֶ�
	//RCODE : ��Ӧ��(Response coded)����������Ӧ��
	//ֵΪ0(û�в��)
	//ֵΪ3��ʾ���ֲ����Ȩ�����ַ��������أ���ʾ�ڲ�ѯ��ָ������������
	//QDCOUNT
	//Number of entries in the question section
	//ANCOUNT
	//Number of RRs in the answer section
	//NSCOUNT
	//Number of name server RRs in authority records section
	//ARCOUNT
	//Number of RRs in additional records section



	struct header {
		unsigned short ID;
		unsigned char QR : 1;
		unsigned char Opcode : 4;
		unsigned char AA : 1;
		unsigned char TC : 1;
		unsigned char RD : 1;
		unsigned char RA : 1;
		unsigned char Z : 3;
		unsigned char RCODE : 4;
		unsigned short QDCOUNT;
		unsigned short ANCOUNT;
		unsigned short NSCOUNT;
		unsigned short ARCOUNT;
	};

#ifdef __cplusplus
}
#endif

#endif // _DNSHEADER_H