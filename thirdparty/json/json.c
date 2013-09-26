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

#include "json.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <ctype.h>

/** \defgroup json JSON
	A very small and very fast library for parsing and generating json.

  <a href="http://www.json.org">JSON</a> is quite widely used when communicating with web servers, or other network enabled devices.
  It's nice and small, and easy to work with.  It's quite well supported in many programming
  environments, so it's not a bad option for a communication format when you need to talk to other
  devices from the Make Controller.

  \b Disclaimer - in an attempt to keep it as small and as simple as possible, this library is not
  completely full featured at the moment.  It doesn't process escaped strings for you, and doesn't deal
  with some of the more exotic numeric representations outlined in the JSON specification.
  It does, however, work quite well for most other JSON tasks.

  \section Generating
  Generating JSON is pretty simple - just make successive calls to the API to add the desired
  elements to your string.

  You need to provide a few things:
   - a JsonWriter
   - the buffer in which to store the JSON string being built
   - a count of how many bytes are left in that buffer
   The API will update the count, so it's not too much trouble.

  \code
  #define MAX_JSON_LEN 256
  char jsonbuf[MAX_JSON_LEN];
  int remaining = MAX_JSON_LEN;
  JsonWriter jw;

  char *p = jsonbuf; // keep a pointer to the current location
  jsonwriterInit(&jw);
  p = jsonwriterObjectOpen(&jw);
  p = jsonwriterString(&jw, "hello");
  p = jsonwriterInt(&jw, 234);
  p = jsonwriterObjectClose(&jw);
  // now the string in jsonbuf looks like {"hello":234} - beautifully formatted JSON
  int json_len = MAX_JSON_LEN - jw.remaining; // this is the total length of the string in jsonbuf
  \endcode

  \b Note - the library will add the appropriate separators (: or , usually) to the string,
  depending on the context of the objects and arrays you've opened, or other data you've inserted.

  \section Reading
  Reading is done using an event-based mechanism.  This means you can register for the kind of data you care
  to hear about, and then be called with their value as they're encountered in the JSON string.
  Each read process needs its own JsonReader.

  In each callback, return true to continue parsing, or return false and parsing will stop.

  If you need to pass around some context that you would like available in each of the callbacks,
  you can pass it to jsonreaderInit() and it will be passed to each of the callbacks you've registered.
  Otherwise, just pass 0 if you don't need it.

  \code
  // first, define the functions that we want to be called back on
  bool on_obj_opened(void* ctx)
  {
    // will be called when an object has been opened...
    return true; // keep parsing
  }

  bool on_int(void *ctx, int val)
  {
    iny my_json_int = val;
    // called when an int is encountered...
    return true; // keep parsing
  }

  bool on_string(void *ctx, char *string, int len)
  {
    // called when a string is encountered...
    return true; // keep parsing
  }
  
  // the json string that we'll be reading
  char jsonstr[] = "[{\"label\":\"value\",\"label2\":{\"nested\":234}}]";
  
  // Now, register these callbacks with the JSON reader.
  JsonReader jr;
  jsonreaderInit(&jr, 0);
  jr.start_obj_handler = on_obj_opened;
  jr.int_handler = on_int;
  jr.string_handler = on_string;

  // Finally, run the parser.
  jsonreaderGo(&jr, jsonstr, strlen(jsonstr));
  // now each of our callbacks will be triggered at the appropriate time
  \endcode
  Thanks to YAJL (http://code.google.com/p/yajl-c) for some design inspiration.
	\ingroup dataformats
	@{
*/

static void jsonwriterAppendedAtom(JsonWriter* jw);

/**
  Initialize or reset the state of a JsonWriter.
  Be sure to do this each time before you start a new parse.
  @param jw The JsonWriter to initialize.
  @param buffer The buffer it will be writing to.
  @param len The maximum size of \b buffer
*/
void jsonwriterInit(JsonWriter* jw, char* buffer, int len)
{
  jw->depth = 0;
  jw->steps[0] = JSON_START;
  jw->p = buffer;
  jw->remaining = len;
}

/**
  Open up a new JSON object.
  This adds an opening '{' to the json string.
  @param jw The JsonWriter being used.
  @return A pointer to the location in the JSON buffer after this element has been added, or NULL if there was no room.
*/
char* jsonwriterObjectOpen(JsonWriter* jw)
{
  if (!jw->p)
    return 0;
  int len = 1;
  switch (jw->steps[jw->depth]) {
    case JSON_ARRAY_START:
    case JSON_OBJ_START:
    case JSON_START:
      if (jw->remaining < len)
        return NULL;
      *jw->p = '{';
      break;
    case JSON_OBJ_KEY:
    case JSON_IN_ARRAY:
      len += 1; // for ,
      if (jw->remaining < len)
        return NULL;
      memcpy(jw->p, ",{", len);
      break;
    case JSON_OBJ_VALUE:
      len += 1; // for :
      if (jw->remaining < len)
        return NULL;
      memcpy(jw->p, ":{", len);
      break;
  }
  
  if (++jw->depth > JSON_MAX_DEPTH)
    return NULL;
  jw->steps[jw->depth] = JSON_OBJ_START;
  jw->remaining -= len;
  jw->p += len;
  return jw->p;
}

/**
  Set the key for a JSON object.
  This is a convenience function that simply calls jsonwriterString().
  It is provided to help enforce the idea that the first member of a JSON
  object pair must be a string.
  @param jw The JsonWriter being used.
  @param key The object key
  @return A pointer to the location in the JSON buffer after this element has been added, or NULL if there was no room.
*/
char* jsonwriterObjectKey(JsonWriter* jw, const char *key)
{
  return jsonwriterString(jw, key);
}

/**
  Close a JSON object.
  Adds a closing '}' to the string.
  @param jw The JsonWriter being used.
  @return A pointer to the location in the JSON buffer after this element has been added, or NULL if there was no room.
*/
char* jsonwriterObjectClose(JsonWriter* jw)
{
  if (jw->remaining < 1 || !jw->p )
    return NULL;
  *jw->p++ = '}';
  jw->remaining--;
  jw->depth--;
  jsonwriterAppendedAtom(jw);
  return jw->p;
}

/**
  Open up a new JSON array.
  This adds an opening '[' to the json string.
  @param jw The JsonWriter being used.
  @return A pointer to the location in the JSON buffer after this element has been added, or NULL if there was no room.
*/
char* jsonwriterArrayOpen(JsonWriter* jw)
{
  if (!jw->p)
    return 0;
  int len = 1;
  switch (jw->steps[jw->depth]) {
    case JSON_ARRAY_START:
    case JSON_OBJ_START:
    case JSON_START:
      if (jw->remaining < len)
        return NULL;
      *jw->p = '[';
      break;
    case JSON_OBJ_KEY:
    case JSON_IN_ARRAY:
      len += 1; // for ,
      if (jw->remaining < len)
        return NULL;
      memcpy(jw->p, ",[", len);
      break;
    case JSON_OBJ_VALUE:
      len += 1; // for :
      if (jw->remaining < len)
        return NULL;
      memcpy(jw->p, ":[", len);
      break;
  }
  if (++jw->depth > JSON_MAX_DEPTH)
    return NULL;
  jw->steps[jw->depth] = JSON_ARRAY_START;
  jw->remaining -= len;
  jw->p += len;
  return jw->p;
}

/**
  Close an array.
  Adds a closing ']' to the string.
  @param jw The JsonWriter being used.
  @return A pointer to the JSON buffer after this element has been added, or NULL if there was no room.
*/
char* jsonwriterArrayClose(JsonWriter* jw)
{
  if (jw->remaining < 1 || !jw->p)
    return NULL;
  *jw->p++ = ']';
  jw->remaining--;
  jw->depth--;
  jsonwriterAppendedAtom(jw);
  return jw->p;
}

/**
  Add a string to the current JSON string.
  Depending on whether you've opened objects, arrays, or other inserted 
  other data, the approprate separating symbols will be added to the string.

  @param jw The JsonWriter being used.
  @param string The string to be added.
  @return A pointer to the JSON buffer after this element has been added, or NULL if there was no room.
*/
char* jsonwriterString(JsonWriter* jw, const char *string)
{
  if (!jw->p)
    return 0;
  int string_len = strlen(string) + 2 /* quotes */ + 1 /* null terminator*/;
  // json doesn't actually need the null-terminator, but it's worth
  // requiring so we can just use sprintf directly into the buffer

  switch (jw->steps[jw->depth]) {
    case JSON_ARRAY_START:
    case JSON_OBJ_START:
      if (jw->remaining < string_len)
        return NULL;
      string_len = sniprintf(jw->p, string_len, "\"%s\"", string);
      break;
    case JSON_OBJ_KEY:
    case JSON_IN_ARRAY:
      string_len += 1; // for ,
      if (jw->remaining < string_len)
        return NULL;
      string_len = sniprintf(jw->p, string_len, ",\"%s\"", string);
      break;
    case JSON_OBJ_VALUE:
      string_len += 1; // for :
      if (jw->remaining < string_len)
        return NULL;
      string_len = sniprintf(jw->p, string_len, ":\"%s\"", string);
      break;
    default:
      return NULL;
  }
  jsonwriterAppendedAtom(jw);
  jw->remaining -= string_len;
  jw->p += string_len;
  return jw->p;
}

/**
  Add an int to a JSON string.

  @param jw The JsonWriter being used.
  @param value The integer to be added.
  @return A pointer to the JSON buffer after this element has been added, or NULL if there was no room.
*/
char* jsonwriterInt(JsonWriter* jw, int value)
{
  if (!jw->p)
    return 0;
  int int_as_str_len = 10 + 1 + 1; // largest 32-bit int is 10 digits long, and also leave room for a +/-
  int int_len = 0;
  switch (jw->steps[jw->depth]) {
    case JSON_ARRAY_START:
    {
      char temp[int_as_str_len];
      int_len = sniprintf(temp, int_as_str_len, "%d", value);
      if (jw->remaining < int_len)
        return NULL;
      memcpy(jw->p, temp, int_len);
      break;
    }
    case JSON_IN_ARRAY:
    {
      int_as_str_len += 1; // for ,
      char temp[int_as_str_len];
      int_len = sniprintf(temp, int_as_str_len, ",%d", value);
      if (jw->remaining < int_len)
        return NULL;
      memcpy(jw->p, temp, int_len);
      break;
    }
    case JSON_OBJ_VALUE:
    {
      int_as_str_len += 1; // for :
      char temp[int_as_str_len];
      int_len = sniprintf(temp, int_as_str_len, ":%d", value);
      if (jw->remaining < int_len)
        return NULL;
      memcpy(jw->p, temp, int_len);
      break;
    }
    default:
      return NULL; // bogus state
  }
  jsonwriterAppendedAtom(jw);
  jw->remaining -= int_len;
  jw->p += int_len;
  return jw->p;
}

/**
  Add a boolean value to a JSON string.

  @param jw The JsonWriter being used.
  @param value The boolean value to be added.
  @return A pointer to the JSON buffer after this element has been added, or NULL if there was no room.
*/
char* jsonwriterBool(JsonWriter* jw, bool value)
{
  if (!jw->p)
    return 0;
  const char* boolval;
  int bool_len;
  if (value) {
    boolval = "true";
    bool_len = 4 + 1; // for null terminator
  }
  else {
    boolval = "false";
    bool_len = 5 + 1; // for null terminator
  }
  switch (jw->steps[jw->depth]) {
    case JSON_ARRAY_START:
      if (jw->remaining < bool_len)
        return NULL;
      bool_len = sniprintf(jw->p, bool_len, "%s", boolval);
      break;
    case JSON_IN_ARRAY:
      bool_len += 1; // for ,
      if (jw->remaining < bool_len)
        return NULL;
      bool_len = sniprintf(jw->p, bool_len, ",%s", boolval);
      break;
    case JSON_OBJ_VALUE:
      bool_len += 1; // for :
      if (jw->remaining < bool_len)
        return NULL;
      bool_len = sniprintf(jw->p, bool_len, ":%s", boolval);
      break;
    default:
      return NULL; // bogus state
  }
  jsonwriterAppendedAtom(jw);
  jw->remaining -= bool_len;
  jw->p += bool_len;
  return jw->p;
}

/*
  Called after adding a new value (atom) to a string in order
  to update the state appropriately.
*/
// static
void jsonwriterAppendedAtom(JsonWriter* jw)
{
  switch (jw->steps[jw->depth]) {
    case JSON_OBJ_START:
    case JSON_OBJ_KEY:
      jw->steps[jw->depth] = JSON_OBJ_VALUE;
      break;
    case JSON_ARRAY_START:
      jw->steps[jw->depth] = JSON_IN_ARRAY;
      break;
    case JSON_OBJ_VALUE:
      jw->steps[jw->depth] = JSON_OBJ_KEY;
      break;
    default:
      break;
  }
}

/****************************************************************************
 JsonDecode
****************************************************************************/

typedef enum JsonReaderToken_t {
  token_true,
  token_false,
  token_colon,
  token_comma,
  token_eof,
  token_error,
  token_left_brace,
  token_left_bracket,
  token_null,
  token_right_brace,
  token_right_bracket,
  token_number,
  token_maybe_negative,
  token_string,
  token_unknown
} JsonReaderToken;

static JsonReaderToken jsonreaderGetToken(char* text, int len);

/**
  Initialize or reset a JsonReader.
  Do this prior to making a call to jsonreaderGo().
  @param jr The JsonReader to use.
  @param context (optional) An optional parameter that your code can use to
  pass around a known object within the callbacks.  Set it to 0 if you don't need it.
  @param resetHandlers True to disable all the handlers, false to leave them untouched.
*/
void jsonreaderInit(JsonReader* jr, void* context, bool resetHandlers)
{
  jr->depth = 0;
  jr->gotcomma = false;
  jr->context = context;
  jr->p = 0;
  jr->len = 0;
  if (resetHandlers) {
    jr->null_handler        = 0;
    jr->bool_handler        = 0;
    jr->int_handler         = 0;
    jr->float_handler       = 0;
    jr->string_handler      = 0;
    jr->start_obj_handler   = 0;
    jr->obj_key_handler     = 0;
    jr->end_obj_handler     = 0;
    jr->start_array_handler = 0;
    jr->end_array_handler   = 0;
  }
}

/**
  Parse a JSON string.
  While it's reading, handlers that have been registered will be called
  with any relevant data.
  @param jr The JsonReader to use.
  @param text The JSON string to parse.
  @param len The length of the JSON string.
  @return True on a successful parse, false on failure.

  \par Example
  \code
  // quotes are escaped since I'm writing it out manually
  char jsonstr[] = "[{\"label\":\"value\",\"label2\":{\"nested\":234}}]";
  JsonReader jr;
  jsonreaderInit(&jr, 0);
  jr.int_handler = myIntHandler;
  jsonreaderGo(&jr, jsonstr, strlen(jsonstr));
  // now we expect to be called back on any callbacks we registered.
  \endcode
*/
bool jsonreaderGo(JsonReader* jr, char* text, int len)
{
  JsonReaderToken token;
  jr->p = text;
  jr->len = len;

  while (jr->len) {
    while (*jr->p == ' ') // eat white space
      jr->p++;
    token = jsonreaderGetToken( jr->p, jr->len);
    switch (token) {
      case token_true:
        if (jr->bool_handler && !jr->bool_handler(jr->context, true))
          return false;
        jr->p += 4;
        jr->len -= 4;
        break;
      case token_false:
        if (jr->bool_handler && !jr->bool_handler(jr->context, false))
          return false;
        jr->p += 5;
        jr->len -= 5;
        break;
      case token_null:
        if (jr->null_handler && !jr->null_handler(jr->context))
          return false;
        jr->p += 4;
        jr->len -= 4;
        break;
      case token_comma:
        jr->gotcomma = true;
        // intentional fall-through
      case token_colon: // just get the next one      
        jr->p++;
        jr->len--;
        break;
      case token_left_bracket:
        jr->steps[++jr->depth] = JSON_READER_OBJECT_START;
        if (jr->start_obj_handler && !jr->start_obj_handler(jr->context))
          return false;
        jr->p++;
        jr->len--;
        break;
      case token_right_bracket:
        jr->depth--;
        if (jr->end_obj_handler && !jr->end_obj_handler(jr->context))
          return false;
        jr->p++;
        jr->len--;
        break;
      case token_left_brace:
        jr->steps[++jr->depth] = JSON_READER_IN_ARRAY;
        if (jr->start_array_handler && !jr->start_array_handler(jr->context))
          return false;
        jr->p++;
        jr->len--;
        break;
      case token_right_brace:
        jr->depth--;
        if (jr->end_array_handler && !jr->end_array_handler(jr->context))
          return false;
        jr->p++;
        jr->len--;
        break;
      case token_number:
      {
        const char* p = jr->p;
        bool keepgoing = true;
        bool gotdecimal = false;
        do {
          if (*p == '.') {
            if (gotdecimal) // we only expect to get one decimal place in a number
              return false;
            gotdecimal = true;
            p++;
          }
          else if (!isdigit((int)*p))
            keepgoing = false;
          else
            p++;
        } while (keepgoing);
        int size = p - jr->p;
        if (gotdecimal) {
          if (jr->float_handler && !jr->float_handler(jr->context, atof(jr->p)))
            return false;
        }
        else {
          if (jr->int_handler && !jr->int_handler(jr->context, atoi(jr->p)))
            return false;
        }
        jr->p += size;
        jr->len -= size;
        break;
      }
      case token_string:
      {
        char* p = ++jr->p; // move past the opening "
        jr->len--;
        bool keepgoing = true;
        while (keepgoing) {
          if (*p == '\\') // we got an escape - skip the escape and the next character
            p += 2;
          else if (*p == '"') // we got the end of a string
            keepgoing = false;
          else
            p++; // keep looking for the end of the string
        }
        int size = p - jr->p;
        *p = 0; // replace the trailing " with a null to make a string

        // figure out if this is a key or a normal string
        bool objkey = false;
        if (jr->steps[jr->depth] == JSON_READER_OBJECT_START) {
            jr->steps[jr->depth] = JSON_READER_IN_OBJECT;
          objkey = true;
        }
        if (jr->gotcomma && jr->steps[jr->depth] == JSON_READER_IN_OBJECT) {
          jr->gotcomma = false;
          objkey = true;
        }

        if (objkey) { // last one was a comma - next string has to be a key
          if(jr->obj_key_handler && !jr->obj_key_handler(jr->context, jr->p, size))
            return false;
        }
        else { // just a normal string
          if (jr->string_handler && !jr->string_handler(jr->context, jr->p, size))
            return false;
        }
        jr->p += (size+1); // account for the trailing "
        jr->len -= (size+1);
        break;
      }
      default:
        return false;
    }
  }
  return true;
}

/** @}
*/

// static
JsonReaderToken jsonreaderGetToken(char* text, int len)
{
  if (len < 1) return token_unknown;
  switch (text[0]) {
    case ':': return token_colon;
    case ',': return token_comma;
    case '{': return token_left_bracket;
    case '}': return token_right_bracket;
    case '[': return token_left_brace;
    case ']': return token_right_brace;
    case '"': return token_string;
    case '0': case '1': case '2': case '3': case '4': 
    case '5': case '6': case '7': case '8': case '9':
      return token_number;
    case '-': return token_maybe_negative;
    case 't': return strncmp(text, "true", 4) == 0 ? token_true : token_unknown;
    case 'f': return strncmp(text, "false", 5) == 0 ? token_false : token_unknown;
    case 'n': return strncmp(text, "null", 4) == 0 ? token_null : token_unknown;
    case '\0': return token_eof;
    default:  return token_unknown;
  }
}





