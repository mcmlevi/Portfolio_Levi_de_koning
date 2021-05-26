#include "pch.h"
#include <Platform/PC/PCDevice.h>
#include "Graphics/OpenGL.h"
#include <GLFW/glfw3.h>

// unnamed namespace so these utility functions can only be used in this translation unit (the same as making the functions static)
namespace 
{
	void logOpenGLVersionInfo()
	{
		const auto vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		const auto renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		const auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		const auto shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		LOGINFO("OpenGL Vendor %s", vendor);
		LOGINFO("OpenGL Renderer %s", renderer);
		LOGINFO("OpenGL Version %s", version);
		LOGINFO("OpenGL Shader Version %s", shaderVersion);
	}

} // unnamed namespace

namespace Frac 
{
	PCDevice::PCDevice(RenderSystem& renderSystem) : 
		m_renderSystem(renderSystem),
		m_backgroundColor(.5f)
	{}

	PCDevice::~PCDevice()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void PCDevice::Initialize(unsigned width, unsigned height)
	{
		m_width = width;
		m_height = height;

		if (!glfwInit())
		{
			LOGCRITICAL("GLFW init failed");
			assert(false);
			exit(EXIT_FAILURE);
		}

		LOGINFO("GLFW version %i.%i.%i", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(CONFIG_DEBUG) || defined (CONFIG_DEVELOP)	
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		m_videoMode = glfwGetVideoMode(primaryMonitor);
		LOGINFO("Resolution Width: %i Height: %i", m_videoMode->width, m_videoMode->height);
		m_window = glfwCreateWindow( width, height, "FracEngine Window", nullptr, nullptr);

		if (!m_window)
		{
			LOGCRITICAL("GLFW window could not be created");
			glfwTerminate();
			assert(false);
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(m_window);
		glfwSetFramebufferSizeCallback(m_window, PCDevice::Framebuffer_size_callback);
		glfwSwapInterval(1);
		glfwSetWindowUserPointer(m_window, this);

		int major = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MAJOR);
		int minor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MINOR);
		int revision = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_REVISION);
		LOGINFO("GLFW OpenGL context version %i.%i.%i", major, minor, revision);

		// OpenGL init here
		if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
		{
			LOGCRITICAL("GLAD failed to initialize OpenGL context");
			assert(false);
			exit(EXIT_FAILURE);
		}

		logOpenGLVersionInfo();
	}

	void PCDevice::SwapBuffers()
	{
		glfwSwapBuffers(m_window);
		glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	GLFWwindow* PCDevice::GetWindow() const
	{
		return m_window;
	}

	void PCDevice::PollEvents()
	{
		glfwPollEvents();
	}

	bool PCDevice::ShouldClose()
	{
		return glfwWindowShouldClose(m_window);
	}

	unsigned PCDevice::GetWindowWidth() const
	{
		return m_width;
	}

	unsigned PCDevice::GetWindowHeight() const
	{
		return m_height;
	}

	void PCDevice::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		Frac::PCDevice* device = reinterpret_cast<Frac::PCDevice*>(glfwGetWindowUserPointer(window));
		device->m_renderSystem.GetRenderAPI().ResizeViewport(static_cast<unsigned>(width), static_cast<unsigned>(height));
		device->m_width = width;
		device->m_height = height;
	}

} // namespace Frac