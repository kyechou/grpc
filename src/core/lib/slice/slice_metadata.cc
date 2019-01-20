#include "src/core/lib/slice/slice_metadata.h"

#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <grpc/support/log.h>

using namespace std;

static bool is_valid(char c) {
  if (c < ' ' || c == '@' || c == 0x7f)
    return false;
  return true;
}

static const multimap<string, string> grpc_slice_buffer_get_metadata(
    const grpc_slice_buffer *sb, const vector<string> &keys) {
  multimap<string, string> metadata;
  size_t buf_len = 0, slice_len;
  uint8_t *buf = new uint8_t[sb->length + 1], *slice;

  for (size_t i = 0; i < sb->count; ++i) {
    slice = GRPC_SLICE_START_PTR(sb->slices[i]);
    slice_len = GRPC_SLICE_LENGTH(sb->slices[i]);
    memcpy(buf + buf_len, slice, slice_len);
    buf_len += slice_len;
  }
  GPR_ASSERT(buf_len == sb->length);
  buf[buf_len] = 0;

  uint8_t *s = buf;
  while (s < buf + buf_len) {
    for (auto key = keys.begin(); key != keys.end(); ++key) {
      const char *k = strstr((const char *)s, key->c_str());
      if (k) {  // found the key
        const char *begin = k + key->size() + 1, *end;
        while (!is_valid(*begin))
          ++begin;
        end = begin + 1;
        while (is_valid(*end))
          ++end;
        string value(begin, end - begin);
        metadata.insert(make_pair(*key, value));
      }
    }
    s += strlen((const char *)s) + 1;
  }

  delete [] buf;
  return metadata;
}

grpc::TimestampsArgs *grpc_slice_buffer_get_tsarg(const grpc_slice_buffer *sb) {
  grpc::TimestampsArgs *arg = nullptr;

  // get metadata if any
  string rpc_uuid, rpc_type, func_name;
  vector<string> keys;
  keys.push_back("rpc_uuid");
  keys.push_back("rpc_type");
  keys.push_back("func_name");
  const multimap<string, string> &metadata
    = grpc_slice_buffer_get_metadata(sb, keys);
  auto it = metadata.find("rpc_uuid");
  rpc_uuid = (it == metadata.end()) ? "" : it->second;
  it = metadata.find("rpc_type");
  rpc_type = (it == metadata.end()) ? "" : it->second;
  it = metadata.find("func_name");
  func_name = (it == metadata.end()) ? "" : it->second;

  /* Create a new TimestampsArgs only if metadata is recovered successfully. */
  if (!rpc_uuid.empty() && !rpc_type.empty() && !func_name.empty()) {
    arg = new grpc::TimestampsArgs;
    arg->rpc_uuid = rpc_uuid;
    arg->rpc_type = rpc_type;
    arg->func_name = func_name;
    arg->peer = "";
    arg->seq_no = 0;
  }

  return arg;
}
