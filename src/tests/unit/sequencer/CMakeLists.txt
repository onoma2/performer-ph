include_directories(../../../apps/sequencer)

register_test(TestCurve TestCurve.cpp)
register_test(TestScale TestScale.cpp)
register_test(TestClock TestClock.cpp)

add_subdirectory(model)
add_subdirectory(generators)
