
#ifndef _channel_h
#define _channel_h

#include "Json.h"
#include "Sockets.h"

namespace service
{

class channel {
public:
	channel(TSharedPtr<FSocket> _s)
	{
		s = _s;

		buff_size = 1024 * 1024 * 16;
		buff_offset = 0;
		buff = new uint8[buff_size];
	}

	void recv(const uint8 * data, int32 size)
	{
		if ( (buff_offset + size) > buff_size)
		{
			buff_size *= 2;
			auto new_buff = new uint8[buff_size];
			memcpy(new_buff, buff, buff_offset);
			delete buff;
			buff = new_buff;
		}
		memcpy(buff + buff_offset, data, size);

		int32 tmp_buff_len = buff_offset + size;
		int32 tmp_buff_offset = 0;
		while (tmp_buff_len > (tmp_buff_offset + 4))
		{
			auto tmp_buff = buff + tmp_buff_offset;
			int len = (int)tmp_buff[0] | ((int)tmp_buff[1] << 8) | ((int)tmp_buff[2] << 16) | ((int)tmp_buff[3] << 24);

			if ( (len + tmp_buff_offset + 4) <= tmp_buff_len )
			{
				auto reader = TJsonReaderFactory<TCHAR>::Create(FString((char*)(&tmp_buff[4])));
				TArray< TSharedPtr<FJsonValue> > OutArray;
				if (FJsonSerializer::Deserialize(reader, OutArray))
				{
					que.Enqueue(OutArray);
				}

				tmp_buff_offset += len + 4;
			}
			else
			{
				memcpy(buff, &buff[tmp_buff_offset], tmp_buff_len - tmp_buff_offset);
				buff_offset = tmp_buff_len - tmp_buff_offset;
				tmp_buff_offset = tmp_buff_len;
			}
		}

		if (tmp_buff_len > tmp_buff_offset)
		{
			memcpy(buff, &buff[tmp_buff_offset], tmp_buff_len - tmp_buff_offset);
			buff_offset = tmp_buff_len - tmp_buff_offset;
			tmp_buff_offset = tmp_buff_len;
		}
	}

	bool pop(TArray< TSharedPtr<FJsonValue> > & OutArray)
	{
		return que.Dequeue(OutArray);
	}

	void push(const TArray< TSharedPtr<FJsonValue> >& InArray)
	{
		FString OutputString;
		auto write = TJsonWriterFactory<TCHAR>::Create(&OutputString);
		if (FJsonSerializer::Serialize(InArray, write))
		{
			char * buff = TCHAR_TO_ANSI(*OutputString);
			int len = strlen(buff) + 1;

			int send_len = 0;
			while (send_len < len)
			{
				int tmp = 0;
				if (s->Send(&buff[send_len], len - send_len, tmp))
				{
					send_len += tmp;
				}
			}
		}
	}

private:
	TQueue< TArray< TSharedPtr<FJsonValue> > > que;

	TSharedPtr<FSocket> s;

	uint8 * buff;
	int32 buff_size;
	int32 buff_offset;

};

}

#endif
