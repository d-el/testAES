cmake_minimum_required(VERSION 3.14)

PROJECT(testAES CXX C)
set(CMAKE_CXX_STANDARD 17)

set(COMMONFLAGS "-g3 -O0")

set(CMAKE_CXX_FLAGS "${CPUFLAGS} ${COMMONFLAGS} -std=c++17 -fno-rtti -fno-exceptions")
set(CMAKE_C_FLAGS "${CPUFLAGS} ${COMMONFLAGS} -std=gnu11")
	
set(CMAKE_EXE_LINKER_FLAGS "\
	${CPUFLAGS} \
	${FUSED} \
	-Xlinker --gc-sections \
	-Wl,--print-memory-usage \
	-Wl,--undefined=uxTopUsedPriority \
	-Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map \
	 ")

set(aeskeyfile ${CMAKE_CURRENT_SOURCE_DIR}/key.txt)
# Embedded files
set(PROJECT_EMBED_FILES ${aeskeyfile})
set(PROJECT_EMBED_OBJ_FILES "")
foreach(FILENAME ${PROJECT_EMBED_FILES})
    get_filename_component(FILENAME_ONLY ${FILENAME} NAME)
    get_filename_component(FILEPATH_ONLY ${FILENAME} PATH)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME_ONLY}.o 
                       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                       COMMAND ${CMAKE_OBJCOPY} 
                       ARGS -I binary -O elf64-x86-64 -B i386 ${FILENAME_ONLY} ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME_ONLY}.o
                       DEPENDS ${FILENAME_ONLY}
                       )
    list(APPEND PROJECT_EMBED_OBJ_FILES ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME_ONLY}.o)
    message(STATUS "Objcopy'ing ${FILENAME}")
endforeach(FILENAME)
 
add_subdirectory(lib/aes)

add_executable(${PROJECT_NAME}.elf main.c ${PROJECT_EMBED_OBJ_FILES})

target_link_libraries(${PROJECT_NAME}.elf
	tinyaes
	)

set(filetoencrypt ${CMAKE_CURRENT_SOURCE_DIR}/text)
file(SIZE ${filetoencrypt} filetoencryptsize)
execute_process(COMMAND printf %08x ${filetoencryptsize} OUTPUT_VARIABLE filetoencryptsize)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
	COMMAND echo C0DE1234 | xxd -r -p > ${filetoencrypt}.temp
	COMMAND echo ${filetoencryptsize} | xxd -r -p >> ${filetoencrypt}.temp
	COMMAND cat ${filetoencrypt} >> ${filetoencrypt}.temp
	COMMAND openssl enc -aes-128-cbc -K `cat ${aeskeyfile}` -iv 0 -bufsize 1024 -p -in ${filetoencrypt}.temp -out ${filetoencrypt}.enc
	COMMAND rm -f ${filetoencrypt}.temp
	COMMENT "Generate hex and bin"
	)
