//
// Created by palulukan on 5/16/20.
//

#ifndef PROCESS_H_6C09C452E88B4903954E4F03104DEE6B
#define PROCESS_H_6C09C452E88B4903954E4F03104DEE6B

#include <functional>

namespace process
{

void NextTick(const std::function<void(void)>& cb);

}   // namespace process

#endif //PROCESS_H_6C09C452E88B4903954E4F03104DEE6B
