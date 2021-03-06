﻿
/*
* breeze License
* Copyright (C) 2014 YaweiZhang <yawei_zhang@foxmail.com>.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#include "dbHelper.h"
using namespace zsummer::mysql;
using namespace zsummer::log4z;


const std::string& DBResult::extractOneField()
{
	if (_curIter == _result.end())
	{
		throw std::runtime_error("result have not any row.");
	}
	if (_fieldCursor >= _curIter->size())
	{
		throw std::runtime_error("result read field over");
	}
	
	const std::string & field = _curIter->at(_fieldCursor);
	_fieldCursor++;
	if (_fieldCursor == _curIter->size())
	{
		_curIter++;
		_fieldCursor = 0;
	}
	return field;
}


void DBResult::_setQueryResult(QueryErrorCode qec, const std::string & sql, MYSQL * db)
{
	_ec = qec;
	_sql = sql;
	if (qec == QueryErrorCode::QEC_MYSQLERROR)
	{
		_lastErrorMsg = "[MYSQL_ERRNO:";
		_lastErrorMsg += _toString(mysql_errno(db));
		_lastErrorMsg += "]";
		const char * msg = mysql_error(db);
		_lastErrorMsg += msg == NULL ? "" : msg;
	}
	else if (qec == QueryErrorCode::QEC_SUCCESS)
	{
		_affectedRows = mysql_affected_rows(db);
		MYSQL_RES * res = mysql_store_result(db);
		if (res)
		{
			unsigned int fieldCount = mysql_num_fields(res);
			MYSQL_FIELD * fieldDesc = mysql_fetch_field(res);
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(res)) != NULL)
			{
				_result.push_back(std::vector<std::string>());
				auto & vct = _result.back();
				vct.reserve(fieldCount);

				unsigned long * fieldLength = mysql_fetch_lengths(res);
				for (unsigned int i = 0; i < fieldCount; ++i)
				{
					if (fieldDesc[i].type == MYSQL_TYPE_TINY_BLOB
						|| fieldDesc[i].type == MYSQL_TYPE_MEDIUM_BLOB
						|| fieldDesc[i].type == MYSQL_TYPE_LONG_BLOB
						|| fieldDesc[i].type == MYSQL_TYPE_BLOB)
					{
						vct.push_back(std::string(row[i], fieldLength[i]));
					}
					else
					{
						vct.push_back(row[i] == nullptr ? "" : row[i]);
					}
				}
			}
			mysql_free_result(res);
			res = nullptr;
		}
		while (!mysql_next_result(db))
		{
			res = mysql_store_result(db);
			mysql_free_result(res);
			res = nullptr;
			//LOGW("db have other result (multi-result).");
		}
		
	}
	else
	{
		LOGA("db have error.");
	}
	_curIter = _result.begin();
	_fieldCursor = 0;
}


bool DBHelper::connect()
{
	if (_mysql)
	{
		mysql_close(_mysql);
		_mysql = nullptr;
	}
	_mysql = mysql_init(nullptr);
	if (!_mysql)
	{
		LOGE("mysql_init false. mysql ip = " << _ip << ", _port = " << _port << ", _user = " << _user << ", _pwd = " << _pwd << ", _db=" << _db );
		return false;
	}
	mysql_options(_mysql, MYSQL_OPT_CONNECT_TIMEOUT, "5");
	mysql_options(_mysql, MYSQL_SET_CHARSET_NAME, "UTF8");
	mysql_set_character_set(_mysql, "UTF8");
	MYSQL * ret = mysql_real_connect(_mysql, _ip.c_str(), _user.c_str(), _pwd.c_str(), _db.c_str(), _port, nullptr, 0);
	if (!ret)
	{
		return false;
	}
	return true;
}

bool DBHelper::waitEnable()
{
	if (!_waiting)
	{
		return false;
	}
	size_t lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	do
	{
		LOGE("try reconnect mysql. mysql=" << _mysql);
		if (connect())
		{
			return true;
		}
		size_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		if (now - lastTime < 5000)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(now - lastTime));
		}
		lastTime += now - lastTime;
	} while (_waiting);
	return false;
}

DBResultPtr DBHelper::query(const std::string & sql)
{
	auto ret = std::make_shared<DBResult>();
	if (_mysql == nullptr)
	{
		return ret;
	}
	if (mysql_real_query(_mysql, sql.c_str(), (unsigned long)sql.length()) != 0)
	{
		//retry
		if (mysql_errno(_mysql) == CR_SERVER_LOST || mysql_errno(_mysql) == CR_SERVER_GONE_ERROR)
		{
			if (waitEnable())
			{
				if (mysql_real_query(_mysql, sql.c_str(), (unsigned long)sql.length()) == 0)
				{
					ret->_setQueryResult(QueryErrorCode::QEC_SUCCESS, sql, _mysql);
					return ret;
				}
			}
		}
		ret->_setQueryResult(QueryErrorCode::QEC_MYSQLERROR, sql, _mysql);
		return ret;
	}
	ret->_setQueryResult(QueryErrorCode::QEC_SUCCESS, sql, _mysql);
	return ret;
}

















