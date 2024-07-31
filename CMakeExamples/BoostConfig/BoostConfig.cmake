# Явным образом указываем, где находятся библиотеки The Boost Library
set( BOOST_ROOT c:/boost/ )
set( BOOST_INCLUDEDIR c:/boost/include/boost-1_82/ )
set( BOOST_LIBRARYDIR c:/boost/lib )

# Указываем, что следует использовать статическую линковку RUNTIME 
option(MSVC_CRT_STATIC "Link .exe with /MT instead of the default /MD" OFF)
