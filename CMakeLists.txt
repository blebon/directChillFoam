cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

project(directChillFoam LANGUAGES CXX C)

# Display OpenFOAM version
MESSAGE(STATUS "Using OpenFOAM $ENV{WM_PROJECT_VERSION}")

# Check if required libraries are available
find_library(OpenFOAM_LIB
    NAMES OpenFOAM libOpenFOAM
    PATHS ENV FOAM_LIBBIN
    REGISTRY_VIEW $ENV{WM_ARCH_OPTION}
    REQUIRED
)
find_library(DL_LIB 
    NAMES dl libdl
    REGISTRY_VIEW $ENV{WM_ARCH_OPTION}
    REQUIRED
)
find_library(M_LIB
    NAMES m libm
    REGISTRY_VIEW $ENV{WM_ARCH_OPTION}
    REQUIRED
)

# OpenFOAM compilation options
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-m$ENV{WM_ARCH_OPTION})
add_compile_definitions($ENV{WM_ARCH})
add_compile_definitions(WM_ARCH_OPTION=$ENV{WM_ARCH_OPTION})
add_compile_definitions(WM_DP)
add_compile_definitions(WM_LABEL_SIZE=$ENV{WM_LABEL_SIZE})
add_compile_options(-Wall -Wextra -Wold-style-cast -Wnon-virtual-dtor -Wno-unused-parameter -Wno-invalid-offsetof -Wno-attributes)
add_compile_options(-O3)
add_compile_definitions(NoRepository)
# -ftemplate-depth-100
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100")
endif(CMAKE_COMPILER_IS_GNUCXX)
# -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Match path configuration of Make/options for directChillFoam
set(DIRECTCHILLFOAM_DIR applications/solvers/heatTransfer/${PROJECT_NAME})
set(MULTICOMPONENTALLOY_DIR ${DIRECTCHILLFOAM_DIR}/multicomponentAlloy)

include_directories(${MULTICOMPONENTALLOY_DIR})
include_directories(${MULTICOMPONENTALLOY_DIR}/soluteModel)
include_directories($ENV{FOAM_SRC}/finiteVolume/cfdTools)
include_directories($ENV{FOAM_SRC}/finiteVolume/lnInclude)
include_directories($ENV{FOAM_SRC}/meshTools/lnInclude)
include_directories($ENV{FOAM_SRC}/sampling/lnInclude)
include_directories($ENV{FOAM_SRC}/dynamicFvMesh/lnInclude)
include_directories($ENV{FOAM_SRC}/OpenFOAM/lnInclude)
include_directories($ENV{FOAM_SRC}/OSspecific/POSIX/lnInclude)

# Link directories
link_directories($ENV{FOAM_LIBBIN})
link_directories($ENV{FOAM_USER_LIBBIN})

# libmulticomponentAlloy
add_library(multicomponentAlloy SHARED ${MULTICOMPONENTALLOY_DIR}/soluteModel/soluteModel.C ${MULTICOMPONENTALLOY_DIR}/multicomponentAlloy.C)

target_link_libraries(multicomponentAlloy LINK_PUBLIC finiteVolume dynamicFvMesh topoChangerFvMesh meshTools sampling fvModels fvConstraints OpenFOAM dl m)
target_link_options(multicomponentAlloy PUBLIC -fuse-ld=bfd)
target_link_options(multicomponentAlloy PUBLIC LINKER:--add-needed,--no-as-needed)

# Copy libmulticomponentAlloy.so library upon successful compilation
add_custom_command(TARGET multicomponentAlloy
                   COMMENT "Copying libmulticomponentAlloy.so to $ENV{FOAM_USER_LIBBIN}"
                   POST_BUILD COMMAND cp libmulticomponentAlloy.so $ENV{FOAM_USER_LIBBIN}
)

# libmyFvConstraints
set(LIMITSOLUTE_DIR src/fvConstraints/limitSolute)
add_library(myFvConstraints SHARED ${LIMITSOLUTE_DIR}/limitSolute.C)

target_include_directories(myFvConstraints PUBLIC $ENV{FOAM_SRC}/fvConstraints/lnInclude)
target_include_directories(myFvConstraints PUBLIC $ENV{FOAM_SRC}/transportModels/lnInclude)
target_include_directories(myFvConstraints PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/solidThermo/lnInclude)
target_include_directories(myFvConstraints PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/basic/lnInclude)
target_include_directories(myFvConstraints PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/specie/lnInclude)
target_include_directories(myFvConstraints PUBLIC $ENV{FOAM_SRC}/MomentumTransportModels/momentumTransportModels/lnInclude)
target_include_directories(myFvConstraints PUBLIC $ENV{FOAM_SRC}/MomentumTransportModels/compressible/lnInclude)
target_include_directories(myFvConstraints PUBLIC $ENV{FOAM_SRC}/ThermophysicalTransportModels/lnInclude)

target_link_libraries(myFvConstraints LINK_PUBLIC multicomponentAlloy finiteVolume meshTools sampling momentumTransportModels fvConstraints compressibleMomentumTransportModels thermophysicalTransportModels OpenFOAM dl m)
target_link_options(myFvConstraints PUBLIC -fuse-ld=bfd)
target_link_options(myFvConstraints PUBLIC LINKER:--add-needed,--no-as-needed)

# Copy libmyFvConstraints.so library upon successful compilation
add_custom_command(TARGET myFvConstraints
                   COMMENT "Copying libmyFvConstraints.so to $ENV{FOAM_USER_LIBBIN}"
                   POST_BUILD COMMAND cp libmyFvConstraints.so $ENV{FOAM_USER_LIBBIN}
)

# libmyFvModels
set(MUSHYZONESOURCE_DIR src/fvModels/derived/mushyZoneSource)
add_library(myFvModels SHARED ${MUSHYZONESOURCE_DIR}/mushyZoneSource.C)

target_include_directories(myFvModels PUBLIC $ENV{FOAM_SRC}/transportModels/lnInclude)
target_include_directories(myFvModels PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/solidThermo/lnInclude)
target_include_directories(myFvModels PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/basic/lnInclude)
target_include_directories(myFvModels PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/specie/lnInclude)
target_include_directories(myFvModels PUBLIC $ENV{FOAM_SRC}/MomentumTransportModels/momentumTransportModels/lnInclude)
target_include_directories(myFvModels PUBLIC $ENV{FOAM_SRC}/MomentumTransportModels/compressible/lnInclude)
target_include_directories(myFvModels PUBLIC $ENV{FOAM_SRC}/ThermophysicalTransportModels/lnInclude)

target_link_libraries(myFvModels LINK_PUBLIC multicomponentAlloy finiteVolume fvModels meshTools sampling momentumTransportModels compressibleMomentumTransportModels thermophysicalTransportModels OpenFOAM dl m)
target_link_options(myFvModels PUBLIC -fuse-ld=bfd)
target_link_options(myFvModels PUBLIC LINKER:--add-needed,--no-as-needed)

# Copy libmyFvModels.so library upon successful compilation
add_custom_command(TARGET myFvModels
                   COMMENT "Copying libmyFvModels.so to $ENV{FOAM_USER_LIBBIN}"
                   POST_BUILD COMMAND cp libmyFvModels.so $ENV{FOAM_USER_LIBBIN}
)

# libmythermophysicalTransportModels
set(DERIVEDFVPATCHFIELDS_DIR src/ThermophysicalTransportModels/derivedFvPatchFields)
add_library(mythermophysicalTransportModels SHARED ${DERIVEDFVPATCHFIELDS_DIR}/mouldHTC/mouldHTCFvPatchScalarField.C ${DERIVEDFVPATCHFIELDS_DIR}/waterFilmHTC/waterFilmHTCFvPatchScalarField.C)

target_include_directories(mythermophysicalTransportModels PUBLIC $ENV{FOAM_SRC}/ThermophysicalTransportModels/lnInclude)
target_include_directories(mythermophysicalTransportModels PUBLIC $ENV{FOAM_SRC}/MomentumTransportModels/momentumTransportModels/lnInclude)
target_include_directories(mythermophysicalTransportModels PUBLIC $ENV{FOAM_SRC}/MomentumTransportModels/compressible/lnInclude)
target_include_directories(mythermophysicalTransportModels PUBLIC $ENV{FOAM_SRC}/transportModels/lnInclude)
target_include_directories(mythermophysicalTransportModels PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/basic/lnInclude)
target_include_directories(mythermophysicalTransportModels PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/specie/lnInclude)
target_include_directories(mythermophysicalTransportModels PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/solidThermo/lnInclude)
target_include_directories(mythermophysicalTransportModels PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/solidSpecie/lnInclude)

target_link_libraries(mythermophysicalTransportModels LINK_PUBLIC thermophysicalTransportModels fluidThermophysicalModels solidThermo momentumTransportModels specie finiteVolume meshTools OpenFOAM dl m)
target_link_options(mythermophysicalTransportModels PUBLIC -fuse-ld=bfd)
target_link_options(mythermophysicalTransportModels PUBLIC LINKER:--add-needed,--no-as-needed)

# Copy libmythermophysicalTransportModels.so library upon successful compilation
add_custom_command(TARGET myFvModels
                   COMMENT "Copying libmythermophysicalTransportModels.so to $ENV{FOAM_USER_LIBBIN}"
                   POST_BUILD COMMAND cp libmythermophysicalTransportModels.so $ENV{FOAM_USER_LIBBIN}
)

# directChillFoam
add_executable(${PROJECT_NAME} ${DIRECTCHILLFOAM_DIR}/${PROJECT_NAME}.C)

target_include_directories(${PROJECT_NAME} PUBLIC ${DIRECTCHILLFOAM_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{FOAM_APP}/solvers/compressible/rhoPimpleFoam)
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{FOAM_SRC}/transportModels/lnInclude)
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{FOAM_SRC}/thermophysicalModels/basic/lnInclude)
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{FOAM_SRC}/MomentumTransportModels/momentumTransportModels/lnInclude)
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{FOAM_SRC}/MomentumTransportModels/compressible/lnInclude)
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{FOAM_SRC}/ThermophysicalTransportModels/lnInclude)

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC multicomponentAlloy fluidThermophysicalModels specie momentumTransportModels compressibleMomentumTransportModels thermophysicalTransportModels finiteVolume dynamicFvMesh topoChangerFvMesh meshTools sampling fvModels fvConstraints OpenFOAM dl m)
target_link_options(${PROJECT_NAME} PUBLIC -fuse-ld=bfd)
target_link_options(${PROJECT_NAME} PUBLIC LINKER:--add-needed,--no-as-needed)

# Copy directChillFoam executable upon successful compilation
add_custom_command(TARGET ${PROJECT_NAME}
                   COMMENT "Copying ${PROJECT_NAME} to $ENV{FOAM_USER_APPBIN}"
                   POST_BUILD COMMAND cp ${PROJECT_NAME} $ENV{FOAM_USER_APPBIN}
)