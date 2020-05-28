#ifndef PLATFORM_V2_BASE_BASE_PIPE_H_
#define PLATFORM_V2_BASE_BASE_PIPE_H_

#include <cstdint>
#include <deque>
#include <memory>

#include "platform_v2/api/condition_variable.h"
#include "platform_v2/api/mutex.h"
#include "platform_v2/base/byte_array.h"
#include "platform_v2/base/exception.h"
#include "platform_v2/base/input_stream.h"
#include "platform_v2/base/output_stream.h"
#include "absl/base/thread_annotations.h"

namespace location {
namespace nearby {

// Common Pipe implenentation.
// It does not depend on platform implementation, and this allows it to
// be used in the platform implementation itself.
// Concrete class must be derived from it, as follows:
//
// class DerivedPipe : public BasePipe {
//  public:
//   DerivedPipe() {
//     auto mutex = /* construct platform-dependent mutex */;
//     auto cond = /* construct platform-dependent condition variable */;
//     Setup(std::move(mutex), std::move(cond));
//   }
//   ~DerivedPipe() override = default;
//   DerivedPipe(DerivedPipe&&) = default;
//   DerivedPipe& operator=(DerivedPipe&&) = default;
// };
class BasePipe {
 public:
  static constexpr const size_t kChunkSize = 64 * 1024;
  virtual ~BasePipe() = default;

  // Pipe is not copyable or movable, because copy/move will invalidate
  // references to input and output streams.
  // If move is required, Pipe could be wrapped with std::unique_ptr<>.
  BasePipe(BasePipe&&) = delete;
  BasePipe& operator=(BasePipe&&) = delete;

  // Get...() methods return references to input and output steam facades.
  // It is safe to call Get...() methods multiple times.
  InputStream& GetInputStream() { return input_stream_; }
  OutputStream& GetOutputStream() { return output_stream_; }

 protected:
  BasePipe() = default;

  void Setup(std::unique_ptr<api::Mutex> mutex,
             std::unique_ptr<api::ConditionVariable> cond) {
    mutex_ = std::move(mutex);
    cond_ = std::move(cond);
  }

 private:
  class BasePipeInputStream : public InputStream {
   public:
    explicit BasePipeInputStream(BasePipe* pipe) : pipe_(pipe) {}
    ~BasePipeInputStream() override { DoClose(); }

    ExceptionOr<ByteArray> Read(std::int64_t size) override {
      return pipe_->Read(size);
    }
    Exception Close() override {
      return DoClose();
    }

   private:
    Exception DoClose() {
      pipe_->MarkInputStreamClosed();
      return {Exception::kSuccess};
    }
    BasePipe* pipe_;
  };
  class BasePipeOutputStream : public OutputStream {
   public:
    explicit BasePipeOutputStream(BasePipe* pipe) : pipe_(pipe) {}
    ~BasePipeOutputStream() override { DoClose(); }

    Exception Write(const ByteArray& data) override {
      return pipe_->Write(data);
    }
    Exception Flush() override { return {Exception::kSuccess}; }
    Exception Close() override {
      return DoClose();
    }

   private:
    Exception DoClose() {
      pipe_->MarkOutputStreamClosed();
      return {Exception::kSuccess};
    }
    BasePipe* pipe_;
  };

  ExceptionOr<ByteArray> Read(size_t size) ABSL_LOCKS_EXCLUDED(mutex_);
  Exception Write(const ByteArray& data) ABSL_LOCKS_EXCLUDED(mutex_);

  void MarkInputStreamClosed() ABSL_LOCKS_EXCLUDED(mutex_);
  void MarkOutputStreamClosed() ABSL_LOCKS_EXCLUDED(mutex_);

  Exception WriteLocked(const ByteArray& data)
      ABSL_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Order of declaration matters:
  // - mutex must be defined before condvar;
  // - input & output streams must be after both mutex and condvar.
  bool input_stream_closed_ ABSL_GUARDED_BY(mutex_) = false;
  bool output_stream_closed_ ABSL_GUARDED_BY(mutex_) = false;
  bool read_all_chunks_ ABSL_GUARDED_BY(mutex_) = false;

  std::deque<ByteArray> ABSL_GUARDED_BY(mutex_) buffer_;
  std::unique_ptr<api::Mutex> mutex_;
  std::unique_ptr<api::ConditionVariable> cond_;

  BasePipeInputStream input_stream_{this};
  BasePipeOutputStream output_stream_{this};
};

}  // namespace nearby
}  // namespace location

#endif  // PLATFORM_V2_BASE_BASE_PIPE_H_
