/**
 * Copyright 2016 Yahoo Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Latch.h"

#include <boost/make_shared.hpp>

namespace pulsar {

struct CountIsZero {
    const int& count_;

    CountIsZero(const int& count)
            : count_(count) {
    }

    bool operator()() const {
        return count_ == 0;
    }
};

Latch::Latch(int count)
        : state_(boost::make_shared<InternalState>()) {
    state_->count = count;
}

void Latch::countdown() {
    Lock lock(state_->mutex);

    state_->count--;

    if (state_->count == 0) {
        state_->condition.notify_all();
    }
}

int Latch::getCount() {
    Lock lock(state_->mutex);

    return state_->count;

}

void Latch::wait() {
    Lock lock(state_->mutex);

    state_->condition.wait(lock, CountIsZero(state_->count));
}

bool Latch::wait(const boost::posix_time::time_duration& timeout) {
    Lock lock(state_->mutex);
    return state_->condition.timed_wait(lock, timeout, CountIsZero(state_->count));
}

} /* namespace pulsar */
