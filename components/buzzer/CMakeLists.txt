set(APP_SOURCES
  ./buzzer.cpp
)

idf_component_register(SRCS ${APP_SOURCES}
  INCLUDE_DIRS "include" REQUIRES driver esp_timer)
