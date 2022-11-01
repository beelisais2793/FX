%{
#include "Gamma/FormantData.h"
%}

namespace gam
{
    class Vowel{
    public:

        /// Voice types
        enum Voice{
            MAN=0,		/**< */
            WOMAN=1,	/**< */
            CHILD=2,	/**< */
            NUM_VOICES
        };

        /// Phonemes
        enum Phoneme{
            HEED=0,		/**< */
            HID=1,		/**< */
            HEAD=2,		/**< */
            HAD=3,		/**< */
            HOD=4,		/**< */
            HAWED=5,	/**< */
            HOOD=6,		/**< */
            WHOD=7,		/**< */
            HUD=8,		/**< */
            HEARD=9,	/**< */
            NUM_PHONEMES
        };

        /// Get amplitude of filter
        static float amp(Voice v, Phoneme p, int i);
        static float dB(Voice v, Phoneme p, int i);
        static float freq(Voice v, Phoneme p, int i);
        static float pitch(Voice v, Phoneme p, int i);
    };
}