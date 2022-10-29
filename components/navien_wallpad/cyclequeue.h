#pragma once


namespace esphome {
namespace navien_wallpad {


template<class _Ty> class CycleQueue {
public:
  CycleQueue(int max_data = 256) : max_data_(max_data) {
    start_ = end_ = 0;
    data_ = new _Ty[max_data_ + 1];
  }


  CycleQueue(const CycleQueue& other) : max_data_(other.max_data_) {
    start_ = other.start_;
    end_ = other.end_;
    data_ = new _Ty[max_data_ + 1];
    memcpy(data_, other.data_, sizeof(_Ty) * (max_data_ + 1));
  }


  ~CycleQueue() {
    if (data_ != nullptr) {
      delete[] data_;
      data_ = nullptr;
    }
  }


public:
  bool empty() const {
    return (start_ == end_);
  }


  int size() const {
    if (start_ <= end_) {
      return end_ - start_;
    }

      return (max_data_ + 1) + end_ - start_;
  }


  void clear() {
    start_ = end_;
  }


  void push(_Ty b) {
    if ((end_ + 1) % (max_data_ + 1) == start_) {
      pop();
    }

    data_[end_] = b;
    ++end_ %= (max_data_ + 1);
  }


  _Ty front() const {
    if (empty()) {
      return 0;
    }

    return data_[start_];
  }


  void pop() {
    if (empty()) return;

    ++start_ %= (max_data_ + 1);
  }


public:
  _Ty& operator[] (int index) {
    return data_[(start_ + index) % (max_data_ + 1)];
  }


  const _Ty& operator[] (int index) const {
    return data_[(start_ + index) % (max_data_ + 1)];
  }


  void to_buffer(_Ty* p, int start, int end) const {
    for (int i = start; i < end; i++) {
      p[i] = (*this)[i];
    }
  }


public:
  const int max_data_;


protected:
  _Ty* data_;
  int start_;
  int end_;
};


} // namespace navien_wallpad
} // namespace esphome
