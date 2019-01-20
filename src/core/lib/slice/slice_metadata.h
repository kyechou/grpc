#ifndef GRPC_CORE_LIB_SLICE_SLICE_METADATA_H
#define GRPC_CORE_LIB_SLICE_SLICE_METADATA_H

#include <grpc/support/port_platform.h>

#include <grpc/slice_buffer.h>
#include <grpcpp/timestamps.h>

grpc::TimestampsArgs *grpc_slice_buffer_get_tsarg(const grpc_slice_buffer *sb);

#endif /* GRPC_CORE_LIB_SLICE_SLICE_METADATA_H */
