#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/Timestamp.h>
#include <base/Types.h>

namespace thefox
{
    
class Buffer;

typedef void (*ConnectionCallback)(int32_t connId);
typedef void (*CloseCallback)(int32_t connId);
typedef void (*MessageCallback)(int32_t connId,
                                Buffer *buffer,
                                Timestamp recvTime);
typedef void (*WriteCompleteCallback)(int32_t connId);

void defaultConnectionCallback(int32_t connId);
void defaultMessageCallback(int32_t connId,
                            Buffer *buffer,
                            Timestamp recvTime);

} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_