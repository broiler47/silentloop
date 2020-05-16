//
// Created by palulukan on 5/16/20.
//

#ifndef DUPLEX_H_594B6CC3F10E49FEBD81BC5D71EC9070
#define DUPLEX_H_594B6CC3F10E49FEBD81BC5D71EC9070

#include "Readable.h"
#include "Writable.h"

namespace stream
{

class Duplex : public Readable, public Writable
{
};

}   // namespace stream

#endif //DUPLEX_H_594B6CC3F10E49FEBD81BC5D71EC9070
