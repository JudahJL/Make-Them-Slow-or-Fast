set(FOMOD_INFO_DATA 
"<fomod> \n"
"	<Name>${PROJECT_NAME}</Name> \n"
"	<Author>${PROJECT_AUTHOR}</Author> \n"
"	<Version>${CURRENT_PROJECT_MAJOR_VER}.${CURRENT_PROJECT_MINOR_VER}.${CURRENT_PROJECT_PATCH_VER}.3</Version> \n"
"	<Website>https://www.nexusmods.com/skyrimspecialedition/mods/109355/</Website>\n"
"	<Description>Make Them Slow or Fast - A spell Patcher is a mod designed for Dodge Mods.\n"
"Helps You Change and/or limit the Speed of All Spells. Namely Only Aimed - Fire and Forget Spells(For Now).</Description>\n"
"	<Groups>\n"
"		<element>Gameplay Effects and Changes</element>\n"
"	</Groups>\n" 
"</fomod>\n"
)

if( NOT DEFINED DISTRIBUTION_DIR )
	message(FATAL_ERROR "Variable ZIP_DIR is not Defined")
endif()

if(NOT DEFINED ENV{GITHUB_ENV})
	message(STATUS "GitHub environment not detected. Executing fomod xml file modification.")
    file(WRITE "${DISTRIBUTION_DIR}/fomod/info.xml" ${FOMOD_INFO_DATA})
else()
	message(STATUS "GitHub environment detected. Skipping fomod xml file modification.")
endif()
