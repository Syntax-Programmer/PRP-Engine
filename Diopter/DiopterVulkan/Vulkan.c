#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

int main() {
    VkInstance inst;
    VkApplicationInfo app_info = {.applicationVersion =
                                      VK_MAKE_VERSION(1, 0, 0),
                                  .pApplicationName = "Test",
                                  .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                  .pEngineName = "TestE",
                                  .pNext = NULL,
                                  .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  .apiVersion = VK_API_VERSION_1_3};
    VkInstanceCreateInfo info = {.flags = 0,
                                 .sType =
                                     VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                 .pNext = NULL,
                                 .enabledLayerCount = 0,
                                 .ppEnabledLayerNames = NULL,
                                 .enabledExtensionCount = 0,
                                 .ppEnabledExtensionNames = NULL,
                                 .pApplicationInfo = &app_info};

    VkResult rslt = vkCreateInstance(&info, NULL, &inst);

    uint32_t c;
    VkPhysicalDevice *d;
    rslt = vkEnumeratePhysicalDevices(inst, &c, NULL);
    if (rslt == VK_SUCCESS) {
        d = malloc(sizeof(VkPhysicalDevice) * c);
        vkEnumeratePhysicalDevices(inst, &c, d);
    }
    free(d);

    VkMemoryType x = {.}

    printf("%d %d \n", rslt, c);    
}
