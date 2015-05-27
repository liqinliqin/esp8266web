/*
 * web_utils.c
 *
 *  Created on: 25 ���. 2014 �.
 *      Author: PV`
 */
#include "user_config.h"
#include "bios.h"
#include "add_sdk_func.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "user_interface.h"


#define mMIN(a, b)  ((a<b)?a:b)
/******************************************************************************
 * copy_align4
 * �������� ������ �� ������� ����������� flash � �.�.
*******************************************************************************/
void copy_align4(void *ptrd, void *ptrs, uint32 len)
{
	union {
		uint8 uc[4];
		uint32 ud;
	}tmp;
	uint8 *pd = ptrd;
	uint32 *p = (uint32 *)((uint32)ptrs & (~3));
	uint32 xlen = ((uint32)ptrs) & 3;
	if(xlen) {
		if(((uint32)p >= 0x20000000)&&(((uint32)p < 0x60002000)||((uint32)p >= 0x60008000))) tmp.ud = *p++;
		else {
			tmp.ud = 0;
			p++;
		}
		while (len)  {
			*pd++ = tmp.uc[xlen++];
			len--;
			if(xlen >= 4) break;
		}
	}
	xlen = len >> 2;
	while(xlen) {
		if(((uint32)p >= 0x20000000)&&(((uint32)p < 0x60002000)||((uint32)p >= 0x60008000))) tmp.ud = *p++;
		else {
			tmp.ud = 0;
			p++;
		}
		*pd++ = tmp.uc[0];
		*pd++ = tmp.uc[1];
		*pd++ = tmp.uc[2];
		*pd++ = tmp.uc[3];
		xlen--;
	}
	len &= 3;
	if(len) {
		if(((uint32)p >= 0x20000000)&&(((uint32)p < 0x60002000)||((uint32)p >= 0x60008000))) tmp.ud = *p;
		else tmp.ud = 0;
		uint8 * ptmp = tmp.uc;
		while (len--)  *pd++ = *ptmp++;
	}
}
/******************************************************************************
 * FunctionName : hextoul
*******************************************************************************/
// bool conv_str_hex(uint32 * dest, uint8 *s);
uint32 ICACHE_FLASH_ATTR hextoul(uint8 *s)
{
/*
	uint32 val;
	if(!conv_str_hex(&val, s)) return 0;
	return val;
*/
	uint32 val = 0;
          while (*s)
          {
               if (*s >= '0' && *s <= '9')
               {
                 val <<= 4;
                 val |= *s - '0';
               }
               else if (*s >= 'A' && *s <= 'F')
               {
                 val <<= 4;
                 val |= *s - 'A' + 10;
               }
               else if (*s >= 'a' && *s <= 'f')
               {
                 val <<= 4;
                 val |= *s - 'a' + 10;
               }
               else break;
               s++;
          };
          return val;
}
/******************************************************************************
 * FunctionName : ahextoul
*******************************************************************************/
// bool convert_para_str(uint32 * dest, uint8 *s);
uint32 ICACHE_FLASH_ATTR ahextoul(uint8 *s)
{
/*
	uint32 ret;
	if(!convert_para_str(&ret, s)) return 0;
	return ret;
*/
	if((s[0]=='0') && ((s[1] | 0x20) =='x')) return hextoul(s+2);
	return atoi(s);
}
/******************************************************************************
 * FunctionName : cmpcpystr
 * Description  : �������� ����� �� ������ ������ � ��������� ��������� ��������
 *                � �������� ������������. ���������� � ��������� ������ �� ��������, ���� ������.
 * Parameters   : ��� ������� ���������� ������� = '\0' ������� ����� ������ (>' ').
                  �������� �� ������� <' ' ��� �����������.
                  �������� ����������� ������� ������ ��� ����������� ����� (� ������������ � ����� '\0'!).
 * Returns      : ������� �� �������� �����������, ��������� �� ���������� � ������,
                  ���� ���������� ������.
                  ���� NULL, �� ��������� ��� �������� ���������� �� ������.
*******************************************************************************/
uint8 * ICACHE_FLASH_ATTR cmpcpystr(uint8 *pbuf, uint8 *pstr, uint8 a, uint8 b, uint16 len)
{
            if(len == 0) pbuf = NULL;
            if(pstr == NULL) {
              if(pbuf != NULL) *pbuf='\0';
              return NULL;
            };
            uint8 c;
            do {
              c = *pstr;
              if(c < ' ') { // ������ ���������
                if(pbuf != NULL) *pbuf='\0';
                return NULL; // id �� ������
              };
              if((a == '\0')&&(c > ' ')) break; // �� ����� -> ����� ������
              pstr++;
              if(c == a) break; // ����� ��������� ������ (������������ � �����)
            }while(1);
            if(pbuf != NULL) {
              while(len--) {
                c = *pstr;
                if(c == b) { // ����� ������������� ������ (������������ � �����)
                  *pbuf='\0';
                  return pstr; // �������� ���������� ������
                };
//                if(c <= ' ') { // ������ ��������� ��� ������
                if(c < ' ') { // ������ ��������� ��� ������
                  *pbuf='\0';
                  return NULL; // �������� ���������� �� ������
                };
                pstr++;
                *pbuf++ = c;
              };
              *--pbuf='\0'; // ������� �����
            };
            do {
              c = *pstr;
              if(c == b) return pstr; // ����� ������������� ������
//              if(c <= ' ') return NULL; // ������ ���������
              if(c < ' ') return NULL; // ������ ���������
              pstr++;
            }while(1);
}
/******************************************************************************
 * FunctionName : strtoip (192.168.4.1 = 0x01040A8C0)
*******************************************************************************/
/* uint32 ICACHE_FLASH_ATTR strtoip(uint8 *s)
{
	uint32 val = 0;
	uint8 pbuf[4];
	s = cmpcpystr(pbuf, s, '\0', '.', 4);
	val = atoi(pbuf)&0xFF;
	s = cmpcpystr(pbuf, s, '.', '.', 4);
	val += (atoi(pbuf)&0xFF) << 8;
	s = cmpcpystr(pbuf, s, '.', '.', 4);
	val += (atoi(pbuf)&0xFF) << 16;
	s = cmpcpystr(pbuf, s, '.', ' ', 4);
	val += (atoi(pbuf)&0xFF) << 24;
	return val;
} */
/******************************************************************************
 * FunctionName : strtmac
*******************************************************************************/
void ICACHE_FLASH_ATTR strtomac(uint8 *s, uint8 *macaddr)
{
	uint8 pbuf[4];
	s = cmpcpystr(pbuf, s, 0, ':', 3);
	*macaddr++ = hextoul(pbuf);
	int i = 4;
	while(i--) {
		s = cmpcpystr(pbuf, s, ':', ':', 3);
		*macaddr++ = hextoul(pbuf);
	}
	s = cmpcpystr(pbuf, s, ':', ' ', 3);
	*macaddr++ = hextoul(pbuf);
}
/******************************************************************************
 * FunctionName : urldecode
*******************************************************************************/
int ICACHE_FLASH_ATTR urldecode(uint8 *d, uint8 *s, uint16 lend, uint16 lens)
{
	uint16 ret = 0;
	if(s != NULL) while ((lens--) && (lend--) && (*s > ' ')) {
		if ((*s == '%')&&(lens > 1)) {
			s++;
			int i = 2;
			uint8 val = 0;
			while(i--) {
				if (*s >= '0' && *s <= '9') {
					val <<= 4;
					val |= *s - '0';
				} else if (*s >= 'A' && *s <= 'F') {
					val <<= 4;
					val |= *s - 'A' + 10;
				} else if (*s >= 'a' && *s <= 'f') {
					val <<= 4;
					val |= *s - 'a' + 10;
				} else
					break;
				s++;
				lens--;
			};
			s--;
			*d++ = val;
		} else if (*s == '+')
			*d++ = ' ';
		else
			*d++ = *s;
		ret++;
		s++;
	}
	*d = '\0';
	return ret;
}
/******************************************************************************
 * FunctionName : urlencode
*******************************************************************************/
/*int ICACHE_FLASH_ATTR urlencode(uint8 *d, uint8 *s, uint16 lend, uint16 lens)
{
	uint16 ret = 0;
	if(s != NULL) while ((lens--) && (lend--) && (*s != '\0')) {
		if ( (48 <= *s && *s <= 57) //0-9
				|| (65 <= *s && *s <= 90) //abc...xyz
				|| (97 <= *s && *s <= 122) //ABC...XYZ
				|| (*s == '~' || *s == '!' || *s == '*' || *s == '(' || *s == ')' || *s == '\'')) {
			*d++ = *s++;
			ret++;
		} else {
			if(lend >= 3) {
				ret += 3;
				lend -= 3;
				*d++ = '%';
				uint8 val = *s >> 4;
				if(val <= 9) val += '0';
				else val += 0x41 - 10;
				*d++ = val;
				val = *s++ & 0x0F;
				if(val <= 9) val += '0';
				else val += 0x41 - 10;
				*d++ = val;
			}
			else break;
		}
	}
	*d = '\0';
	return ret;
}*/
/******************************************************************************
 * FunctionName : htmlcode
*******************************************************************************/
int ICACHE_FLASH_ATTR htmlcode(uint8 *d, uint8 *s, uint16 lend, uint16 lens)
{
	uint16 ret = 0;
	if(s != NULL) while ((lens--) && (lend--) && (*s != '\0')) {
		if ( *s == 0x27 ) { // "'" &apos;
			if(lend >= 6) {
				ret += 6;
				lend -= 6;
				s++;
				*d++ = '&';
				*d++ = 'a';
				*d++ = 'p';
				*d++ = 'o';
				*d++ = 's';
				*d++ = ';';
			}
			else break;
		} else if ( *s == '"' ) { // &quot;
			if(lend >= 6) {
				ret += 6;
				lend -= 6;
				s++;
				*d++ = '&';
				*d++ = 'q';
				*d++ = 'u';
				*d++ = 'o';
				*d++ = 't';
				*d++ = ';';
			}
			else break;
		} else if ( *s == '&' ) { // &amp;
			if(lend >= 5) {
				ret += 5;
				lend -= 5;
				s++;
				*d++ = '&';
				*d++ = 'a';
				*d++ = 'm';
				*d++ = 'p';
				*d++ = ';';
			}
			else break;
		} else if ( *s == '<' ) { // &lt;
			if(lend >= 4) {
				ret += 4;
				lend -= 4;
				s++;
				*d++ = '&';
				*d++ = 'l';
				*d++ = 't';
				*d++ = ';';
			}
			else break;
		} else if ( *s == '>' ) { // &gt;
			if(lend >= 4) {
				ret += 4;
				lend -= 4;
				s++;
				*d++ = '&';
				*d++ = 'g';
				*d++ = 't';
				*d++ = ';';
			}
			else break;
		} else {
			*d++ = *s++;
			ret++;
		}
	}
	*d = '\0';
	return ret;
}
//=============================================================================
uint8* ICACHE_FLASH_ATTR
web_strnstr(const uint8* buffer, const uint8* token, int len)
{
  const uint8* p;
  int tokenlen = ets_strlen(token);
  if (tokenlen == 0) {
    return (uint8 *)buffer;
  };
  for (p = buffer; *p && (p + tokenlen <= buffer + len); p++) {
    if ((*p == *token) && (ets_strncmp(p, token, tokenlen) == 0)) {
      return (uint8 *)p;
    };
  };
  return NULL;
}
//=============================================================================
static const uint8_t base64map[128] ICACHE_RODATA_ATTR =
{
		255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63,
		 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,  0,255,255,
		255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
		255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255
};
//=============================================================================
bool ICACHE_FLASH_ATTR base64decode(const uint8 *in, int len, uint8_t *out, int *outlen)
{
	uint8 *map = UartDev.rcv_buff.pRcvMsgBuff;
	ets_memcpy(map, base64map, 128);
    int g, t, x, y, z;
    uint8_t c;
    g = 3;
    for (x = y = z = t = 0; x < len; x++) {
        if ((c = map[in[x]&0x7F]) == 0xff) continue;
        if (c == 254) {  /* this is the end... */
            c = 0;
            if (--g < 0) return false;
        }
        else if (g != 3) return false; /* only allow = at end */
        t = (t<<6) | c;
        if (++y == 4) {
            out[z++] = (uint8_t)((t>>16)&255);
            if (g > 1) out[z++] = (uint8_t)((t>>8)&255);
            if (g > 2) out[z++] = (uint8_t)(t&255);
            y = t = 0;
        }
        /* check that we don't go past the output buffer */
        if (z > *outlen) return false;
    }
    if (y != 0) return false;
    *outlen = z;
    return true;
}
//=============================================================================
/* Table 6-bit-index-to-ASCII used for base64-encoding
const u8_t base64_table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
  'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
  'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '+', '/'
};
*/
#define base64_table ((const uint8 * )(0x3FFFD600))
//=============================================================================
/** Base64 encoding */
size_t ICACHE_FLASH_ATTR base64encode(char* target, size_t target_len, const char* source, size_t source_len)
{
  size_t i;
  s8_t j;
  size_t target_idx = 0;
  size_t longer = 3 - (source_len % 3);
  size_t source_len_b64 = source_len + longer;
  size_t len = (((source_len_b64) * 4) / 3);
  u8_t x = 5;
  u8_t current = 0;

  if(target == NULL || target_len < len) return 0;

  for (i = 0; i < source_len_b64; i++) {
    u8_t b = (i < source_len ? source[i] : 0);
    for (j = 7; j >= 0; j--, x--) {
      u8_t shift = ((b & (1 << j)) != 0) ? 1 : 0;
      current |= shift << x;
      if (x == 0) {
        target[target_idx++] = base64_table[current];
        x = 6;
        current = 0;
      }
    }
  }
  for (i = len - longer; i < len; i++) {
    target[i] = '=';
  }
  return len;
}
//=============================================================================

void ICACHE_FLASH_ATTR print_hex_dump(uint8 *buf, uint32 len, uint8 k)
{
	if(!system_get_os_print()) return; // if(*((uint8 *)(0x3FFE8000)) == 0) return;
	uint32 ss[2];
	ss[0] = 0x78323025;
	ss[1] = k;
	uint8* ptr = buf;
//	uint32 i = 0;
	while(len--) {
		if(len == 0) ss[1] = 0;
//		else if((++i & 0x0F) == 0) ss[1] = 0x0d;
//		else ss[1] = k;
		ets_printf((uint8 *)&ss[0], *ptr++);
	}
}