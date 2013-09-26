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

#ifndef JSON_H
#define JSON_H

#include "types.h"

#ifndef JSON_MAX_DEPTH
#define JSON_MAX_DEPTH 20
#endif

// state object for encoding
typedef enum JsonWriterStep_t {
  JSON_START,
  JSON_OBJ_START,
  JSON_OBJ_KEY,
  JSON_OBJ_VALUE,
  JSON_ARRAY_START,
  JSON_IN_ARRAY
} JsonWriterStep;

/**
  The structure used to maintain the state of a JSON encode process.
  You'll need to have one of these for each JSON string you want to encode.
  The same variable can be reused after resetting it with a call to jsonwriterInit().
 */
typedef struct JsonWriter_t {
  JsonWriterStep steps[JSON_MAX_DEPTH]; /**< An array to keep track of the state of each step in the encoder. */
  int depth;                            /**< The current depth of the encoder (how many elements have been opened). */
  char* p;                              /**< A pointer to the buffer to write into. */
  int remaining;                        /**< The number of bytes remaining in the buffer to write into. */
} JsonWriter;

// state object for decoding
typedef enum JsonReaderStep_t {
  JSON_READER_OBJECT_START,
  JSON_READER_IN_OBJECT,
  JSON_READER_IN_ARRAY
} JsonReaderStep;

/**
  The structure used to maintain the state of a JSON decode process.
  You'll need to have one of these for each JSON string you want to encode.
  The same variable can be reused after resetting it with a call to jsonreaderInit().
 */
typedef struct JsonReader_t {
  JsonReaderStep steps[JSON_MAX_DEPTH];     /**< An array to keep track of each step of the decoder. */
  int depth;                                /**< The current depth of the decoder (how many elements have been opened). */
  bool gotcomma;                            /**< Used internally by the decoder. */
  void* context;                            /**< A pointer to the user context. */
  char* p;                                  /**< A pointer to the data. */
  int len;                                  /**< The current length. */
  bool(*null_handler)(void*);                /**< Called when "null" is encountered. */
  bool(*bool_handler)(void*, bool);          /**< Called when a boolean value is encountered. */
  bool(*int_handler)(void*, int);            /**< Called when an int is encountered. */
  bool(*float_handler)(void*, float);        /**< Called when a float is encountered. */
  bool(*string_handler)(void*, char*, int);  /**< Called when a string value is encountered. */
  bool(*start_obj_handler)(void*);           /**< Called when an object is opened - a { is encountered. */
  bool(*obj_key_handler)(void*, char*, int); /**< Called when the key of an object is encountered. */
  bool(*end_obj_handler)(void*);             /**< Called when an object is closed - a } is encountered. */
  bool(*start_array_handler)(void*);         /**< Called when an array is opened - a [ is encountered. */
  bool(*end_array_handler)(void*);           /**< Called when an object is closed - a ] is encountered. */
} JsonReader;

#ifdef __cplusplus
extern "C" {
#endif
// writer
void  jsonwriterInit(JsonWriter* jw, char* buffer, int length);
char* jsonwriterObjectOpen(JsonWriter* jw);
char* jsonwriterObjectKey(JsonWriter* jw, const char *key);
char* jsonwriterObjectClose(JsonWriter* jw);
char* jsonwriterArrayOpen(JsonWriter* jw);
char* jsonwriterArrayClose(JsonWriter* jw);
char* jsonwriterString(JsonWriter* jw, const char *string);
char* jsonwriterInt(JsonWriter* jw, int value);
char* jsonwriterBool(JsonWriter* jw, bool value);

// reader
void jsonreaderInit(JsonReader* jr, void* context, bool resetHandlers);
bool jsonreaderGo(JsonReader* jr, char* text, int len);
#ifdef __cplusplus
}
#endif

#endif /* JSON_H */


