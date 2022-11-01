#pragma once

#include "tfp/util/Parameter.hxx"
#include "tfp/math/Math.hpp"
#include "tfp/util/String.hpp"

#include <cstring>
#include <cstdlib>
#include <cmath>

namespace tfp {

typedef Parameter<float>       ParameterF;
typedef Parameter<double>      ParameterD;
typedef Parameter<std::string> ParameterS;

// ----------------------------------------------------------------------------
/* // TODO not actually required at the moment. Doesn't quite work yet
 * template <class T>
Parameter<T>::Parameter(std::string value, std::string name) :
    name_(name)
{
    constructFloatFromString<T>(value);
}*/

// ----------------------------------------------------------------------------
template <class T>
Parameter<T>::Parameter() :
    value_(T()),
    originalValue_(T())
{
}

// ----------------------------------------------------------------------------
template <class T>
Parameter<T>::Parameter(T value, MetricType metric, std::string unit, std::string name) :
    value_(value),
    originalValue_(value),
    unit_(unit),
    name_(name)
{
    constructFromValueAndMetric<T>(value, metric);
}

// ----------------------------------------------------------------------------
// TODO not actually required at the moment. Doesn't quite work yet
/*
template <class T>
template <class U>
void Parameter<T>::constructFloatFromString(std::string value, ForcedFloat(T, U)*)
{
    // Easiest things first, extract the value
    value_ = T(std::atof(value.c_str()));

    // Find first non-digit character that is not a whitespace
    const char* ptr = value.c_str();
    while(*ptr && (isdigit(*ptr) || isspace(*ptr)))
        ptr++;
    if(*ptr == '\0')
        return;

    // If string is only 1 character, it means there is no metric.
    if(strlen(ptr) == 1)
    {
        unit_ = ptr;
        return;
    }

    // Check if dB
    if(strcmp(ptr, "dB") == 0)
    {
        metric_ = Metric::DECIBELS;
        unit_ = "dB";
        return;
    }

    // Check if tons
    if(*ptr == 't')
    {
        value_ *= 1e6;
        metric_ = Metric::MEGA;
        unit_ = "g";
        return;
    }

    // Check if minutes
    if(strcmp(ptr, "min") == 0)
    {
        value_ *= 60;
        unit_ = "s";
    }

    // Check if hours
    if(*ptr == 'h')
    {
        value_ *= 3600;
        unit_ = "s";
    }

    // Check if days
    if(*ptr == 'd')
    {
        value_ *= 3600 * 24;
        unit_ = "s";
    }

     *
     * At this point all exceptions have been checked. We must account for all
     * of the normal cases now.
     *

    // See if the first character pair(s) match any entries in our metric table
    unsigned iMetric = 0;
    extern const char** g_MetricTable;
    while(iMetric <= Metric::EXA)
    {
        if(str::strcmpoff(ptr, g_MetricTable[iMetric]) > 0)
            break;
        iMetric++;
    }

    // We've found a valid metric, convert to SI
    if(iMetric <= Metric::EXA)
    {
        metric_ = static_cast<Metric>(iMetric);
        value_ = toMetric(Metric::UNITY);
        metric_ = Metric::UNITY;
    }

    // The rest of the string is the unit
    ptr++;
    unit_ = ptr;
}*/

// ----------------------------------------------------------------------------
template <class T>
template <class U>
void Parameter<T>::constructFromValueAndMetric(T value, MetricType metric, ForcedFloat(T, U)*)
{
    /*
     * All functions assume the value is SI in the "normal" cases. The only
     * units that are the exception to this rule are decibels (in which case
     * the internally stored metric is Metric::DECIBELS) and seconds (in which
     * case the internal metric is Metric::SECONDS). Metric::UNITY is
     * synonymous with Metric::SECONDS.
     */

    // handle the special cases
    if(metric == Metric::DECIBELS)
    {
        unit_ = "dB";
        metric_ = Metric::DECIBELS;
        return;
    }
    if(metric == Metric::TONS)
    {
        unit_ = "g";
        metric = Metric::MEGA;
    }
    // If user has specified "s" as the unit, convert the quantity to seconds
    if(unit_ == "s" && (metric < Metric::SECONDS || metric > Metric::DAYS))
    {
        value_ = toMetric(Metric::UNITY, metric, value_);
        metric_ = Metric::SECONDS;
        return;
    }
    // If the user has not specified "s" but has specified Metric::SECONDS,
    // set the unit appropriately.
    if(metric >= Metric::SECONDS && metric <= Metric::DAYS)
    {
        metric_ = Metric::SECONDS;
        unit_ = "s";
    }

    // convert from the passed in metric to unity. The value will then be SI.
    value_ = toMetric(Metric::UNITY, metric, value_);
}

template <class T>
template <class U>
void Parameter<T>::constructFromValueAndMetric(T value, MetricType metric, ForcedString(T, U)*)
{
    metric_ = metric;
    value_ = value;
}

// ----------------------------------------------------------------------------
template <class T>
template <class U>
ForcedFloat(T, U) Parameter<T>::toMetric(MetricType metric) const
{
    return toMetric(metric, Metric::UNITY, value_);
}

template <class T>
template <class U>
ForcedFloat(T, U) Parameter<T>::toMetric(MetricType to, MetricType from, T value) const
{

    // decibels cannot be converted
    if(metric_ == Metric::DECIBELS || to == Metric::DECIBELS || from == Metric::DECIBELS)
        return value;

    // convert seconds
    if(metric_ == Metric::SECONDS)
        return toMetric_Seconds<T>(to, from, value);

    // tons are really just 10e6
    if(to == Metric::TONS)
        to = Metric::MEGA;
    if(from == Metric::TONS)
        from = Metric::TONS;

    // do a normal conversion
    return toMetric_Normal<T>(to, from, value);
}

// ----------------------------------------------------------------------------
template <class T>
template <class U>
ForcedFloat(T, U) Parameter<T>::toMetric_Normal(MetricType to, MetricType from, T value) const
{
    assert(to >= Metric::ATTO && to <= Metric::EXA);

    while(from != to)
    {
        if(from < to)
        {
            value /= T(1000);
            ++from;
        }
        else
        {
            value *= T(1000);
            --from;
        }
    }

    return value;
}

// ----------------------------------------------------------------------------
template <class T>
template <class U>
ForcedFloat(T, U) Parameter<T>::toMetric_Seconds(MetricType to, MetricType from, T value) const
{
    assert(unit_ == "s");
    assert(
        (to >= Metric::SECONDS && to <= Metric::DAYS) ||
        (to >= Metric::ATTO && to <= Metric::EXA)
    );
    assert(
        (from >= Metric::SECONDS && from <= Metric::DAYS) ||
        (from >= Metric::ATTO && from <= Metric::EXA)
    );

    /*
     * Seconds are an interesting unit, because we handle it empirically if
     * the value is greater than 1s, but metrically if the value is less than
     * 1s. If we cross this 1s "boundary" during conversion we have to perform
     * two conversions.
     */

    if(to == Metric::UNITY)
        to = Metric::SECONDS;
    if(from == Metric::UNITY)
        from = Metric::SECONDS;
    if(to >= Metric::SECONDS && from <= Metric::UNITY)
    {
        value = toMetric_Normal<T>(Metric::UNITY, from, value);
        from = Metric::SECONDS;
    }
    if(from >= Metric::SECONDS && to <= Metric::UNITY)
    {
        value = toMetric_Normal<T>(to, Metric::UNITY, value);
        to = Metric::SECONDS;
    }

    while(from < to)
    {
        if(from == Metric::HOURS) // seconds and minutes
        {
            value /= T(24);
            ++from;
        }
        if(from < Metric::HOURS)
        {
            value /= T(60);
            ++from;
        }
    }
    while(from > to)
    {
        if(from < Metric::DAYS)
        {
            value *= T(60);
            --from;
        }
        if(from == Metric::DAYS)
        {
            value *= T(24);
            --from;
        }
    }

    return value;
}

// ----------------------------------------------------------------------------
template <class T>
template <class U>
ForcedFloat(T, U) Parameter<T>::toSI() const
{
    /*
     * The internal value is always converted to Metric::UNITY in the
     * constructor. This corresponds to SI in all cases except for kilo-grams.
     */
    if(unit_ == "g")
        return value_ * 0.001;

    return value_;
}

// ----------------------------------------------------------------------------
template <class T>
template <class U, class R>
Parameter<R> Parameter<T>::toParameterTimpl(ForcedString(T, U)*, ForcedFloat(R, R)*) const
{
    R value = static_cast<R>(std::atof(value_));
    Metric metric = Metric::fromString(unit_);
    if(metric == Metric::UNITY)
        metric = Metric::fromString(metric_);
    return Parameter<R>(value, metric, unit_, name_);
}

// ----------------------------------------------------------------------------
template <class T>
template <class U>
std::string Parameter<T>::toHumanReadable(unsigned numSignificantDigits, ForcedFloat(T, U)*) const
{
    // Perform conversion and store the resulting metric
    MetricType convertedMetric;
    T convertedValue = value_ * toHumanReadableScaleFactor<T>(&convertedMetric);
    std::string convertedStr = str::to_string(convertedValue, numSignificantDigits);

    // Special case if more than 60 seconds
    if(metric_ == Metric::SECONDS && std::fabs(value_) >= 1)
        return convertedStr + " " + convertedMetric.toString();

    // Special case if tons
    if(unit_ == "g" && convertedMetric == Metric::MEGA)
        return convertedStr + " t";

    return convertedStr + " " + convertedMetric.toString() + unit_;
}

// ----------------------------------------------------------------------------
template <class T>
template <class U>
T Parameter<T>::toHumanReadableScaleFactor(MetricType* convertedMetric, ForcedFloat(T, U)*) const
{
    // decibels cannot be converted
    if(metric_ == Metric::DECIBELS)
        return T(1);

    // Check for zero and infinity before converting.
    if(fpu_error(value_) || value_ == 0)
        return value_;

    if(metric_ == Metric::SECONDS)
        return toHumanReadableScaleFactor_Seconds<T>(convertedMetric);
    return toHumanReadableScaleFactor_Normal<T>(convertedMetric);
}

// ----------------------------------------------------------------------------
template <class T>
template <class U>
T Parameter<T>::toHumanReadableScaleFactor_Normal(MetricType* convertedMetric, ForcedFloat(T, U)*) const
{
    T convertedValue = value_;
    MetricType currentMetric = Metric::UNITY;

    while(std::fabs(convertedValue) < 1)
    {
        convertedValue *= T(1000);
        --currentMetric;
    }
    while(std::fabs(convertedValue) >= 1000)
    {
        convertedValue /= T(1000);
        ++currentMetric;
    }

    if(convertedMetric)
        *convertedMetric = currentMetric;

    return convertedValue / value_;
}

// ----------------------------------------------------------------------------
template <class T>
template <class U>
T Parameter<T>::toHumanReadableScaleFactor_Seconds(MetricType* convertedMetric, ForcedFloat(T, U)*) const
{
    assert(unit_ == "s");

    /*
     * Seconds are an interesting unit, because we handle it empirically if
     * the value is greater than 1s, but metrically if the value is less than
     * 1s. If we cross this 1s "boundary" during conversion we have to perform
     * two conversions.
     */

    // Handle units less than 1 second
    if(std::fabs(value_) < 1)
        return toHumanReadableScaleFactor_Normal<T>(convertedMetric);

    T convertedValue = value_;
    MetricType currentMetric = Metric::SECONDS;

    // Handle all units greater than 1 seconds
    while(fabs(convertedValue) >= 60 && currentMetric <= Metric::MINUTES)
    {
        convertedValue /= T(60);
        ++currentMetric;
    }
    if(fabs(convertedValue) >= 24 && currentMetric == Metric::HOURS)
    {
        convertedValue /= T(24);
        ++currentMetric;
    }

    if(convertedMetric)
        *convertedMetric = currentMetric;

    return convertedValue / value_;
}

/*
// ----------------------------------------------------------------------------
template <class T>
template <class U>
std::string Parameter<T>::toHumanReadable(unsigned numSignificantDigits, ForcedFloat(T, U)*) const
{
    if(unit_ == "")
        return str::to_string(value_, numSignificantDigits);

    // decibels
    if(metric_ == Metric::DECIBELS)
        return str::to_string(value_, numSignificantDigits) + " dB";

    // Check for zero and infinity before converting.
    if(fpu_error(value_))
        return "inf " + unit_;
    if(value_ == 0)
        return "0 " + unit_;

    // special case with seconds
    if(metric_ == Metric::SECONDS)
        return toHumanReadable_seconds(numSignificantDigits);

    T convertedValue = value_;
    MetricType currentMetric = Metric::UNITY;
    while(abs(convertedValue) < 1 && currentMetric != Metric::ATTO)
    {
        convertedValue = toMetricNormal(currentMetric - 1, currentMetric, convertedValue);
        --currentMetric;
    }
    while(abs(convertedValue) >= 1000 && currentMetric != Metric::EXA)
    {
        convertedValue = toMetricNormal(currentMetric + 1, currentMetric, convertedValue);
        ++currentMetric;
    }

    // special case for 1000 kg (which is 1 ton)
    if(unit_ == "g" && currentMetric == Metric::MEGA)
        return str::to_string(convertedValue, numSignificantDigits) + " t";

    return str::to_string(convertedValue, numSignificantDigits) + " " + currentMetric.toString() + unit_;
}

// ----------------------------------------------------------------------------
template <class T>
std::string Parameter<T>::toHumanReadable_seconds(unsigned numSignificantDigits) const
{
    T convertedValue = toSI(); // in seconds
    Metric currentMetric = Metric::SECONDS;

    while(abs(convertedValue) < 1 && currentMetric != Metric::ATTO)
    {
        MetricType nextMetric = currentMetric - 1;
        if(currentMetric == Metric::SECONDS)
            nextMetric = Metric::MILLI;
        convertedValue = toMetric(nextMetric, currentMetric, convertedValue);
        currentMetric = nextMetric;
    }
    while(abs(convertedValue) < 1 && currentMetric != Metric::DAYS)
    {
        MetricType nextMetric = currentMetric + 1;
        if(currentMetric == Metric::UNITY)
            nextMetric = Metric::MINUTES;

        if(abs(convertedValue) > 1)
            break;

        convertedValue = toMetric(nextMetric, currentMetric, convertedValue);
        currentMetric = nextMetric;
    }

    return str::to_string(convertedValue, numSignificantDigits) + " " + currentMetric.toString() + unit_;
}*/

// ----------------------------------------------------------------------------
template <class T>
T Parameter<T>::value() const
{
    return value_;
}

// ----------------------------------------------------------------------------
template <class T>
T Parameter<T>::originalValue() const
{
    return originalValue_;
}

// ----------------------------------------------------------------------------
template <class T>
typename Parameter<T>::MetricType Parameter<T>::metric() const
{
    return metric_;
}

// ----------------------------------------------------------------------------
template <class T>
std::string Parameter<T>::unit() const
{
    return unit_;
}

// ----------------------------------------------------------------------------
template <class T>
std::string Parameter<T>::SIUnit() const
{
    if(unit_ == "g")
        return "k" + unit_;
    return unit_;
}

// ----------------------------------------------------------------------------
template <class T>
std::string Parameter<T>::name() const
{
    return name_;
}

} // namespace tfp
