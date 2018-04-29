/*
 *  qianqians
 *  2014-10-5
 */
#include "compress_and_encrypt.h"

namespace compress_and_encrypt {

std::mutex c_and_e_mutex;
unsigned char c_and_e_output_buff[max_buff_len];

std::mutex e_and_c_mutex;
unsigned char e_and_c_output_buff[max_buff_len];

}