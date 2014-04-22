#include "rpc/RpcController.h"
#include <google/protobuf/message.h>

using namespace thefox;

RpcController::RpcController()
{
}

RpcController::~RpcController()
{
}

void RpcController::statusCallback(
      struct evrpc_status *status,
      void* request,
      void* response,
      void *arg)
{
  VLOG(0) << "statusCallback " << status->error;
  gpb::Closure* closure = static_cast<gpb::Closure*>(arg);
  closure->Run();
}

void RpcController::marshal(struct evbuffer* buf, void* arg)
{
  const gpb::Message* msg = static_cast<const gpb::Message*>(arg);
  VLOG(0) << "marshal " << msg->GetTypeName();
  std::string s = msg->SerializeAsString();
  evbuffer_add(buf, s.data(), s.size());
}

int RpcController::unmarshal(void* arg, struct evbuffer* buf)
{
  gpb::Message* msg = static_cast<gpb::Message*>(arg);
  VLOG(0) << "unmarshal " << msg->GetTypeName();
  size_t len = evbuffer_get_length(buf);
  std::vector<char> array(len);
  size_t nread = evbuffer_remove(buf, &*array.begin(), len);
  CHECK_EQ(len, nread);
  bool ok = msg->ParseFromArray(&*array.begin(), nread);
  return ok ? 0 : -1;
}

void RpcController::clear(void* arg)
{
  gpb::Message* msg = static_cast<gpb::Message*>(arg);
  VLOG(0) << "clear " << msg->GetTypeName();
}

void* RpcController::newMessage(void* arg)
{
  const gpb::Message* msg = static_cast<const gpb::Message*>(arg);
  VLOG(0) << "new " << msg->GetTypeName();
  return msg->New();
}

void RpcController::deleteMessage(void* arg)
{
  gpb::Message* msg = static_cast<gpb::Message*>(arg);
  VLOG(0) << "delete " << msg->GetTypeName();
  delete msg;
}

int RpcController::complete(void* arg)
{
  gpb::Message* msg = static_cast<gpb::Message*>(arg);
  VLOG(0) << "complete " << msg->GetTypeName();
  return 0;
}