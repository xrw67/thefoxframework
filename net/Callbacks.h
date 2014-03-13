#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/Timestamp.h>
#include <base/Types.h>

namespace thefox
{
    
class Buffer;

typedef void (*ConnectionCallback)(uint32_t connId);
typedef void (*CloseCallback)(uint32_t connId);
typedef void (*MessageCallback)(uint32_t connId,
                                Buffer *buffer,
                                Timestamp recvTime);

void defaultConnectionCallback(cuint32_t connId);
void defaultMessageCallback(uint32_t connId,
                            Buffer *buffer,
                            Timestamp recvTime);

} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_