cmake_minimum_required(VERSION 3.16)

project(sound_processor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOUND_PROCESSOR_SOURCES
    src/app/application.cpp
    src/args/args_parser.cpp
    src/converter/converter.cpp
    src/filter_producers/filter_producers.cpp
    src/pipeline/pipeline.cpp
    src/waveform/waveform.cpp
    src/wav/wav_io.cpp
    src/filters/ampl_filter.cpp
    src/filters/normalize_filter.cpp
    src/filters/silence_filter.cpp
    src/filters/timestretch_filter.cpp
    src/filters/lowpass_filter.cpp
    src/filters/highpass_filter.cpp
    src/filters/bandpass_filter.cpp
    src/filters/reject_filter.cpp
    src/filters/generator_filters.cpp
    src/filters/mute_filter.cpp
    src/filters/mix_filter.cpp
)

set(SOUND_PROCESSOR_HEADERS
    src/app/application.h
    src/args/args_parser.h
    src/converter/converter.h
    src/filter/filter.h
    src/filter_producers/filter_producers.h
    src/filters/ampl_filter.h
    src/filters/normalize_filter.h
    src/filters/silence_filter.h
    src/filters/timestretch_filter.h
    src/filters/lowpass_filter.h
    src/filters/highpass_filter.h
    src/filters/bandpass_filter.h
    src/filters/reject_filter.h
    src/filters/generator_filters.h
    src/filters/mute_filter.h
    src/filters/mix_filter.h
    src/pipeline/pipeline.h
    src/wav/wav_io.h
    src/wav/wav_headers.h
    src/waveform/waveform.h
)

add_executable(sound_processor
    ${SOUND_PROCESSOR_SOURCES}
    ${SOUND_PROCESSOR_HEADERS}
    src/app/main.cpp
)

target_include_directories(sound_processor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(sound_processor PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

include(CTest)

add_executable(test_sound_processor
    ${SOUND_PROCESSOR_SOURCES}
    ${SOUND_PROCESSOR_HEADERS}
    tests/sound_processor_tests/test_args_parser.cpp
    tests/sound_processor_tests/test_waveform.cpp
    tests/sound_processor_tests/test_filters.cpp
    tests/sound_processor_tests/test_main.cpp
    tests/sound_processor_tests/test_pipeline.cpp
    tests/sound_processor_tests/test_wav_io.cpp
)

target_include_directories(test_sound_processor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(test_sound_processor PRIVATE contrib_catch2_main)

target_compile_options(test_sound_processor PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(NAME test_sound_processor COMMAND test_sound_processor)