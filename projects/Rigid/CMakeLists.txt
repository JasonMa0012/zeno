add_subdirectory(bullet3)
add_subdirectory(HACD)

file (GLOB ZEN_RIGID_SOURCE *.cpp)
add_library(zeno_Rigid SHARED ${ZEN_RIGID_SOURCE})
target_include_directories(zeno_Rigid PRIVATE .)
target_include_directories(zeno_Rigid PRIVATE bullet3/src)

target_link_libraries(zeno_Rigid PRIVATE LinearMath)
target_link_libraries(zeno_Rigid PRIVATE Bullet3Common)
target_link_libraries(zeno_Rigid PRIVATE BulletInverseDynamics)
target_link_libraries(zeno_Rigid PRIVATE BulletCollision)
target_link_libraries(zeno_Rigid PRIVATE BulletDynamics)
target_link_libraries(zeno_Rigid PRIVATE BulletSoftBody)
target_link_libraries(zeno_Rigid PRIVATE HACD)

target_link_libraries(zeno_Rigid PUBLIC zeno)
