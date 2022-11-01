#ifndef LADSPAPP_LADSPA_PLUGIN_INSTANCE_1_HH
#define LADSPAPP_LADSPA_PLUGIN_INSTANCE_1_HH

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <ladspa.h>
#include <cmath>
#include <algorithm>
#include <stdexcept>

#include <ladspamm-1/plugin.h>

namespace ladspamm1
{
	/**
	 * @brief Represents an instance of a LADSPA plugin.
	 */
	struct plugin_instance 
	: 
		boost::noncopyable 
	{
		plugin_ptr the_plugin;
		LADSPA_Handle handle;
		unsigned long samplerate;
		
		/**
		 * @brief Some plugins make wrong assumptions about ports being connected before 
		 * activate() gets called, thus we'll connect them to a small array 
		 * to make them not crash
		 */
		float buf[8];
		
		plugin_instance
		(
			plugin_ptr the_plugin,
			unsigned long samplerate
		)
		:
			the_plugin(the_plugin),
			samplerate(samplerate)
		{
			handle = the_plugin->descriptor->instantiate(the_plugin->descriptor, samplerate);
			
			if (NULL == handle)
			{
				throw std::runtime_error("Failed to instantiate plugin " + the_plugin->the_dl->filename + " " + the_plugin->label());
			}
			
			/**
			 * Workaround for crashing plugins. see buf[] above
			 */
			for (unsigned int port_index = 0; port_index < the_plugin->port_count(); ++port_index)
			{
				connect_port(port_index, buf);
			}
		}
		
		~plugin_instance()
		{
			if (NULL != the_plugin->descriptor->cleanup)
			{
				the_plugin->descriptor->cleanup(handle);
			}
		}
		
		plugin_ptr plugin()
		{
			return the_plugin;
		}
		

		void activate() 
		{
			if (NULL != the_plugin->descriptor->activate)
			{
				the_plugin->descriptor->activate(handle);
			}
		}
		
		void deactivate()
		{
			if (NULL != the_plugin->descriptor->deactivate)
			{
				the_plugin->descriptor->deactivate(handle);
			}
		}
		
		void connect_port(unsigned long port_index, float *location)
		{
			the_plugin->descriptor->connect_port(handle, port_index, location);
		}
		
		/**
		 * @brief Scaled by the samplerate if nessecary.
		 */
		float port_lower_bound(unsigned int index)
		{
			if (false == the_plugin->port_is_bounded_below(index))
			{
				throw std::logic_error("Port has no lower bound");
			}
			
			if (true == the_plugin->port_is_scaled_by_samplerate(index))
			{
				return (float)samplerate * the_plugin->port_lower_bound(index);
			}
			
			return the_plugin->port_lower_bound(index);
		}
		
		/**
		 * @brief Scaled by the samplerate if nessecary.
		 */
		float port_upper_bound(unsigned int index)
		{
			if (false == the_plugin->port_is_bounded_above(index))
			{
				throw std::logic_error("Port has no lower bound");
			}

			if (true == the_plugin->port_is_scaled_by_samplerate(index))
			{
				return (float)samplerate * the_plugin->port_upper_bound(index);
			}
			
			return the_plugin->port_upper_bound(index);
		}
		
		/**
		 * @brief This function tries to do an educated
		 * guess when the plugin does silly things like
		 * specifying default values outside of bounds
		 * or no default at all
		 */
		float port_default_guessed(unsigned int index)
		{
			float guess = 0;
			
			if (the_plugin->port_has_default(index))
			{
				guess = port_default(index);
			}
			
			if (the_plugin->port_is_bounded_below(index))
			{
				guess = std::max(port_lower_bound(index), guess);
			}
			
			if (the_plugin->port_is_bounded_above(index))
			{
				guess = std::min(port_upper_bound(index), guess);
			}
			
			return guess;
		}
		
		/**
		 * @brief Rounded if port_is_integer() and scaled by
		 * samplerate if nessecary
		 */
		float port_default(unsigned int index)
		{
			if (the_plugin->port_is_integer(index))
			{
				return roundf(the_plugin->port_default(index));
			}
			
			if (the_plugin->port_default_is_scaled_by_samplerate(index))
			{
				return (float)samplerate * the_plugin->port_default(index);
			}
			
			return the_plugin->port_default(index);
		}
		
		void run(unsigned long nframes)
		{
			the_plugin->descriptor->run(handle, nframes);
		}
	};

	typedef boost::shared_ptr<plugin_instance> plugin_instance_ptr;
} // namespace
#endif
