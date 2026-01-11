idf_component_register(SRCS main.c graphic.c com.c player.c game.c flashlight.c mixkit_arcade_retro.c short-beep-tone-47916.c
                       INCLUDE_DIRS .
                       PRIV_REQUIRES esp_timer driver config lcd cursor pin sound)
# PRIV_REQUIRES ...
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
