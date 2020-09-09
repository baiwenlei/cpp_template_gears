///Michael Caisse: ”Solving World Problems with Fusion“ -- group example implementation
///youtube: https://www.youtube.com/watch?v=6V73Q7ULFi0&t=4570s


#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/container.hpp>
#include <iostream>
#include <type_traits>

using namespace boost;

namespace sig {
    struct a{};
    struct b{};
    struct c{};
    struct d{};
    struct e{};
}

struct group_1 : mpl::vector<sig::a, sig::b> {};
struct group_2 : mpl::vector<sig::c, sig::d> {};
struct group_3 : mpl::vector<sig::e> {};

struct group_4 : mpl::vector<mpl::vector<sig::a, sig::b>, sig::e> {};


template <class Seq, class T, class Enable = void>
struct flatten_groups_impl {
    using type = typename mpl::push_back<Seq, T>::type;
};

//待展开对象是'容器'时,递归展开容器内各对象
template <class Seq, class T>
struct flatten_groups_impl<Seq, T, std::enable_if_t<mpl::is_sequence<T>::value>> {
    using type = typename mpl::fold<T, Seq, flatten_groups_impl<mpl::_1, mpl::_2>>::type;
};

template <class Groups>
struct flatten_groups : flatten_groups_impl<mpl::vector<>, Groups> {};

template <class Groups>
using flatten_groups_t = typename flatten_groups<Groups>::type;

template <class Groups>
class signal_combiner {
public:
    using sig_map_def_t = typename mpl::transform<flatten_groups_t<Groups>, typename fusion::result_of::make_pair<mpl::_1, std::function<void()>>::type >::type;
    using sig_map_t = typename fusion::result_of::as_map<sig_map_def_t>::type;
    
    template <class Signal, class F>
    void bind(F f) {
        fusion::at_key<Signal>(sign_map) = f;
    }
    
    template <class Group>
    void notifyGroup() {
        mpl::for_each<Group>(*this);
    }
    
    template <class Signal>
    void notifySignal() {
        using Group = typename mpl::deref<typename mpl::find_if<Groups, mpl::contains<mpl::_1, Signal>>::type>::type;
        notifyGroup<Group>();
    }
    
    template <class Signal>
    void operator()(const Signal& item) {
        auto const& f = fusion::at_key<Signal>(sign_map);
        if (f) {
            f();
        }
    }
    
    sig_map_t sign_map;
};

struct Foo : public mpl::vector<group_1, group_2, group_3> {};
using SignalCombiner = signal_combiner<Foo>;


int main(int argc, char **argv) {
    SignalCombiner combiner;
    combiner.bind<sig::a>([]{std::cout << "notify sig::a\n";});
    combiner.bind<sig::b>([]{std::cout << "notify sig::b\n";});
    combiner.bind<sig::c>([]{std::cout << "notify sig::c\n";});
    combiner.bind<sig::d>([]{std::cout << "notify sig::d\n";});
    combiner.bind<sig::e>([]{std::cout << "notify sig::e\n";});

    combiner.notifyGroup<group_1>();

    combiner.notifySignal<sig::c>();

    return 0;
}
