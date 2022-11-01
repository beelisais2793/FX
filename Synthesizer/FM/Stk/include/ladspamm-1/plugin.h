#ifndef LADSPAMM_PLUGIN_1_HH
#define LADSPAMM_PLUGIN_1_HH

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <ladspa.h>
#include <iostream>
#include <cmath>

#include <ladspamm-1/dl.h>

namespace ladspamm1
{
	/**
	 * @brief Represents a LADSPA plugin. See plugin_instance
	 * for creating instances
	 */
	struct plugin
	:
		boost::noncopyable
	{
		dl_ptr the_dl;
		const LADSPA_Descriptor *descriptor;
		
		plugin(dl_ptr the_dl, const LADSPA_Descriptor *descriptor)
		:
			the_dl(the_dl),
			descriptor(descriptor)
		{

		}
		
		std::string label() 
		{
			return descriptor->Label;
		}
		
		std::string name()
		{
			return descriptor->Name;
		}
		
		std::string maker()
		{
			return descriptor->Maker;
		}
		
		unsigned long uid()
		{
			return descriptor->UniqueID;
		}
		
		unsigned long port_count()
		{
			return descriptor->PortCount;
		}
		
		std::string port_name(unsigned int index)
		{
			return descriptor->PortNames[index];
		}
		
		bool port_is_input(unsigned int index)
		{
			return LADSPA_IS_PORT_INPUT(descriptor->PortDescriptors[index]);
		}

		bool port_is_output(unsigned int index)
		{
			return LADSPA_IS_PORT_OUTPUT(descriptor->PortDescriptors[index]);
		}

		bool port_is_control(unsigned int index)
		{
			return LADSPA_IS_PORT_CONTROL(descriptor->PortDescriptors[index]);
		}

		bool port_is_audio(unsigned int index)
		{
			return LADSPA_IS_PORT_AUDIO(descriptor->PortDescriptors[index]);
		}

		float port_lower_bound(unsigned int index)
		{
			return descriptor->PortRangeHints[index].LowerBound;
		}

		float port_upper_bound(unsigned int index)
		{
			return descriptor->PortRangeHints[index].UpperBound;
		}

		bool port_is_bounded_above(unsigned int index)
		{
			return LADSPA_IS_HINT_BOUNDED_BELOW(descriptor->PortRangeHints[index].HintDescriptor);
		}
		
		bool port_is_bounded_below(unsigned int index)
		{
			return LADSPA_IS_HINT_BOUNDED_BELOW(descriptor->PortRangeHints[index].HintDescriptor);
		}
		
		bool port_is_scaled_by_samplerate(unsigned int index)
		{
			return LADSPA_IS_HINT_SAMPLE_RATE(descriptor->PortRangeHints[index].HintDescriptor);
		}
		
		bool port_default_is_scaled_by_samplerate(unsigned int index)
		{
			return 
				port_is_scaled_by_samplerate(index)
				&&
				(
					   port_default_is_low(index)
					|| port_default_is_lower_bound(index)
					|| port_default_is_middle(index)
					|| port_default_is_high(index)
					|| port_default_is_upper_bound(index)
				);
		}
		
		bool port_is_logarithmic(unsigned int index)
		{
			return LADSPA_IS_HINT_LOGARITHMIC(descriptor->PortRangeHints[index].HintDescriptor);
		}
		
		bool port_is_integer(unsigned int index)
		{
			return LADSPA_IS_HINT_INTEGER(descriptor->PortRangeHints[index].HintDescriptor);
		}

		bool port_is_toggled(unsigned int index)
		{
			return LADSPA_IS_HINT_TOGGLED(descriptor->PortRangeHints[index].HintDescriptor);
		}

		bool port_has_default(unsigned int index)
		{
			return LADSPA_IS_HINT_HAS_DEFAULT(descriptor->PortRangeHints[index].HintDescriptor);
		}
		
		bool port_default_is_0(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_0(descriptor->PortRangeHints[index].HintDescriptor);
		}

		bool port_default_is_1(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_1(descriptor->PortRangeHints[index].HintDescriptor);
		}

		bool port_default_is_100(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_100(descriptor->PortRangeHints[index].HintDescriptor);
		}

		bool port_default_is_440(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_440(descriptor->PortRangeHints[index].HintDescriptor);
		}

		bool port_default_is_lower_bound(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_MINIMUM(descriptor->PortRangeHints[index].HintDescriptor);
		}
		
		bool port_default_is_upper_bound(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_MAXIMUM(descriptor->PortRangeHints[index].HintDescriptor);
		}

		bool port_default_is_low(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_LOW(descriptor->PortRangeHints[index].HintDescriptor);
		}
		
		bool port_default_is_high(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_HIGH(descriptor->PortRangeHints[index].HintDescriptor);
		}

		bool port_default_is_middle(unsigned int index)
		{
			return LADSPA_IS_HINT_DEFAULT_MIDDLE(descriptor->PortRangeHints[index].HintDescriptor);
		}
		
		bool has_run_adding()
		{
			return NULL != descriptor->run_adding;
		}
		
		bool has_set_run_adding_gain()
		{
			return NULL != descriptor->set_run_adding_gain;
		}
		
		/**
		 * @brief Not rounded even if port_is_integer. See plugin_instance::port_default()
		 */
		float port_default(unsigned int index)
		{
			if (false == port_has_default(index))
			{
				throw std::logic_error("Port has no default");
			}
			
			if (port_default_is_0(index))
			{
				return 0;
			}
			
			if (port_default_is_1(index))
			{
				return 1;
			}
			
			if (port_default_is_100(index))
			{
				return 100;
			}
			
			if (port_default_is_440(index))
			{
				return 440;
			}
			
			if (port_default_is_lower_bound(index))
			{
				return port_lower_bound(index);
			}

			if (port_default_is_upper_bound(index))
			{
				return port_upper_bound(index);
			}
			
			if (port_default_is_middle(index))
			{
				if (port_is_logarithmic(index))
				{
					return expf(logf(port_lower_bound(index)) * 0.5f + logf(port_upper_bound(index)) * 0.5f);
				}
				else
				{
					return port_lower_bound(index) * 0.5f + port_upper_bound(index) * 0.5f;
				}
			}
			
			if (port_default_is_low(index))
			{
				if (port_is_logarithmic(index))
				{
					return expf(logf(port_lower_bound(index)) * 0.75f + logf(port_upper_bound(index)) * 0.25f);
				}
				else
				{
					return port_lower_bound(index) * 0.75f + port_upper_bound(index) * 0.25f;
				}
			}

			if (port_default_is_high(index))
			{
				if (port_is_logarithmic(index))
				{
					return expf(logf(port_lower_bound(index)) * 0.25f + logf(port_upper_bound(index)) * 0.75f);
				}
				else
				{
					return port_lower_bound(index) * 0.25f + port_upper_bound(index) * 0.75f;
				}
			}

			throw std::logic_error("Unhandled default case - this is a bug in ladspamm. Please report to the author..");
		}
	};
	
	typedef boost::shared_ptr<plugin> plugin_ptr;
} // namespace

#endif
