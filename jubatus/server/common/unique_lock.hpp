// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_SERVER_COMMON_UNIQUE_LOCK_HPP_
#define JUBATUS_SERVER_COMMON_UNIQUE_LOCK_HPP_

#include <algorithm>
#include <cassert>
#include <pficommon/lang/noncopyable.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>

namespace jubatus {
namespace server {
namespace common {

namespace unique_lock_detail_ {

template<class Lockable>
class default_lock_functions {
 public:
  static bool lock(Lockable& l) {
    return l.lock();
  }
  static void unlock(Lockable& l) {
    l.unlock();
  }
};


template<class Lockable,
         class LockFunctions = default_lock_functions<Lockable> >
class basic_unique_lock : pfi::lang::noncopyable {
 public:
  typedef Lockable lockable_type;
  typedef LockFunctions lock_functions;

  basic_unique_lock()
      : lp_(), locked_(false) {
  }

  explicit basic_unique_lock(lockable_type& l)
      : lp_(&l), locked_(false) {
    if (lock_functions::lock(*lp_)) {
      locked_ = true;
    }
  }

  ~basic_unique_lock() {
    unlock();
  }

  void swap(basic_unique_lock& ul) {
    using std::swap;
    swap(lp_, ul.lp_);
    swap(locked_, ul.locked_);
  }
  friend void swap(basic_unique_lock& l, basic_unique_lock& r) {
    l.swap(r);
  }

  void unlock() {
    if (locked_) {
      assert(lp_ != NULL);
      lock_functions::unlock(*lp_);
      locked_ = false;
    }
  }

  bool locked() const {
    return locked_;
  }

 private:
  lockable_type* lp_;
  bool locked_;
};

}  // namespace unique_lock_detail_

using unique_lock_detail_::basic_unique_lock;
typedef basic_unique_lock<pfi::concurrent::lockable> unique_lock;

}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_UNIQUE_LOCK_HPP_
