add_library(tinyaes STATIC
	tiny-AES-c/aes.c
	)

target_include_directories(tinyaes PUBLIC
	tiny-AES-c
	)
