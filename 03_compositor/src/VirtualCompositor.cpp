#include "VirtualCompositor.hpp"

VirtualCompositor::VirtualCompositor() {
	_serial = "vc_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
	
	

}

VirtualCompositor::~VirtualCompositor() {
	
}

std::shared_ptr<VirtualCompositor> VirtualCompositor::make_new()
{
	return std::shared_ptr<VirtualCompositor>(new VirtualCompositor());
}

std::string VirtualCompositor::get_serial() const
{
	return _serial;
}

void VirtualCompositor::update()
{


}

vr::TrackedDeviceIndex_t VirtualCompositor::get_index() const
{
	return _index;
}

void VirtualCompositor::process_event(const vr::VREvent_t & event)
{
}

vr::EVRInitError VirtualCompositor::Activate(vr::TrackedDeviceIndex_t index)
{
	_index = index;
	_compositor_running = true;
	_render_thread = std::thread([&]() {
		// see http://discourse.glfw.org/t/multithreading-glfw/573/5 for more info
		GLFWwindow* _window = nullptr;
		if (glfwInit()) {
			DebugDriverLog("GLFW Init success\n");
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			_window = glfwCreateWindow(640, 480, _serial.c_str(), nullptr, nullptr);
			if (_window != nullptr) {
				glfwMakeContextCurrent(_window);
				glfwSwapInterval(1);
				DebugDriverLog("Window Opened\n");

				while (_compositor_running) {
					glfwMakeContextCurrent(_window);

					int width, height;
					glfwGetFramebufferSize(_window, &width, &height);
					glViewport(0, 0, width, height);
					double time = glfwGetTime();


					glfwSwapBuffers(_window);
					glfwPollEvents();

					std::this_thread::sleep_for(std::chrono::milliseconds(16));
				}
				if (_window != nullptr) {
					glfwDestroyWindow(_window);
					_window = nullptr;
					glfwTerminate();
					DebugDriverLog("Window Closed\n");
				}
			}
			else {
				DebugDriverLog("Error opening window\n");
			}
		}
		else {
			DebugDriverLog("GLFW Init fail\n");
		}
	});



	return vr::VRInitError_None;
}

void VirtualCompositor::Deactivate()
{
	_compositor_running = false;
	_render_thread.join();
	// Clear device id
	_index = vr::k_unTrackedDeviceIndexInvalid;
}

void VirtualCompositor::EnterStandby()
{
}

void * VirtualCompositor::GetComponent(const char * component)
{
	if (std::string(component) == std::string(vr::IVRVirtualDisplay_Version))
	{
		return static_cast<vr::IVRVirtualDisplay*>(this);
	}
	return nullptr;
}

void VirtualCompositor::DebugRequest(const char * request, char * response_buffer, uint32_t response_buffer_size)
{
	// No custom debug requests defined
	if (response_buffer_size >= 1)
		response_buffer[0] = 0;
}

vr::DriverPose_t VirtualCompositor::GetPose()
{
	vr::DriverPose_t pose = { 0 };
	pose.poseIsValid = true;
	pose.result = vr::TrackingResult_Running_OK;
	pose.deviceIsConnected = true;
	pose.qWorldFromDriverRotation.w = 1;
	pose.qWorldFromDriverRotation.x = 0;
	pose.qWorldFromDriverRotation.y = 0;
	pose.qWorldFromDriverRotation.z = 0;
	pose.qDriverFromHeadRotation.w = 1;
	pose.qDriverFromHeadRotation.x = 0;
	pose.qDriverFromHeadRotation.y = 0;
	pose.qDriverFromHeadRotation.z = 0;
	return pose;
}

void VirtualCompositor::Present(const vr::PresentInfo_t * present_info, uint32_t present_info_size)
{
	
}

void VirtualCompositor::WaitForPresent()
{
}

bool VirtualCompositor::GetTimeSinceLastVsync(float * seconds_since_last_vsync, uint64_t * frame_counter)
{

	return false;
}
