
target_sources (main-app_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/MatrixNode.cc
	${CMAKE_CURRENT_SOURCE_DIR}/SudokuMatrix.cc

	${CMAKE_CURRENT_SOURCE_DIR}/Board.cc
	)

target_include_directories (main-app_lib PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	)
