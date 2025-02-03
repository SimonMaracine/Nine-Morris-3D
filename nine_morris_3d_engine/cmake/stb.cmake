add_library(stb STATIC
    "extern/stb_image/stb_image.c"
    "extern/stb_image_resize2/stb_image_resize2.c"
    "extern/stb_image_write/stb_image_write.c"
    "extern/stb_truetype/stb_truetype.c"
    "extern/stb_vorbis/stb_vorbis.c"
)

target_include_directories(stb PUBLIC
    "extern/stb_image"
    "extern/stb_image_resize2"
    "extern/stb_image_write"
    "extern/stb_truetype"
    "extern/stb_vorbis"
)

if(NM3D_DISTRIBUTION_MODE)
    target_compile_definitions(stb PRIVATE "NDEBUG")
endif()
