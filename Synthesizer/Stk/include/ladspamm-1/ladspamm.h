#ifndef LADSPAMM_LADSPA_WORLD_1_HH
#define LADSPAMM_LADSPA_WORLD_1_HH

#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <ladspamm-1/dl.h>
#include <ladspamm-1/library.h>

/**
 * \mainpage ladspamm-1
 *
 * @brief ladspamm is a simple C++ library for handling (finding, loading 
 * and instantiating) LADSPA plugins.	
 */

/**
 * \namespace ladspamm1
 * 
 * @brief All ladspamm classes and functions are in this namespace.
 */
namespace ladspamm1
{
	/**
	 * @brief Utility function to get the LADSPA_PATH environment variable.
	 */
	inline std::string get_path_from_environment(std::string environment_variable_name = "LADSPA_PATH")
	{
		char *path = getenv(environment_variable_name.c_str());
		
		if (NULL == path) 
		{
			return "/usr/lib/ladspa:/usr/local/lib/ladspa";
		}
		
		return std::string(path);
	}

	/**
	 * @brief Utility function to split a path into components.
	 */
	inline std::vector<std::string> split_path(std::string path, char separator) 
	{
		std::vector<std::string> components;
		std::stringstream stream(path);
		
		std::string component;
		while(std::getline(stream, component, separator))
		{
			components.push_back(component);
		}
		
		return components;
	}

	/**
	 * @brief Find all libraries in LADSPA_PATH.
	 */
	inline std::vector<library_ptr> world_scan
	(
		std::string path
	)
	{
		std::vector<std::string> path_components = split_path(path, ':');
	
		std::vector<library_ptr> libraries;
		
		try
		{
			for (unsigned int index = 0; index < path_components.size(); ++index) 
			{
				// std::cerr << "LADSPA_PATH component: " << path_components[index] << std::endl;
				
				boost::filesystem::path path(path_components[index]);
				
				if (false == boost::filesystem::is_directory(path))
				{
					continue;
				}
				
				for 
				(
					boost::filesystem::directory_iterator it = boost::filesystem::directory_iterator(path);
					it != boost::filesystem::directory_iterator();
					++it
				)
				{
					// std::cerr << "LADSPA library: " << (*it).path().c_str() << std::endl;
					try
					{
						dl_ptr the_dl(new dl((*it).path().c_str()));
						libraries.push_back(library_ptr(new library(the_dl)));
					}
					catch (std::runtime_error &e) 
					{
						std::cerr << "Warning: could not load library: " << (*it).path().c_str() << " " << e.what() << std::endl;
					}
				}
			}
		}
		catch (boost::filesystem::filesystem_error &e) 
		{
			throw std::runtime_error(e.what());
		}
		
		return libraries;
	}
} // namespace

#endif
