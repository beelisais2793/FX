#include "tfp/config.hpp"

#include "tfp/util/TemplateStuff.hpp"
#include "tfp/util/Metric.hpp"

#include <cassert>

namespace tfp {

/*!
 * @brief A parameter consists of a value, a metric and a unit. For example,
 * "12 km/s" will have a value of "12", a metric "k" and a unit "m/s".
 *
 * History & Design Decisions
 * --------------------------
 * This class was primarily designed to store floating point numbers as SI
 * units and provide methods to convert between different metrics. It was later
 * decided to also add support for simply storing the three values mentioned
 * above as strings (rather than as floats) in which case no conversions are
 * performed at all. In order to ensure the conversion methods toSI(),
 * toHumanReadable() etc. can only be used if T is a floating point number,
 * the enable_if construct was ported from c++11 to c++03.
 *
 * The Parameter<std::string> variant is used in the Importer class for storing
 * all of the header parameters.
 *
 * Usage
 * -----
 * This class can be used like this:
 * ```
 * Parameter<float> param(3405, Metric::KILO, "W", "Power consumption of my battery");
 * std::cout << param.toHumanReadable() << std::endl;
 * ```
 * This code will output the string "3.405 MW". Note that you can use any
 * floating point type (float, double, or long double).
 *
 * You can also use the string variant of the class:
 * ```
 * Parameter<std::string> param("3405", "k", "W", "Power consumption of my battery");
 * // param.toHumanReadable() is invalid and will cause a compiler error
 * ```
 * The string variant behaves slightly differently. The conversion methods are
 * unavailable, and internally, the value is not converted to SI but it is
 * simply stored as a string, in this case, "3405". If you want to later
 * convert a ```Parameter<string>``` to SI or to a human readable value, you
 * can use the ```toParameter()``` method:
 * ```
 * Parameter<std::string> param("3405", "k", "W", "Power consumption of my battery");
 * Parameter<double> converted = param.toParameter<double>();
 * std::cout << converted.toSI() << std::endl;
 * ```
 * Here you can see we converted the string parameter into a floating point
 * parameter and then output the SI value of it. This code will output the
 * number "3405000".
 *
 * Exceptions
 * ----------
 * There are a few exceptions. Units of time can be constructed without having
 * to provide the unit. The "s" is automatically added when specifying a time
 * metric. Examples:
 * ```
 * Parameter<float> time1(15, Metric::SECONDS);    // time1.unit() will return "s"
 * Parameter<float> time2(42, Metric::DAYS);       // time2.unit() will return "s"
 * Parameter<float> time3(12, Metric::MILLI, "s"); // NOTE: The "s" is required here again, because MILLI is not a time metric.
 * ```
 *
 * Another exception is the decibels metric:
 * ```
 * Parameter<float> ratio(34, Metric::DECIBELS);   // ratio.unit() will return "dB".
 * ```
 *
 * When constructing weight, Metric::TONS will also automatically append "g":
 * ```
 * Parameter<float> weight1(5, Metric::TONS);       // weight1.unit() will return "g"
 * Parameter<float> weight2(3, Metric::KILO, "g");  // NOTE: The "g" is required to create kilo-grams.
 * ```
 */
template <class T>
class Parameter
{
    // Macro for forcing a floating point type for T. The function in question
    // should be templated with U and also passed to this macro.
#define ForcedFloat(T, U) typename enable_if<is_floating_point<T>::value, typename same_type<T, U>::type>::type
#define ForcedString(T, U) typename enable_if<is_string<T>::value, typename same_type<T, U>::type>::type

public:
    // This typedef will be either "Metric" if T is floating point, or "std::string" if T is not.
    typedef typename conditional<is_floating_point<T>::value, Metric, std::string>::type MetricType;

    /// Default constructor
    Parameter();

    /*!
     * @brief Try and construct from a string as best as possible. The value
     * and the metric as well as the unit is extracted automatically.
     * @param[in] value A value string such as "1.23 ml/s".
     */
    //Parameter(std::string value, std::string name="");

    /*!
     * @brief Construct from value, metric and unit.
     * @param[in] value The number to store.
     * @param[in] metric Specifies which metric the value has. This is later
     * used as a reference point when doing conversions.
     * @param[in] unit The unit string. Can be an empty string.
     */
    Parameter(T value, MetricType metric, std::string unit="", std::string name="");

    /*!
     * @brief Scales the number so it corresponds to the specified metric.
     * @note This function is only callable if the value is a floating point
     * type
     * @param[in] to If this is the only parameter you pass, then the value is
     * assumed to initially be SI (Metric::UNITY) and it will be scaled to the
     * metric specified by "to". For example, if you specify Metric::MILLI and
     * the value is "1" then the return value will be "1000".
     * @param[in] from Specifies the metric to scale from. This parameter is
     * optional. When not supplied, the "from" metric will be Metric::UNITY.
     * For example, toMetric(Metric::MILLI, Metric::KILO, 0.001) will return
     * 1000.
     * @param[in] value The value to scale. This parameter is optional, but is
     * required if "from" is specified. If this parameter is omitted, then the
     * internal value will be converted instead.
     */
    T toMetric(MetricType to) const { return toMetric<T>(to); }
    T toMetric(MetricType to, MetricType from, T value) const { return toMetric<T>(to, from, value); }

    /*!
     * @brief Scales the number such that its metric is unity (Metric=UNITY).
     * @note This function is only callable if the value is a floating point
     * type
     * @return Returns the scaled value as SI.
     */
    T toSI() const { return toSI<T>(); }

    /*!
     * @brief Converts a string parameter to that of a floating point parameter
     * @note This function is only callable if T is a string type and R is a
     * floating point type.
     */
    template <class R>
    Parameter<R> toParameter() const { return toParameterTimpl<T, R>(); }

    /*!
     * @brief Scales the number so it is pleasant to read. Ideal if you want
     * to output the parameter to a text field.
     * @note This function is only callable if the value is a floating point
     * type
     * @param[in] numSignificantDigits Limits the number of digits that are
     * added to the returned string. Specify nothing if you don't want to limit
     */
    std::string toHumanReadable(unsigned numSignificantDigits=0) const { return toHumanReadable<T>(numSignificantDigits); }
    T toHumanReadableScaleFactor(MetricType* convertedMetric = NULL) const { return toHumanReadableScaleFactor<T>(convertedMetric); }

    //! Returns the internally stored value.
    T value() const;
    //! Returns the original value that was used during construction
    T originalValue() const;
    //! Returns the internally stored metric. Will always be either Metric::UNITY, Metric::SECONDS or Metric::DECIBELS.
    MetricType metric() const;
    //! Returns the unit string of the parameter. Can be an empty string. This will be whatever was initially passed to the constructor.
    std::string unit() const;
    //! Returns the unit string of the parameter, converted to SI. This is the same as unit() except for kg. unit() will return "g", SIUnit() will return "kg".
    std::string SIUnit() const;
    //! Returns a descriptive name. Can be an empty string. This will be whatever was initially passed to the constructor.
    std::string name() const;

private:

    /*
     * The following functions rely on SFINAE to force T into being a floating
     * point type. If any of these functions are in the callgraph of one of the
     * public member functions when T is not a floating point type, then a
     * compiler error will be generated.
     */
    template <class U> void constructFloatFromString(std::string value, ForcedFloat(T, U)* = 0);
    template <class U> void constructFromValueAndMetric(T value, MetricType metric, ForcedFloat(T, U)* = 0);
    template <class U> void constructFromValueAndMetric(T value, MetricType metric, ForcedString(T, U)* = 0);

    template <class U> std::string toHumanReadable(unsigned numSignificantDigits, ForcedFloat(T, U)* = 0) const;
    template <class U> T toHumanReadableScaleFactor(MetricType* convertedMetric, ForcedFloat(T, U)* = 0) const;
    template <class U> T toHumanReadableScaleFactor_Normal(MetricType* convertedMetric, ForcedFloat(T, U)* = 0) const;
    template <class U> T toHumanReadableScaleFactor_Seconds(MetricType* convertedMetric, ForcedFloat(T, U)* = 0) const;

    template <class U> ForcedFloat(T, U) toMetric(MetricType to) const;
    template <class U> ForcedFloat(T, U) toMetric(MetricType to, MetricType from, T value) const;
    template <class U> ForcedFloat(T, U) toMetric_Normal(MetricType to, MetricType from, T value) const;
    template <class U> ForcedFloat(T, U) toMetric_Seconds(MetricType to, MetricType from, T value) const;

    template <class U> ForcedFloat(T, U) toSI() const;
    template <class U, class R> Parameter<R> toParameterTimpl(ForcedString(T, U)* = 0, ForcedFloat(R, R)* = 0) const;

    T value_;
    T originalValue_;
    MetricType metric_;
    std::string unit_;
    std::string name_;
};

} // namespace tfp
