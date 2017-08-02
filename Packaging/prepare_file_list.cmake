function(prepare_file_list file_list_result)

  set(file_list)
  #This must exist in any OTB Installation. minimal or full
  if(NOT EXISTS "${SUPERBUILD_INSTALL_DIR}/bin/otbApplicationLauncherCommandLine${EXE_EXT}")
    message(
      FATAL_ERROR
      "${SUPERBUILD_INSTALL_DIR}/bin/otbApplicationLauncherCommandLine${EXE_EXT} not found."
      )
    
    return()
  endif()

  set(file_list "otbApplicationLauncherCommandLine${EXE_EXT}")

  if(HAVE_QT4)
    list(APPEND file_list "otbApplicationLauncherQt${EXE_EXT}")
  endif()

  if(HAVE_MVD)
    list(APPEND file_list "monteverdi${EXE_EXT}")
    list(APPEND file_list "mapla${EXE_EXT}")
  endif()
  
  if(HAVE_PYTHON)
    list(APPEND file_list "_otbApplication${PYMODULE_EXT}")
  endif()
    
  foreach(exe_file "iceViewer" "otbTestDriver" "SharkVersion")
    if(EXISTS "${SUPERBUILD_INSTALL_DIR}/bin/${exe_file}${EXE_EXT}")
        list(APPEND file_list "${exe_file}${EXE_EXT}")
    else()
      message(STATUS "${exe_file}${EXE_EXT} not found in ${SUPERBUILD_INSTALL_DIR}/bin. (skipping)")
    endif()
  endforeach()

  #Qt stuff
  if(HAVE_QT4)
    list(APPEND file_list "lrelease${EXE_EXT}")
    list(APPEND file_list "moc${EXE_EXT}")
    list(APPEND file_list "qmake${EXE_EXT}")
    list(APPEND file_list "rcc${EXE_EXT}")
    list(APPEND file_list "uic${EXE_EXT}")
    list(APPEND file_list "proj${EXE_EXT}")
    list(APPEND file_list "cs2cs${EXE_EXT}")
  endif()
  
  #RK: to hell with cmake targets files.
  file(GLOB ALL_EXTRA_FILES
    ${SUPERBUILD_INSTALL_DIR}/lib/*boost*${LIB_EXT}*
    ${SUPERBUILD_INSTALL_DIR}/lib/*glut*${LIB_EXT}*
    ${SUPERBUILD_INSTALL_DIR}/lib/*QtXml*${LIB_EXT}*
    ${SUPERBUILD_INSTALL_DIR}/lib/*kml*${LIB_EXT}*
      )
    foreach(EXTRA_FILE ${ALL_EXTRA_FILES})
      get_filename_component(EXTRA_FILE_name ${EXTRA_FILE} NAME)
      list(APPEND file_list "${EXTRA_FILE_name}")
    endforeach()  

    #RK: there is a bug in itk cmake files in install tree 
    #we workaround with below code
    #start hack
     file(GLOB itk_all_lib_files  
     "${SUPERBUILD_INSTALL_DIR}/${DEST_LIB_DIR}/${LIB_PREFIX}itk*${LIB_EXT}*"
     "${SUPERBUILD_INSTALL_DIR}/${DEST_LIB_DIR}/${LIB_PREFIX}ITK*${LIB_EXT}*"
     )

   foreach(itk_lib_file ${itk_all_lib_files})
     isfile_symlink("${itk_lib_file}" a_symlink itk_lib_file_target)
     if(NOT a_symlink)
       list(APPEND file_list "${itk_lib_file}")
     endif()
   endforeach()
   #end hack
   
   file(GLOB otb_test_exe_list 
     "${SUPERBUILD_INSTALL_DIR}/bin/gdal*${EXE_EXT}"
     "${OTB_BINARY_DIR}/bin/*[T|t]est*${EXE_EXT}"
     )
   
   list(REMOVE_ITEM otb_test_exe_list "${SUPERBUILD_INSTALL_DIR}/bin/gdal-config")
   list(REMOVE_ITEM otb_test_exe_list "${OTB_BINARY_DIR}/bin/otbcli_TestApplication")
   list(REMOVE_ITEM otb_test_exe_list "${OTB_BINARY_DIR}/bin/otbgui_TestApplication")

   foreach(otb_test_exe   ${otb_test_exe_list})
     get_filename_component(otb_test_exe_name ${otb_test_exe} NAME)
     list(APPEND file_list ${otb_test_exe_name})
   endforeach()

   # special case for msvc: ucrtbase.dll must be explicitly vetted.
   # for proj.dll, see Mantis-1424
   if(MSVC)
     list(APPEND file_list "ucrtbase.dll")
     list(APPEND file_list "proj.dll")
   endif()  

  set(otb_applications_dir "${SUPERBUILD_INSTALL_DIR}/lib/otb/applications")
  file(GLOB OTB_APPS_LIST "${otb_applications_dir}/otbapp_*${LIB_EXT}") # /lib/otb
  list(APPEND  file_list ${OTB_APPS_LIST})


    set(${file_list_result} ${file_list} PARENT_SCOPE)
endfunction()
