#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef unsigned int uint;		//无符号整形

struct PDU
{
	uint uiPDULen;			//消息长度
	uint uiPDUType;			//消息类型
	char caData[64];		//发送数据
	int roomList[12] = { 0 };		//房间列表

	int room_id;			//当前房间
	int player_game_id = -1;	//当前玩家ID
	int opposing_player = -1;	//对手玩家进度

	uint uiMSGLen;			//消息长度
	char strMsg[];		//消息内容

};

PDU* mkPDU(uint msgLen)
{
	uint pduLen = sizeof(PDU) + msgLen;
	PDU* pdu = (PDU*)malloc(pduLen);
	if (pdu == nullptr)
	{
		exit(EXIT_FAILURE);
	}

	memset(pdu, 0, pduLen);		//初始化,防止脏数据，清空数据
	pdu->uiPDULen = pduLen;
	pdu->uiMSGLen = msgLen;
	//pdu->strMsg.resize(msgLen);	//初始化字符串大小

	return pdu;
}


enum MSG_TYPE
{
	MIN = 0,			//最小值
	LOGIN_SUCCESS,		//登录成功

	GET_ROOM_LIST,			//获取房间列表
	GET_ROOM_LIST_SUCCESS,	//获取房间列表成功

	GET_GAME_INFO,			//获取游戏信息
	GET_GAME_INFO_SUCCESS,	//获取游戏信息成功

	CREATE_ROOM,			//创建
	CREATE_ROOM_SUCCESS,	//创建成功
	CREATE_ROOM_FAIL,		//创建失败

	UPDATE_ROOM_LIST,			//更新房间列表
    UPDATE_ROOM_LIST_SUCCESS,	//更新房间列表成功

	EXIT_ROOM,				//退出

	EXIT_GAME_LAN,				//退出游戏
    EXIT_GAME_LAN_SUCCESS,		//退出游戏成功

	CANCEL_ROOM,			//取消
	CANCEL_ROOM_SUCCESS,	//取消成功

	ENTER_ROOM,				//进入
	ENTER_ROOM_SUCCESS,		//进入成功

	JOIN_ROOM,				//加入
	JOIN_ROOM_SUCCESS,		//加入成功


	MAX = 0x00FFFFFF		//最大值
};








#endif // PROTOCOL_H