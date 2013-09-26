/*********************************************************************************

 Copyright 2006-2009 MakingThings

 Licensed under the Apache License, 
 Version 2.0 (the "License"); you may not use this file except in compliance 
 with the License. You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0 
 
 Unless required by applicable law or agreed to in writing, software distributed
 under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied. See the License for
 the specific language governing permissions and limitations under the License.

*********************************************************************************/

/* 
  Written by Simon Josefsson.  Partially adapted from GNU MailUtils
  (mailbox/filter_trans.c, as of 2004-11-28).  Improved by review
  from Paul Eggert, Bruno Haible, and Stepan Kasal
*/

#include "base64.h"

static bool isbase64 (char ch);

/**
  \defgroup base64 Base 64
  Decode and encode base 64 data.

  This is often handy when you need to send raw/binary data (as opposed to text) through a 
  text based format, like XML or JSON.

  Most code lifted from gnulib - http://savannah.gnu.org/projects/gnulib - and written by Simon Josefsson.
  \ingroup dataformats
  @{
*/

/* C89 compliant way to cast 'char' to 'unsigned char'. */
static inline unsigned char to_uchar (char ch)
{
  return ch;
}

/**
  Base 64 encode a block of data.
  Provide a buffer to write into and to read from.  As Base64 encoding results in 
  4 bytes for every 3 source bytes, ensure your destination buffer is large enough.

  @param dest The buffer that the encoded string will be written into.
  @param dest_size The maximum number of bytes to write into the destination buffer.
  @param src A block of data to encode.
  @param src_size The number of bytes from src to encode.
  @return The length of the generated string (not including null termination).

  \par Example
  \code
  #define BUFF_SIZE 256
  char encode_buf[BUFF_SIZE];
  int len = base64Encode(encode_buf, BUFF_SIZE, "test", 4);
  // we now have "dGVzdA==" in encode_buf, and len is 8
  \endcode
*/
int base64Encode(char* dest, int dest_size, const char* src, int src_size)
{
  // our library of valid b64 chars
  static const unsigned char b64str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int orig_size = dest_size;

  while(dest_size && src_size)
  {
    *dest++ = b64str[(to_uchar (src[0]) >> 2) & 0x3f];
    if(!--dest_size)
      break;

    *dest++ = b64str[((to_uchar (src[0]) << 4)
                + (--src_size ? to_uchar (src[1]) >> 4 : 0))
                & 0x3f];
    if(!--dest_size)
      break;

    *dest++ = (src_size ? b64str[((to_uchar (src[1]) << 2)
                + (--src_size ? to_uchar (src[2]) >> 6 : 0)) & 0x3f] : '=');
    if(!--dest_size)
      break;

    *dest++ = src_size ? b64str[to_uchar (src[2]) & 0x3f] : '=';
    if(!--dest_size)
      break;

    if(src_size)
      src_size--;
    if(src_size)
      src += 3;
  }
  if(dest_size)
    *dest = '\0';
  return orig_size - dest_size;
}

/* With this approach this file works independent of the charset used
   (think EBCDIC).  However, it does assume that the characters in the
   Base64 alphabet (A-Za-z0-9+/) are encoded in 0..255.  POSIX
   1003.1-2001 require that char and unsigned char are 8-bit
   quantities, though, taking care of that problem.  But this may be a
   potential problem on non-POSIX C99 platforms.

   IBM C V6 for AIX mishandles "#define B64(x) ...'x'...", so use "_"
   as the formal parameter rather than "x".  */
#define B64(_)          \
  ((_) == 'A' ? 0       \
   : (_) == 'B' ? 1       \
   : (_) == 'C' ? 2       \
   : (_) == 'D' ? 3       \
   : (_) == 'E' ? 4       \
   : (_) == 'F' ? 5       \
   : (_) == 'G' ? 6       \
   : (_) == 'H' ? 7       \
   : (_) == 'I' ? 8       \
   : (_) == 'J' ? 9       \
   : (_) == 'K' ? 10        \
   : (_) == 'L' ? 11        \
   : (_) == 'M' ? 12        \
   : (_) == 'N' ? 13        \
   : (_) == 'O' ? 14        \
   : (_) == 'P' ? 15        \
   : (_) == 'Q' ? 16        \
   : (_) == 'R' ? 17        \
   : (_) == 'S' ? 18        \
   : (_) == 'T' ? 19        \
   : (_) == 'U' ? 20        \
   : (_) == 'V' ? 21        \
   : (_) == 'W' ? 22        \
   : (_) == 'X' ? 23        \
   : (_) == 'Y' ? 24        \
   : (_) == 'Z' ? 25        \
   : (_) == 'a' ? 26        \
   : (_) == 'b' ? 27        \
   : (_) == 'c' ? 28        \
   : (_) == 'd' ? 29        \
   : (_) == 'e' ? 30        \
   : (_) == 'f' ? 31        \
   : (_) == 'g' ? 32        \
   : (_) == 'h' ? 33        \
   : (_) == 'i' ? 34        \
   : (_) == 'j' ? 35        \
   : (_) == 'k' ? 36        \
   : (_) == 'l' ? 37        \
   : (_) == 'm' ? 38        \
   : (_) == 'n' ? 39        \
   : (_) == 'o' ? 40        \
   : (_) == 'p' ? 41        \
   : (_) == 'q' ? 42        \
   : (_) == 'r' ? 43        \
   : (_) == 's' ? 44        \
   : (_) == 't' ? 45        \
   : (_) == 'u' ? 46        \
   : (_) == 'v' ? 47        \
   : (_) == 'w' ? 48        \
   : (_) == 'x' ? 49        \
   : (_) == 'y' ? 50        \
   : (_) == 'z' ? 51        \
   : (_) == '0' ? 52        \
   : (_) == '1' ? 53        \
   : (_) == '2' ? 54        \
   : (_) == '3' ? 55        \
   : (_) == '4' ? 56        \
   : (_) == '5' ? 57        \
   : (_) == '6' ? 58        \
   : (_) == '7' ? 59        \
   : (_) == '8' ? 60        \
   : (_) == '9' ? 61        \
   : (_) == '+' ? 62        \
   : (_) == '/' ? 63        \
   : -1)

static const signed char b64[0x100] = {
  B64 (0), B64 (1), B64 (2), B64 (3),
  B64 (4), B64 (5), B64 (6), B64 (7),
  B64 (8), B64 (9), B64 (10), B64 (11),
  B64 (12), B64 (13), B64 (14), B64 (15),
  B64 (16), B64 (17), B64 (18), B64 (19),
  B64 (20), B64 (21), B64 (22), B64 (23),
  B64 (24), B64 (25), B64 (26), B64 (27),
  B64 (28), B64 (29), B64 (30), B64 (31),
  B64 (32), B64 (33), B64 (34), B64 (35),
  B64 (36), B64 (37), B64 (38), B64 (39),
  B64 (40), B64 (41), B64 (42), B64 (43),
  B64 (44), B64 (45), B64 (46), B64 (47),
  B64 (48), B64 (49), B64 (50), B64 (51),
  B64 (52), B64 (53), B64 (54), B64 (55),
  B64 (56), B64 (57), B64 (58), B64 (59),
  B64 (60), B64 (61), B64 (62), B64 (63),
  B64 (64), B64 (65), B64 (66), B64 (67),
  B64 (68), B64 (69), B64 (70), B64 (71),
  B64 (72), B64 (73), B64 (74), B64 (75),
  B64 (76), B64 (77), B64 (78), B64 (79),
  B64 (80), B64 (81), B64 (82), B64 (83),
  B64 (84), B64 (85), B64 (86), B64 (87),
  B64 (88), B64 (89), B64 (90), B64 (91),
  B64 (92), B64 (93), B64 (94), B64 (95),
  B64 (96), B64 (97), B64 (98), B64 (99),
  B64 (100), B64 (101), B64 (102), B64 (103),
  B64 (104), B64 (105), B64 (106), B64 (107),
  B64 (108), B64 (109), B64 (110), B64 (111),
  B64 (112), B64 (113), B64 (114), B64 (115),
  B64 (116), B64 (117), B64 (118), B64 (119),
  B64 (120), B64 (121), B64 (122), B64 (123),
  B64 (124), B64 (125), B64 (126), B64 (127),
  B64 (128), B64 (129), B64 (130), B64 (131),
  B64 (132), B64 (133), B64 (134), B64 (135),
  B64 (136), B64 (137), B64 (138), B64 (139),
  B64 (140), B64 (141), B64 (142), B64 (143),
  B64 (144), B64 (145), B64 (146), B64 (147),
  B64 (148), B64 (149), B64 (150), B64 (151),
  B64 (152), B64 (153), B64 (154), B64 (155),
  B64 (156), B64 (157), B64 (158), B64 (159),
  B64 (160), B64 (161), B64 (162), B64 (163),
  B64 (164), B64 (165), B64 (166), B64 (167),
  B64 (168), B64 (169), B64 (170), B64 (171),
  B64 (172), B64 (173), B64 (174), B64 (175),
  B64 (176), B64 (177), B64 (178), B64 (179),
  B64 (180), B64 (181), B64 (182), B64 (183),
  B64 (184), B64 (185), B64 (186), B64 (187),
  B64 (188), B64 (189), B64 (190), B64 (191),
  B64 (192), B64 (193), B64 (194), B64 (195),
  B64 (196), B64 (197), B64 (198), B64 (199),
  B64 (200), B64 (201), B64 (202), B64 (203),
  B64 (204), B64 (205), B64 (206), B64 (207),
  B64 (208), B64 (209), B64 (210), B64 (211),
  B64 (212), B64 (213), B64 (214), B64 (215),
  B64 (216), B64 (217), B64 (218), B64 (219),
  B64 (220), B64 (221), B64 (222), B64 (223),
  B64 (224), B64 (225), B64 (226), B64 (227),
  B64 (228), B64 (229), B64 (230), B64 (231),
  B64 (232), B64 (233), B64 (234), B64 (235),
  B64 (236), B64 (237), B64 (238), B64 (239),
  B64 (240), B64 (241), B64 (242), B64 (243),
  B64 (244), B64 (245), B64 (246), B64 (247),
  B64 (248), B64 (249), B64 (250), B64 (251),
  B64 (252), B64 (253), B64 (254), B64 (255)
};

/* Return true if CH is a character from the Base64 alphabet, and
   false otherwise.  Note that '=' is padding and not considered to be
   part of the alphabet.  */
bool isbase64 (char ch)
{
  //return uchar_in_range(to_uchar (ch)); && (0 <= b64[to_uchar(ch)]); -- uchar_in_range is always true for a uchar
  return (0 <= b64[to_uchar(ch)]);
}

/**
  Decode a Base64 string into a block of data.

  @param dest A pointer to the block of data to write in.
  @param dest_size A pointer to the maximum number of bytes to write into dest.  The number of bytes successfully written
  will be stored in this value upon return.
  @param src The base 64 string to decode.
  @param src_size The size of the base 64 string.
  @return True on successful decode, false on failure.

  \par Example
  \code
  #define BUFF_SIZE 256
  char decode_buf[BUFF_SIZE];
  int decode_size = BUFF_SIZE;
  bool result = base64Decode(decode_buf, &decode_size, "dGVzdA==", 8);
  // we now have "test" in decode_buf, and decode_size is set to 4
  \endcode
*/
bool base64Decode(char* dest, int* dest_size, const char* src, int src_size)
{
  int out_remaining = *dest_size;

  while(src_size >= 2)
  {
    if(!isbase64 (src[0]) || !isbase64 (src[1]))
      break;
    if(out_remaining)
    {
      *dest++ = ((b64[to_uchar (src[0])] << 2) | (b64[to_uchar (src[1])] >> 4)); 
      out_remaining--;
    }
    if(src_size == 2)
      break;
    if(src[2] == '=')
    {
      if(src_size != 4)
        break;
      if(src[3] != '=')
        break;
    }
    else
    {
      if(!isbase64 (src[2]))
        break;
      if(out_remaining)
      {
        *dest++ = (((b64[to_uchar (src[1])] << 4) & 0xf0) | (b64[to_uchar (src[2])] >> 2));
        out_remaining--;
      }
      if(src_size == 3)
        break;
      if(src[3] == '=')
      {
        if(src_size != 4)
          break;
      }
      else
      {
        if(!isbase64 (src[3]))
          break;
        if(out_remaining)
        {
          *dest++ = (((b64[to_uchar (src[2])] << 6) & 0xc0) | b64[to_uchar (src[3])]);
          out_remaining--;
        }
      }
    }
    src += 4;
    src_size -= 4;
  }
  *dest_size -= out_remaining;
  if(src_size != 0)
    return false;

  return true;
}

/** @}
*/




