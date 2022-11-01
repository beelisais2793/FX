#ifndef LADSPAMM_WORLD_1_HH
#define LADSPAMM_WORLD_1_HH

#include <string>
#include <vector>

#include <ladspamm-1/ladspamm.h>

namespace ladspamm1
{
	/**
	 * @brief This class represents the world of LADSPA plugins.
	 */
	struct world
	{	
		/**
		 * @brief The libraries found in the system.
		 */
		const std::vector<library_ptr> libraries;
		
		/**
		 * @brief This constructor scans the system (parsing LADSPA_PATH or
		 * using sensible defaults).
		 */
		world(std::string path = get_path_from_environment())
		:
			libraries(world_scan(path))
		{
			
		}
	};
} // namespace

#endif
