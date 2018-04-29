/*
 * compress_and_encrypt.h
 *
 *  Created on: 2018-4-28
 *      Author: qianqians
 */
#ifndef _compress_and_encrypt_h
#define _compress_and_encrypt_h

#include <mutex>

#include <zlib.h>

namespace compress_and_encrypt {

#define max_buff_len 1024*1024
extern std::mutex c_and_e_mutex;
extern unsigned char c_and_e_output_buff[];
extern std::mutex e_and_c_mutex;
extern unsigned char e_and_c_output_buff[];

inline size_t compress_and_encrypt(unsigned char * input, size_t input_size, unsigned char xor_key)
{
	uLongf output_buff_size = max_buff_len;
	memset(c_and_e_output_buff, 0, output_buff_size);

	auto ret = compress((Bytef*)c_and_e_output_buff, &output_buff_size, (Bytef*)input, input_size);

	for (size_t i = 0; i < output_buff_size; i++)
	{
		c_and_e_output_buff[i] ^= xor_key;
	}

	return output_buff_size;
}

inline size_t encrypt_and_compress(unsigned char * input, size_t input_size, unsigned char xor_key)
{
	for (size_t i = 0; i < input_size; i++)
	{
		input[i] ^= xor_key;
	}

	uLongf output_buff_size = max_buff_len;
	memset(e_and_c_output_buff, 0, output_buff_size);

	uncompress((Bytef*)e_and_c_output_buff, &output_buff_size, (Bytef*)input, input_size);

	return output_buff_size;
}

} /* namespace compress_and_encrypt */

#endif //_compress_and_encrypt_h
