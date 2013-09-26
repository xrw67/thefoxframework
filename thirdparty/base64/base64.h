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

#ifndef BASE_64_H
#define BASE_64_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
bool base64Decode(char* dest, int* dest_size, const char* src, int src_size);
int  base64Encode(char* dest, int dest_size, const char* src, int src_size);
#ifdef __cplusplus
}
#endif
#endif // BASE_64_H
