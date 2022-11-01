#include "al_ext/openvr/al_OpenVRWrapper.hpp"

using namespace al;

bool OpenVRWrapper::init() {
  if (mInitialized) {
    std::cerr
        << "Another allolib application is using the HMD. Not runnning HMD"
        << std::endl;
    return false;
  }
  // Check whether there is an HMD plugged-in and the SteamVR runtime is
  // installed

#ifdef AL_EXT_OPENVR
  if (vr::VR_IsHmdPresent()) {
    std::cout << "An HMD was successfully found in the system" << std::endl;
    if (vr::VR_IsRuntimeInstalled()) {
      char runtime_path[256];
      uint32_t requiredSize;
      vr::VR_GetRuntimePath(runtime_path, 255, &requiredSize);
      if (requiredSize < 256) {
        std::cout << "Runtime correctly installed at '" << runtime_path << "'"
                  << std::endl;
      }
    } else {
      std::cout << "Runtime was not found." << std::endl;
      return false;
    }
  } else {
    std::cout << "No HMD was found in the system." << std::endl;
    return false;
  }

  if (!vr::VR_IsRuntimeInstalled()) {
    throw std::runtime_error(
        "Error : OpenVR Runtime not detected on the system");
  }

  // Load the SteamVR Runtime
  vr::HmdError err;
  vr_context = vr::VR_Init(&err, vr::EVRApplicationType::VRApplication_Scene);
  // vr_context = vr::VR_Init(&err,
  // vr::EVRApplicationType::VRApplication_Overlay);

  if (vr_context == nullptr) {
    std::cout << "Error while initializing SteamVR runtime. Error code is "
              << vr::VR_GetVRInitErrorAsSymbol(err) << std::endl;
    return false;
  } else {
    std::cout << "SteamVR runtime successfully initialized" << std::endl;
  }

  // Set Overlay Input Method // optional // Mengyu
  std::string m_strName = "SystemOverlay";
  std::string sKey = "MainKey.SystemOverlay";
  vr::VROverlayError overlayError = vr::VROverlay()->CreateOverlay(
      sKey.c_str(), m_strName.c_str(), &m_ulOverlayHandle);

  vr::VROverlay()->SetOverlayInputMethod(m_ulOverlayHandle,
                                         vr::VROverlayInputMethod_Mouse);
  vr::VROverlay()->SetOverlayFlag(
      m_ulOverlayHandle, vr::VROverlayFlags_SendVRTouchpadEvents, true);
  vr::VROverlay()->SetOverlayFlag(
      m_ulOverlayHandle, vr::VROverlayFlags_ShowTouchPadScrollWheel, true);
  // vr::VROverlayFlags_SendVRScrollEvents available in 1.1.3b but not
  // in 1.10.6b Documentation in 1.10.6b indicates that mouse events are sent
  // automatically if  vr::VROverlayInputMethod_Mouse is set, so there seems to
  // be no need for this any more. Untested.
  //  vr::VROverlay()->SetOverlayFlag(m_ulOverlayHandle,
  //                                  vr::VROverlayFlags_SendVRScrollEvents,
  //                                  true);

  //    LeftController = new Controller();
  //    RightController = new Controller();

  // Obtain some basic information given by the runtime
  int base_stations_count = 0;
  for (uint32_t td = vr::k_unTrackedDeviceIndex_Hmd;
       td < vr::k_unMaxTrackedDeviceCount; td++) {
    if (vr_context->IsTrackedDeviceConnected(td)) {
      vr::ETrackedDeviceClass tracked_device_class =
          vr_context->GetTrackedDeviceClass(td);
      std::string td_type = GetTrackedDeviceClassString(tracked_device_class);
      tracked_device_type[td] = td_type;
      std::cout << "Tracking device " << td << " is connected " << std::endl;
      vr::TrackedPropertyError trackingError;
      // TODO check tracking error
      std::cout << "  Device type: " << td_type << ". Name: "
                << GetTrackedDeviceString(vr_context, td,
                                          vr::Prop_TrackingSystemName_String,
                                          &trackingError)
                << std::endl;
      if (tracked_device_class ==
          vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference) {
        base_stations_count++;
      }
      if (td == vr::k_unTrackedDeviceIndex_Hmd) {
        // Fill variables used for obtaining the device name and serial ID (used
        // later for naming the SDL window)
        // TODO check tracking error
        driver_name = GetTrackedDeviceString(
            vr_context, vr::k_unTrackedDeviceIndex_Hmd,
            vr::Prop_TrackingSystemName_String, &trackingError);
        driver_serial = GetTrackedDeviceString(
            vr_context, vr::k_unTrackedDeviceIndex_Hmd,
            vr::Prop_SerialNumber_String, &trackingError);
      }
      std::cout << "Driver name: " << driver_name;
      std::cout << " Driver serial ID: " << driver_serial << std::endl;
    } else {
      std::cout << "Tracking device " << td << " not connected" << std::endl;
    }
  }

  if (!vr::VRCompositor()) {
    throw std::runtime_error("Unable to initialize VR compositor!\n ");
  }

  uint32_t width, height;
  vr_context->GetRecommendedRenderTargetSize(&width, &height);
  al::EasyFBOSetting setting{GL_RGBA8,         GL_RGBA,
                             GL_UNSIGNED_BYTE, GL_DEPTH_COMPONENT24,
                             GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
                             GL_CLAMP_TO_EDGE, GL_LINEAR,
                             GL_LINEAR};
  fboLeft.init(width, height, setting);
  fboRight.init(width, height, setting);
  std::clog << "Initialized HMD with suggested render target size : " << width
            << "x" << height << std::endl;

  // Check whether both base stations are found, not mandatory but just in
  // case...
  // if (base_stations_count < 2) {
  //  std::cout << "There was a problem indentifying the base stations, please
  //  check they are powered on" << endl; return false;
  //}

  projectionLeft = GetHMDMatrixProjectionEye(vr_context, vr::Eye_Left,
                                             m_fNearClip, m_fFarClip);
  projectionRight = GetHMDMatrixProjectionEye(vr_context, vr::Eye_Right,
                                              m_fNearClip, m_fFarClip);
  eyePosLeft = GetHMDMatrixPoseEye(vr_context, vr::Eye_Left);
  eyePosRight = GetHMDMatrixPoseEye(vr_context, vr::Eye_Right);

#endif
  mInitialized = true;
  return true;
}

bool OpenVRWrapper::update() {
#ifdef AL_EXT_OPENVR
  if (!vr_context) {
    return false;
  }
  // Process SteamVR events
  vr::VREvent_t vr_event;
  while (vr_context->PollNextEvent(&vr_event, sizeof(vr_event))) {
    process_vr_event(vr_event);
  }

  // Process Overlay events // optional
  // while( vr::VROverlay()->PollNextOverlayEvent( m_ulOverlayHandle, &vr_event,
  // sizeof( vr_event )  ) ){
  //     process_overlay_event(vr_event);
  // }

  // Obtain tracking device poses
  vr_context->GetDeviceToAbsoluteTrackingPose(
      vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0,
      tracked_device_pose, vr::k_unMaxTrackedDeviceCount);

  // region: Mengyu -> search for controller id if its not assigned
  if (LeftController.deviceID == -1) {
    LeftHand_index = vr_context->GetTrackedDeviceIndexForControllerRole(
        vr::TrackedControllerRole_LeftHand);
    //  std::cout << LeftHand_index << " device num " << std::endl;
    LeftController.deviceID = LeftHand_index;
    controllers[LeftHand_index] = &LeftController;
    // std::cout << "Left Controller ID "<< LeftHand_index << " = " <<
    // LeftControllerID << endl;
  }
  if (RightController.deviceID == -1) {
    RightHand_index = vr_context->GetTrackedDeviceIndexForControllerRole(
        vr::TrackedControllerRole_RightHand);
    RightController.deviceID = RightHand_index;
    controllers[RightHand_index] = &RightController;
    // std::cout << "Right Controller ID " << RightHand_index << " = " <<
    // RightControllerID << endl;
  }
  // end region

  int actual_y = 110, tracked_device_count = 0;
  for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; nDevice++) {
    if ((tracked_device_pose[nDevice].bDeviceIsConnected)) {
      // print_text(("Tracked device #" + ftos((float) nDevice,0) + " (" +
      // tracked_device_type[nDevice] + ")").c_str(), color, 10, actual_y); We
      // take just the translation part of the matrix (actual position of
      // tracked device, not orientation)
      if (tracked_device_pose[nDevice].bPoseIsValid) {
        Vec3f v = {
            tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[0][3],
            tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[1][3],
            tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[2][3]};

        // save hmd pos data
        if (nDevice == 0) {
          HMDPos = v;
        }
        // region mengyu-> save v3 pos data into controller pos variable
        if (nDevice == LeftController.deviceID) {
          LeftController.vel = v - LeftController.pose().pos();
          LeftController.pose().pos() = v;
        }

        if (nDevice == RightController.deviceID) {
          RightController.vel = v - RightController.pose().pos();
          RightController.pose().pos() = v;
        }
      }
      // Check whether the tracked device is a controller
      if (nDevice == LeftController.deviceID ||
          nDevice == RightController.deviceID) {
        vr::VRControllerState_t controller_state;
        if (vr_context->GetControllerState(nDevice, &controller_state,
                                           sizeof(controller_state))) {
          // assign touch pad values
          if (controllers[nDevice]->touchpadTouched) {
            float tPos[2] = {controller_state.rAxis[0].x,
                             controller_state.rAxis[0].y};
            if (controllers[nDevice]->touchPos.mag() != 0.0)
              controllers[nDevice]->touchVel =
                  Vec2f(tPos) - controllers[nDevice]->touchPos;
            controllers[nDevice]->touchPos = tPos;
            // std::cout << nDevice << " ndevice " <<
            // controllers[nDevice]->touchPos.x <<
            // controllers[nDevice]->touchPos.y << std::endl;
          } else if (!controllers[nDevice]->touchpadTouched) {
            controllers[nDevice]->touchVel = (0, 0);
            controllers[nDevice]->touchPos = (0, 0);
            // std::cout << nDevice << " ndevice " <<
            // controllers[nDevice]->touchPos.x <<
            // controllers[nDevice]->touchPos.y << std::endl;
          }
          // assign trigger pressure value and set triggered value based on
          // custom threshold
          if (controllers[nDevice]->triggerTouched ||
              controllers[nDevice]->triggerPressed) {
            controllers[nDevice]->triggerPressure = controller_state.rAxis[1].x;
            if (controllers[nDevice]->triggerPressure >=
                controllers[nDevice]->triggerThreshold) {
              controllers[nDevice]->triggered = true;
              // std::cout << nDevice << " triggered "<<
              // controllers[nDevice]->triggerPressure << std::endl;
            } else {
              controllers[nDevice]->triggered = false;
            }
            // std::cout << nDevice << " ndevice " <<
            // controllers[nDevice]->triggerPressure << std::endl;
          } else if (!controllers[nDevice]->triggerTouched &&
                     !controllers[nDevice]->triggerPressed) {
            controllers[nDevice]->triggered = false;
            controllers[nDevice]->triggerPressure = 0.0f;
            // std::cout << nDevice << " ndevice " <<
            // controllers[nDevice]->triggerPressure << std::endl;
          }
          controllers[nDevice]->buttonsPress =
              controller_state.ulButtonPressed &
              ~controllers[nDevice]->buttonsDown;
          controllers[nDevice]->buttonsRelease =
              ~controller_state.ulButtonPressed &
              controllers[nDevice]->buttonsDown;
          controllers[nDevice]->buttonsDown = controller_state.ulButtonPressed;
          // ((vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Axis1) &
          // controller_state.ulButtonPressed) == 0) ? color = green : color =
          // blue;
        }
      }

      // //end region

      // print_text(vftos(v,2).c_str(), color, 50, actual_y+25);
      actual_y += 60;
      tracked_device_count++;
    }
  }

  auto *m_rTrackedDevicePose = tracked_device_pose;
  vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose,
                                   vr::k_unMaxTrackedDeviceCount, nullptr, 0);
  m_iValidPoseCount = 0;
  m_strPoseClasses = "";
  for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice) {
    if (m_rTrackedDevicePose[nDevice].bPoseIsValid) {
      m_iValidPoseCount++;
      m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToAlMat4f(
          m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
      if (m_rDevClassChar[nDevice] == 0) {
        switch (vr_context->GetTrackedDeviceClass(nDevice)) {
        case vr::TrackedDeviceClass_Controller:
          m_rDevClassChar[nDevice] = 'C';
          break;
        case vr::TrackedDeviceClass_HMD:
          m_rDevClassChar[nDevice] = 'H';
          break;
        case vr::TrackedDeviceClass_Invalid:
          m_rDevClassChar[nDevice] = 'I';
          break;
        case vr::TrackedDeviceClass_GenericTracker:
          m_rDevClassChar[nDevice] = 'G';
          break;
        case vr::TrackedDeviceClass_TrackingReference:
          m_rDevClassChar[nDevice] = 'T';
          break;
        default:
          m_rDevClassChar[nDevice] = '?';
          break;
        }
      }
      m_strPoseClasses += m_rDevClassChar[nDevice];

      // region: Mengyu -> assign controller matrix to custom controller
      // variables if active
      if (tracked_device_type[nDevice] == "controller") {
        // controllers[nDevice]->pose =  m_rmat4DevicePose[nDevice];
        // al::invert(controllers[nDevice]->pose);
        // controllers[nDevice]->quat =
        // al::Quatf().fromMatrix(controllers[nDevice]->pose);
        if (nDevice == RightController.deviceID) {
          // std::cout <<  nDevice << " is " << "updating right controller
          // matrix" << endl;
          RightController.mat = m_rmat4DevicePose[nDevice];
          al::invert(RightController.mat);
          // RightHandQuat = ConvertAlMat4fToAlQuatf(RightHandPose);
          RightController.pose().quat().fromMatrix(RightController.mat);
        } else if (nDevice == LeftController.deviceID) {
          // std::cout << nDevice << " is " << "updating left controller matrix"
          // << endl;
          LeftController.mat = m_rmat4DevicePose[nDevice];
          al::invert(LeftController.mat);
          LeftController.pose().quat().fromMatrix(LeftController.mat);
        } else {
        }
      }
      // end region
    }
  }

  if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
    HMDMat = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
    al::invert(HMDMat);
    HMDQuat = al::Quatf().fromMatrix(HMDMat);
  }

  return true;
#else
  return false;
#endif
}

void OpenVRWrapper::draw(std::function<void(Graphics &)> drawingFunction,
                         Graphics &g) {
#ifdef AL_EXT_OPENVR
  if (!vr_context) {
    return;
  }
  auto viewLeft = eyePosLeft * HMDMat;
  auto projLeft = projectionLeft;

  auto viewRight = eyePosRight * HMDMat;
  auto projRight = projectionRight;

  drawVREye(drawingFunction, g, fboLeft, viewLeft, projLeft);
  drawVREye(drawingFunction, g, fboRight, viewRight, projRight);

  finishDraw(g);
#endif
}

void OpenVRWrapper::close() {
#ifdef AL_EXT_OPENVR
  if (vr_context) {
    vr::VR_Shutdown();
    vr_context = nullptr;
  }
#endif
  mInitialized = false;
}

void OpenVRWrapper::drawVREye(std::function<void(Graphics &)> drawingFunction,
                              Graphics &g, EasyFBO &fbo, Mat4f &view,
                              Mat4f &proj) {
  g.pushFramebuffer(fbo);
  g.pushViewport(fbo.width(), fbo.height());
  g.pushProjMatrix(proj);
  g.pushViewMatrix(view);
  g.pushMatrix();

  // Put draw code here ----------------------------------------------

  drawingFunction(g);

  // -----------------------------------------------------------------

  g.popMatrix();
  g.popProjMatrix();
  g.popViewMatrix();
  g.popViewport();
  g.popFramebuffer();
}

void OpenVRWrapper::finishDraw(Graphics &g) {
#ifdef AL_EXT_OPENVR
  if (!vr_context)
    throw std::runtime_error(
        "Error : presenting frames when VR system handle is NULL");
  vr::EColorSpace colorSpace = vr::ColorSpace_Gamma;
  vr::Texture_t leftEyeTexture = {(void *)(uintptr_t)fboLeft.tex().id(),
                                  vr::TextureType_OpenGL, colorSpace};
  vr::Texture_t rightEyeTexture = {(void *)(uintptr_t)fboRight.tex().id(),
                                   vr::TextureType_OpenGL, colorSpace};
  auto errl = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
  auto errr = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
  if (errl != 0 || errr != 0)
    std::cout << errl << ", from error message " << errr << std::endl;
  vr::VRCompositor()->PostPresentHandoff();
#endif
}

#ifdef AL_EXT_OPENVR
Mat4f OpenVRWrapper::ConvertSteamVRMatrixToAlMat4f(
    const vr::HmdMatrix44_t &matPose) {
  return al::Mat4f{
      matPose.m[0][0], matPose.m[0][1], matPose.m[0][2], matPose.m[0][3],
      matPose.m[1][0], matPose.m[1][1], matPose.m[1][2], matPose.m[1][3],
      matPose.m[2][0], matPose.m[2][1], matPose.m[2][2], matPose.m[2][3],
      matPose.m[3][0], matPose.m[3][1], matPose.m[3][2], matPose.m[3][3],
  };
}

Mat4f al::OpenVRWrapper::ConvertSteamVRMatrixToAlMat4f(
    const vr::HmdMatrix34_t &matPose) {
  return al::Mat4f{matPose.m[0][0],
                   matPose.m[0][1],
                   matPose.m[0][2],
                   matPose.m[0][3],
                   matPose.m[1][0],
                   matPose.m[1][1],
                   matPose.m[1][2],
                   matPose.m[1][3],
                   matPose.m[2][0],
                   matPose.m[2][1],
                   matPose.m[2][2],
                   matPose.m[2][3],
                   0.0f,
                   0.0f,
                   0.0f,
                   1.0f};
}

Mat4f OpenVRWrapper::GetHMDMatrixProjectionEye(vr::IVRSystem *vrsys,
                                               vr::Hmd_Eye nEye, float nearClip,
                                               float farClip) {
  if (!vrsys)
    return al::Mat4f();
  vr::HmdMatrix44_t mat = vrsys->GetProjectionMatrix(nEye, nearClip, farClip);
  return ConvertSteamVRMatrixToAlMat4f(mat);
}

Mat4f OpenVRWrapper::GetHMDMatrixPoseEye(vr::IVRSystem *vrsys,
                                         vr::Hmd_Eye nEye) {
  if (!vrsys)
    return al::Mat4f();
  vr::HmdMatrix34_t mat = vrsys->GetEyeToHeadTransform(nEye);
  auto matrixObj = ConvertSteamVRMatrixToAlMat4f(mat);
  al::invert(matrixObj);
  return matrixObj;
}

std::string OpenVRWrapper::GetTrackedDeviceString(
    vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice,
    vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError) {
  uint32_t requiredBufferLen =
      pHmd->GetStringTrackedDeviceProperty(unDevice, prop, nullptr, 0, peError);
  if (requiredBufferLen == 0)
    return "";
  char *pchBuffer = new char[requiredBufferLen];
  requiredBufferLen = pHmd->GetStringTrackedDeviceProperty(
      unDevice, prop, pchBuffer, requiredBufferLen, peError);
  std::string sResult = pchBuffer;
  delete[] pchBuffer;
  return sResult;
}

std::string
OpenVRWrapper::GetTrackedDeviceClassString(vr::ETrackedDeviceClass td_class) {
  std::string str_td_class = "Unknown class";
  switch (td_class) {
  case vr::TrackedDeviceClass_Invalid: // = 0, the ID was not valid.
    str_td_class = "invalid";
    break;
  case vr::TrackedDeviceClass_HMD: // = 1, Head-Mounted Displays
    str_td_class = "hmd";
    break;
  case vr::TrackedDeviceClass_Controller: // = 2, Tracked controllers
    str_td_class = "controller";
    break;
  case vr::TrackedDeviceClass_GenericTracker: // = 3, Generic trackers,
                                              // similar to controllers
    str_td_class = "generic tracker";
    break;
  case vr::TrackedDeviceClass_TrackingReference: // = 4, Camera and base
                                                 // stations that serve as
                                                 // tracking reference points
    str_td_class = "base station";
    break;
  case vr::TrackedDeviceClass_DisplayRedirect: // = 5, Accessories that
                                               // aren't necessarily tracked
                                               // themselves, but may
                                               // redirect video output from
                                               // other tracked devices
    str_td_class = "display redirect";
    break;
  }
  return str_td_class;
}

void OpenVRWrapper::process_vr_event(const vr::VREvent_t &event) {
  std::string str_td_class = GetTrackedDeviceClassString(
      vr_context->GetTrackedDeviceClass(event.trackedDeviceIndex));
  switch (event.eventType) {
  case vr::VREvent_TrackedDeviceActivated: {
    std::cout << "Device " << event.trackedDeviceIndex << " attached ("
              << str_td_class << ")" << std::endl;
    tracked_device_type[event.trackedDeviceIndex] = str_td_class;

  } break;
  case vr::VREvent_TrackedDeviceDeactivated: {
    std::cout << "Device " << event.trackedDeviceIndex << " detached ("
              << str_td_class << ")" << std::endl;
    tracked_device_type[event.trackedDeviceIndex] = "";

  } break;
  case vr::VREvent_TrackedDeviceUpdated: {
    std::cout << "Device " << event.trackedDeviceIndex << " updated ("
              << str_td_class << ")" << std::endl;
  } break;
  case vr::VREvent_ButtonPress: {
    vr::VREvent_Controller_t controller_data = event.data.controller;
    // controllers[event.trackedDeviceIndex]->buttonsPressed = true;
    switch (controller_data.button) {
    case 2:
      controllers[event.trackedDeviceIndex]->gripped = true;
      // std::cout << " gripped" << std::endl;
      break;
    case 33:
      controllers[event.trackedDeviceIndex]->triggerPressed = true;
      // std::cout << " triggerPressed" << std::endl;
      break;
    case 32:
      controllers[event.trackedDeviceIndex]->touchpadPressed = true;
      // std::cout << " padPressed" << std::endl;
      break;
    case 0:
      controllers[event.trackedDeviceIndex]->systemPressed = true;
      // std::cout << "systemPressed" << std::endl;
      break;
    }

    // std::cout << "this is controller " <<
    // controllers[event.trackedDeviceIndex]->>deviceID << std::endl;
    // std::cout
    // << "Pressed button " <<
    // vr_context->GetButtonIdNameFromEnum((vr::EVRButtonId)
    // controller_data.button) << " of device " << event.trackedDeviceIndex <<
    // "
    // (" << str_td_class << ")" << std::endl;
  } break;
  case vr::VREvent_ButtonUnpress: {
    vr::VREvent_Controller_t controller_data = event.data.controller;
    // controllers[event.trackedDeviceIndex]->buttonsPressed = false;
    // std::cout << " device " << event.trackedDeviceIndex << std::endl;
    switch (controller_data.button) {
    case 2:
      controllers[event.trackedDeviceIndex]->gripped = false;
      // std::cout << " gripp release" << std::endl;
      break;
    case 33:
      controllers[event.trackedDeviceIndex]->triggerPressed = false;
      // std::cout << " trigger press release" << std::endl;
      break;
    case 32:
      controllers[event.trackedDeviceIndex]->touchpadPressed = false;
      // std::cout << " padPress release" << std::endl;
      break;
    case 0:
      controllers[event.trackedDeviceIndex]->systemPressed = false;
      // std::cout << "systemPressed" << std::endl;
      break;
    }
    // std::cout << "Unpressed button " <<
    // vr_context->GetButtonIdNameFromEnum((vr::EVRButtonId)
    // controller_data.button) << " of device " << event.trackedDeviceIndex <<
    // "
    // (" << str_td_class << ")" << std::endl;
  } break;
  case vr::VREvent_ButtonTouch: {
    vr::VREvent_Controller_t controller_data = event.data.controller;
    // controllers[event.trackedDeviceIndex]->buttonsTouched = true;
    // std::cout << controller_data.button <<  " button touched " <<
    // std::endl;
    switch (controller_data.button) {
    case 32:
      controllers[event.trackedDeviceIndex]->touchpadTouched = true;
      // std::cout << " touchpad touched " << std::endl;
      break;
    case 33:
      controllers[event.trackedDeviceIndex]->triggerTouched = true;
      // std::cout << " trigger touched" << std::endl;
      break;
    }
    // std::cout << "Touched button " <<
    // vr_context->GetButtonIdNameFromEnum((vr::EVRButtonId)
    // controller_data.button) << " of device " << event.trackedDeviceIndex <<
    // "
    // (" << str_td_class << ")" << std::endl;
  } break;
  case vr::VREvent_ButtonUntouch: {
    vr::VREvent_Controller_t controller_data = event.data.controller;
    // controllers[event.trackedDeviceIndex]->buttonsTouched = false;
    switch (controller_data.button) {
    case 32:
      controllers[event.trackedDeviceIndex]->touchpadTouched = false;
      // std::cout << " touchpad released " << std::endl;
      break;
    case 33:
      controllers[event.trackedDeviceIndex]->triggerTouched = false;
      // std::cout << " trigger touch release" << std::endl;
      break;
    }
    // std::cout << "Untouched button " <<
    // vr_context->GetButtonIdNameFromEnum((vr::EVRButtonId)
    // controller_data.button) << " of device " << event.trackedDeviceIndex <<
    // "
    // (" << str_td_class << ")" << std::endl;
  } break;
  }
}

#endif
