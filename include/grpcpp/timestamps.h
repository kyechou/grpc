#ifndef GRPCPP_TIMESTAMPS_H
#define GRPCPP_TIMESTAMPS_H

#include <string>
#include <grpc/impl/codegen/gpr_types.h>

/*
 * The original grpc_core::Timestamps structure is defined in
 * "src/core/lib/iomgr/buffer_list.h", which is not exported/installed for
 * public use. Note that this has to be identical to the original structure.
 */
namespace grpc {
class Timestamps {
 public:
  gpr_timespec sendmsg_time;
  gpr_timespec scheduled_time;
  gpr_timespec sent_time;
  gpr_timespec received_time;
  gpr_timespec acked_time;
};

/*
 * Arguments for grpc_core::timestamps_callback. Only rpc_uuid, rpc_type, and
 * func_name are sent via custom metadata. Instances of TimestampsArgs are
 * created in:
 * chttp2_transport.cc: write_action()
 * 	by slice_metadata.cc: grpc_slice_buffer_get_tsarg()
 * , or
 * tcp_posix.cc: tcp_do_read()
 * 	by slice_metadata.cc: grpc_slice_buffer_get_tsarg()
 *
 * and freed by __ts_cb_wrapper() in "server/server_cc.cc" and
 * "client/channel_cc.cc", when timestamps_callback is called.
 *
 * rpc_uuid, rpc_type, func_name, and peer are filled by write_action() in
 * "chttp2_transport.cc", or by tcp_do_read() in "tcp_posix.cc". seq_no and size
 * is filled by TracedBuffer::AddNewEntry() in "buffer_list.cc", called by
 * tcp_write_with_timestamps() in "tcp_posix.cc" right before sending the TCP
 * packet.
 */
class TimestampsArgs {
 public:
  std::string rpc_uuid; // request and response of the same RPC call will have
                        // the same rpc_uuid (created when the request is sent)
  std::string rpc_type; // "request" (from client to server) or
                        // "response" (from server to client)
  std::string func_name;
  std::string peer;     // peer endpoint
  uint32_t    seq_no;   // ("TCP relative seq no" - 2 + "msg payload size")
  uint32_t    size;     // message payload size (not TCP payload)
};
}

#endif
