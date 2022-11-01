/*
 * BLITSaw_controller.h
 *
 * Copyright (c) 2014, fukuroda (https://github.com/fukuroder)
 * Released under the MIT license
 */

#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;
using namespace Steinberg::Vst;
namespace MyVst {

	// controller
	class BLITSaw_controller : public EditController
	{
		// constructor(private)
		BLITSaw_controller();

	public:
		// create
		static FUnknown* create(void* context);

		// from EditController class
		virtual tresult PLUGIN_API initialize(FUnknown* context);
	};

	// feedback Parameter
	class BLITSaw_LeakParameter : public Parameter
	{
	public:
		BLITSaw_LeakParameter();
		virtual void toString(ParamValue normValue, String128 string)const;
	};
} // namespace
