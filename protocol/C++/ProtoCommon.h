 
#ifndef _PROTOCOMMON_H_ 
#define _PROTOCOMMON_H_ 
 
const unsigned short BEC_SUCCESS = 0; //成功 
const unsigned short BEC_UNKOWN_ERROR = 1; //未知错误 
const unsigned short BEC_INNER_ERROR = 2; //内部错误 
const unsigned short BEC_DB_ERROR = 3; //数据库错误 
const unsigned short BEC_PARAM_DENIED = 4; //非法参数 
const unsigned short BEC_PERMISSION_DENIED = 5; //权限非法 
const unsigned short BEC_SYSTEM_ERROR = 6; //系统错误 
const unsigned short BEC_INVALIDE_USERID = 50; //用户ID无效 
const unsigned short BEC_AUTH_USER_NOT_EXIST = 60; //认证错误:用户不存在 
const unsigned short BEC_AUTH_PASSWD_INCORRECT = 61; //认证错误:密码错误 
const unsigned short BEC_AUTH_ACCOUNT_INCORRECT = 62; //认证错误:角色名冲突(创建角色) 
const unsigned short BEC_AUTH_ING = 63; //认证错误:正在认证中... 
const unsigned short BEC_AUTH_AREADY_AUTH = 64; //认证错误:已认证 
const unsigned short BEC_AUTH_LIMITE_COUNT = 65; //认证错误:认证次数超过限制 
 
struct UserInfo //用户信息 
{ 
	unsigned long long uid;  
	std::string nickName; //用户昵称 
	unsigned int iconID; //头像 
	unsigned int level; //等级 
	unsigned int diamond; //当前剩余的充值钻石 
	unsigned int hisotryDiamond; //历史充值钻石总额 
	unsigned int giftDiamond; //当前剩余的赠送钻石 
	unsigned int joinTime; //加入时间 
	UserInfo() 
	{ 
		uid = 0; 
		iconID = 0; 
		level = 0; 
		diamond = 0; 
		hisotryDiamond = 0; 
		giftDiamond = 0; 
		joinTime = 0; 
	} 
}; 
inline zsummer::proto4z::WriteStream & operator << (zsummer::proto4z::WriteStream & ws, const UserInfo & data) 
{ 
	unsigned long long tag = 255ULL; 
	ws << (zsummer::proto4z::Integer)0; 
	zsummer::proto4z::Integer offset = ws.getStreamLen(); 
	ws << tag; 
	ws << data.uid; 
	ws << data.nickName; 
	ws << data.iconID; 
	ws << data.level; 
	ws << data.diamond; 
	ws << data.hisotryDiamond; 
	ws << data.giftDiamond; 
	ws << data.joinTime; 
	ws.fixOriginalData(offset - 4, ws.getStreamLen() - offset); 
	return ws; 
} 
inline zsummer::proto4z::ReadStream & operator >> (zsummer::proto4z::ReadStream & rs, UserInfo & data) 
{ 
	zsummer::proto4z::Integer sttLen = 0; 
	rs >> sttLen; 
	zsummer::proto4z::Integer cursor = rs.getStreamUnreadLen(); 
	unsigned long long tag = 0; 
	rs >> tag; 
	if ( (1ULL << 0) & tag) 
	{ 
		rs >> data.uid;  
	} 
	if ( (1ULL << 1) & tag) 
	{ 
		rs >> data.nickName;  
	} 
	if ( (1ULL << 2) & tag) 
	{ 
		rs >> data.iconID;  
	} 
	if ( (1ULL << 3) & tag) 
	{ 
		rs >> data.level;  
	} 
	if ( (1ULL << 4) & tag) 
	{ 
		rs >> data.diamond;  
	} 
	if ( (1ULL << 5) & tag) 
	{ 
		rs >> data.hisotryDiamond;  
	} 
	if ( (1ULL << 6) & tag) 
	{ 
		rs >> data.giftDiamond;  
	} 
	if ( (1ULL << 7) & tag) 
	{ 
		rs >> data.joinTime;  
	} 
	cursor = cursor - rs.getStreamUnreadLen(); 
	rs.skipOriginalData(sttLen - cursor); 
	return rs; 
} 
 
typedef std::vector<UserInfo> UserInfoVct;  
 
#endif 
