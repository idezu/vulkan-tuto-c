#pragma once

#include <vulkan/vulkan.h>

bool checkValidationLayerSupport();
void getValidationLayers(const int* count,const char*** layers);

VkResult setupDebugMessenger(VkDebugUtilsMessengerEXT* debugMessenger,VkInstance instance);

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

/*
typedef enum error_type_struct_t
	{
		TYPE_SYSTEM_ERROR, // the system has cause an error
		TYPE_USER_ERROR, // the user has made an error seting up
		TYPE_GLFW_ERROR, // glfw has failed
		TYPE_VULKAN_ERROR // vulkan has failed
	} error_type_t ;
typedef enum error_severity_struct_t
	{
		SEVERITY_WARNING,
		SEVERITY_ERROR,
		SEVERITY_FATAL
	} error_severity_t ;

struct _Error
{
	char* message; // the message of the error
	int line; // the line where the error has occured
	char* file; // the file where the error has occured
	error_type_t type; // the type of the error
	error_severity_t severity; // the severity of the error
};


typedef struct error_result_struct_t
{
	struct _Error Error;	
	enum 
		{
			RESULT_OK,// everything is ok
			RESULT_WARNING,// something not important has gone wrong
			RESULT_ERROR// something has broke
		} type;
} result_t;

static inline char* str_error_type(error_type_t type)
	{
		switch (type)
		{
			case TYPE_SYSTEM_ERROR:
				return "SYSTEM_ERROR";
			case TYPE_GLFW_ERROR:
				return "GLFW_ERROR";
			case TYPE_VULKAN_ERROR:
				return "VULKAN_ERROR";
			case TYPE_USER_ERROR:
				return "USER_ERROR";
			default:
				return "UNKNOWN_ERROR";
		}
	}

static inline char* str_error_severity(error_severity_t severity)
	{
		switch (severity)
		{
			case SEVERITY_WARNING:
				return "WARNING";
			case SEVERITY_ERROR:
				return "ERROR";
			case SEVERITY_FATAL:
				return "FATAL";

			default:
				return "UNKNOWN_SEVERITY";
		}
	}

static inline void print_Result(result_t result)
	{
		switch (result.type)
		{
			case RESULT_WARNING:
				printf_color(COLOR_YELLOW,"WARNING %s %s %s: %d: ",str_error_severity(result.Error.severity),str_error_type(result.Error.type),result.Error.file,result.Error.line);
				printf("%s\n",result.Error.message);
				return;

			case RESULT_ERROR:
				printf_color(COLOR_RED,"ERROR %s %s %s: %d: ",str_error_severity(result.Error.severity),str_error_type(result.Error.type),result.Error.file,result.Error.line);
				printf("%s\n",result.Error.message);
				return;

			default:
				printf_color(COLOR_GREEN,"what??");
				return;
		}
	}*/
