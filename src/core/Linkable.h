//
// Created by palulukan on 5/16/20.
//

#ifndef LINKABLE_H_92472882EA8449779598EE20B714EE0C
#define LINKABLE_H_92472882EA8449779598EE20B714EE0C

#include <memory>
#include <vector>

class Linkable : public std::enable_shared_from_this<Linkable>
{
    public:
        virtual ~Linkable(void) = default;

    public:
        void LinkWith(const std::shared_ptr<Linkable>& sp);

    protected:
        void _link(const std::shared_ptr<Linkable>& sp);
        bool _lookup(const std::shared_ptr<Linkable>& sp) const;

    private:
        std::vector<std::shared_ptr<Linkable>> m_vecLinks;
};

#endif //LINKABLE_H_92472882EA8449779598EE20B714EE0C
