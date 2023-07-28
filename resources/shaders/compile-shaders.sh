#!/bin/bash
$VULKAN_SDK/Bin/glslc default.vert -o default.vert.spv
$VULKAN_SDK/Bin/glslc default.frag -o default.frag.spv
read -p "Press Enter to continue..."