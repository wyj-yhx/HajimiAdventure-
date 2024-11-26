#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef unsigned int uint;		//�޷�������

struct PDU
{
	uint uiPDULen;			//��Ϣ����
	uint uiPDUType;			//��Ϣ����
	char caData[64];		//��������
	int roomList[12] = { 0 };		//�����б�

	int room_id;			//��ǰ����
	int player_game_id = -1;	//��ǰ���ID
	int opposing_player = -1;	//������ҽ���

	uint uiMSGLen;			//��Ϣ����
	char strMsg[];		//��Ϣ����

};

PDU* mkPDU(uint msgLen)
{
	uint pduLen = sizeof(PDU) + msgLen;
	PDU* pdu = (PDU*)malloc(pduLen);
	if (pdu == nullptr)
	{
		exit(EXIT_FAILURE);
	}

	memset(pdu, 0, pduLen);		//��ʼ��,��ֹ�����ݣ��������
	pdu->uiPDULen = pduLen;
	pdu->uiMSGLen = msgLen;
	//pdu->strMsg.resize(msgLen);	//��ʼ���ַ�����С

	return pdu;
}


enum MSG_TYPE
{
	MIN = 0,			//��Сֵ
	LOGIN_SUCCESS,		//��¼�ɹ�

	GET_ROOM_LIST,			//��ȡ�����б�
	GET_ROOM_LIST_SUCCESS,	//��ȡ�����б�ɹ�

	GET_GAME_INFO,			//��ȡ��Ϸ��Ϣ
	GET_GAME_INFO_SUCCESS,	//��ȡ��Ϸ��Ϣ�ɹ�

	CREATE_ROOM,			//����
	CREATE_ROOM_SUCCESS,	//�����ɹ�
	CREATE_ROOM_FAIL,		//����ʧ��

	UPDATE_ROOM_LIST,			//���·����б�
    UPDATE_ROOM_LIST_SUCCESS,	//���·����б�ɹ�

	EXIT_ROOM,				//�˳�

	EXIT_GAME_LAN,				//�˳���Ϸ
    EXIT_GAME_LAN_SUCCESS,		//�˳���Ϸ�ɹ�

	CANCEL_ROOM,			//ȡ��
	CANCEL_ROOM_SUCCESS,	//ȡ���ɹ�

	ENTER_ROOM,				//����
	ENTER_ROOM_SUCCESS,		//����ɹ�

	JOIN_ROOM,				//����
	JOIN_ROOM_SUCCESS,		//����ɹ�


	MAX = 0x00FFFFFF		//���ֵ
};








#endif // PROTOCOL_H