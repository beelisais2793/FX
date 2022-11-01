//////////////////////////////////////
// IIR Filters
//////////////////////////////////////

template<typename T>
T cascade_filter(T input, std::vector<std::function<T (T)>> funcs)
{
    std::reverse(funcs.begin(),funcs.end());
    T a = input;
    for(std::function<T (T)> & f : funcs) {
        a = f(a);
    }
    return a;
}
template<typename T>
T serial_filter(T input, std::vector<std::function<T (T)>> funcs)
{
    std::reverse(funcs.begin(),funcs.end());
    T a = input;
    for(std::function<T (T)> & f : funcs) {
        a *= f(a);
    }
    return a;
}
template<typename T>
T parallel_filter(T input, std::vector<std::function<T (T)>> funcs)
{
    std::reverse(funcs.begin(),funcs.end());
    T a = input;
    for(std::function<T (T)> & f : funcs) {
        a += f(a);
    }
    return a;
}
template<typename T>
T circulator_filter(T input, std::vector<std::function<T (T)>> funcs)
{
    std::reverse(funcs.begin(),funcs.end());
    T a = input;
    for(std::function<T (T)> & f : funcs) {
        a = std::fmod(a,f(a));
    }
    return a;
}

