#pragma once

#include "tfp/config.hpp"

#include <string>


namespace tfp {

class TFP_PUBLIC_API Metric
{
public:

    enum Metric_e
    {
        ATTO = 0,
        FEMTO,
        PICO,
        NANO,
        MICRO,
        MILLI,
        UNITY,
        KILO,
        MEGA,
        GIGA,
        TERA,
        PETA,
        EXA,
        // Exceptions start here
        DECIBELS,
        TONS,
        SECONDS,
        MINUTES,
        HOURS,
        DAYS
    };

    /// Default constructor, metric is unity by default.
    Metric();

    // Construct from enum (and implicitly copy construct)
    Metric(Metric_e metric);

    static Metric fromString(const char* str);
    static Metric fromString(const std::string& str) { return fromString(str.c_str()); }

    static unsigned metricStringOffset(const char* str);
    static unsigned metricStringOffset(const std::string& str) { return metricStringOffset(str.c_str()); }

    /// Allow assignments from integers
    Metric& operator=(unsigned rhs);

    // Allow adding integers to metric
    Metric& operator+=(unsigned rhs);

    // Allow subtracting integers from metric
    Metric& operator-=(unsigned rhs);

    /// Allow incrementing metric. The value is clamped internally to the last item in the enum.
    Metric& operator++();

    /// Allow incrementing metric. The value is clamped internally to the last item in the enum.
    Metric operator++(int);

    /// Allow decrementing metric. The value is clamped internally to the last item in the enum.
    Metric& operator--();

    /// Allow decrementing metric. The value is clamped internally to the last item in the enum.
    Metric operator--(int);

    /// Convert the metric to a short string (examples: "n" for nano, "k" for kilo, etc.)
    const char* toString() const;

    unsigned toUInt() const;

    // For allowing comparison operators to access private member data.
    friend bool operator==(const Metric&, const Metric&);
    friend bool operator< (const Metric&, const Metric&);
    friend bool operator==(const Metric&, unsigned);
    friend bool operator< (const Metric&, unsigned);
    friend bool operator==(unsigned, const Metric&);
    friend bool operator< (unsigned, const Metric&);

private:
    Metric_e metric_;
    static const char* metricTable[];
};

inline Metric operator+(Metric lhs, unsigned rhs)
{
    lhs += rhs;
    return lhs;
}

inline Metric operator-(Metric lhs, unsigned rhs)
{
    lhs -= rhs;
    return lhs;
}

inline bool operator==(const Metric& lhs, const Metric& rhs) { return lhs.metric_ == rhs.metric_;                  }
inline bool operator!=(const Metric& lhs, const Metric& rhs) { return !operator==(rhs, lhs);                       }
inline bool operator< (const Metric& lhs, const Metric& rhs) { return lhs.metric_ < rhs.metric_;                   }
inline bool operator> (const Metric& lhs, const Metric& rhs) { return operator< (rhs, lhs);                        }
inline bool operator<=(const Metric& lhs, const Metric& rhs) { return !operator> (lhs, rhs);                       }
inline bool operator>=(const Metric& lhs, const Metric& rhs) { return !operator< (lhs, rhs);                       }

inline bool operator==(const Metric& lhs, unsigned rhs) { return static_cast<unsigned>(lhs.metric_) == rhs;        }
inline bool operator!=(const Metric& lhs, unsigned rhs) { return !operator==(lhs, rhs);                            }
inline bool operator< (const Metric& lhs, unsigned rhs) { return static_cast<unsigned>(lhs.metric_) < rhs;         }
inline bool operator> (const Metric& lhs, unsigned rhs) { return operator< (rhs, lhs);                             }
inline bool operator<=(const Metric& lhs, unsigned rhs) { return !operator> (lhs, rhs);                            }
inline bool operator>=(const Metric& lhs, unsigned rhs) { return !operator< (lhs, rhs);                            }

inline bool operator==(unsigned lhs, const Metric& rhs) { return lhs == static_cast<unsigned>(rhs.metric_);        }
inline bool operator!=(unsigned lhs, const Metric& rhs) { return !operator==(lhs, rhs);                            }
inline bool operator< (unsigned lhs, const Metric& rhs) { return lhs < static_cast<unsigned>(rhs.metric_);         }
inline bool operator> (unsigned lhs, const Metric& rhs) { return operator< (rhs, lhs);                             }
inline bool operator<=(unsigned lhs, const Metric& rhs) { return !operator> (lhs, rhs);                            }
inline bool operator>=(unsigned lhs, const Metric& rhs) { return !operator< (lhs, rhs);                            }

inline bool operator==(const Metric::Metric_e lhs, const std::string& rhs) { return Metric(lhs).toString() == rhs; }
inline bool operator!=(const Metric::Metric_e lhs, const std::string& rhs) { return !operator==(lhs, rhs); }

inline bool operator==(const std::string& lhs, const Metric::Metric_e rhs) { return lhs == Metric(rhs).toString(); }
inline bool operator!=(const std::string& lhs, const Metric::Metric_e rhs) { return !operator==(lhs, rhs);         }

} // namespace tfp
