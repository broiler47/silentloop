//
// Created by palulukan on 5/16/20.
//

#ifndef LINKABLE_H_92472882EA8449779598EE20B714EE0C
#define LINKABLE_H_92472882EA8449779598EE20B714EE0C

#include "Log.h"

#include <functional>
#include <memory>
#include <vector>

class Linkable : public std::enable_shared_from_this<Linkable>
{
    public:
        template <typename T = Linkable>
        std::shared_ptr<T> GetSharedPtr(void)
        {
            // NOTE: In case this object is not managed by std::shared_ptr,
            // shared_from_this() will only throw std::bad_weak_ptr in C++17 or later.
            std::shared_ptr<Linkable> spThis;
            try
            {
                spThis = shared_from_this();
            }
            catch(std::bad_weak_ptr&)
            {
                auto errMsg = "Linkable objects should be managed by std::shared_ptr and should not be linked from the constructor";
                DEBUG(errMsg);
                throw std::runtime_error(errMsg);
            }

            return std::static_pointer_cast<T>(spThis);
        }

    public:
        virtual ~Linkable(void) = default;

    public:
        void LinkWith(const std::shared_ptr<Linkable>& sp);
        void NextTick(const std::function<void(void)> &cb);

    private:
        void _link(const std::shared_ptr<Linkable>& sp);
        bool _lookup(const std::shared_ptr<Linkable>& sp);

    private:
        std::vector<std::shared_ptr<Linkable>> m_vecLinks;
};

#endif //LINKABLE_H_92472882EA8449779598EE20B714EE0C
